
use std::collections::BTreeMap;
use std::collections::btree_map;
use std::slice;
use std::iter::{Iterator, IntoIterator};
use std::fmt;
use std::iter::Rev;

#[derive(Clone, Debug)]
pub struct Order {
    pub order_id : u64, 
    pub price    : u64, 
    pub volume   : u32, 
    pub side     : i8, 
    pub part_id  : String, 
}

pub struct PriceBucket {

    pub price_level : u64, 
    orders      : Vec<Order>, 

}

impl fmt::Debug for PriceBucket {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "PriceBucket {{ price_level: {} }}", self.price_level)
    }
}

pub struct LimitOrderBook {

    ask_book : AskBook, 
    bid_book : BidBook

}

pub trait OrderManager {

    fn add_order( &mut self, order : Order );
    fn remove_order( &mut self, order : Order );

}

pub trait BestPrice {
    fn best_price( &self ) -> u64;
}

impl OrderManager for PriceBucket {

    fn add_order( &mut self, order : Order ) {
        self.orders.push(order);
    }

    fn remove_order( &mut self, order : Order ) {
        // just do linear scan for now, worry about performance later.
        let idx  = self.orders.iter().position(|x| x.order_id == order.order_id );
        if idx.is_some() {
            self.orders.remove(idx.unwrap());
        }
    }
}

impl PriceBucket {

    pub fn from_price(price_level : u64) -> PriceBucket {
        PriceBucket{ price_level : price_level, orders : Vec::new() }
    } 

    pub fn from_order(order : Order) -> PriceBucket {
        PriceBucket{ price_level : order.price, orders : vec![order] }
    } 

    pub fn volume(&self) -> u32 {
        self.orders.iter().map(|x| x.volume ).sum()
    } 
}

#[macro_export]
macro_rules! expand_book_struct {

    ($book_struct_name : ident) => (

        pub struct $book_struct_name {
            price_buckets : BTreeMap<u64, PriceBucket>
        }

        impl $book_struct_name {

            pub fn new() -> $book_struct_name {
                $book_struct_name{ price_buckets : BTreeMap::new() }
            }

            pub fn volume_at_price_level( &self, price : u64 ) -> u32 {
                if let Some(b) = self.price_buckets.get(&price) {
                    b.volume()
                } else {0}
            }
        }

        impl OrderManager for $book_struct_name  {

            fn add_order( &mut self, order : Order ) {
                {   // scope here needed for the else clause to not complain about second borrow.
                    if let Some(bucket) = self.price_buckets.get_mut(&order.price) {
                        bucket.add_order(order);
                        return; 
                    }
                }
                // else - this is the else clause...
                let price = order.price;
                let price_bucket = PriceBucket::from_order(order);
                self.price_buckets.insert( price, price_bucket );
            }

            fn remove_order( &mut self, order : Order ) {
                if let Some(bucket) = self.price_buckets.get_mut(&order.price) {
                    bucket.remove_order(order);
                }
            }


        }
    )
}

expand_book_struct!(BidBook); 
expand_book_struct!(AskBook);

pub struct BookOrderIterMut<'a> {

    outer : btree_map::IterMut<'a, u64, PriceBucket>, 
    inner : slice::IterMut<'a, Order>

}

pub enum IterVariant<'a> {
    // all this trouble because iter_mut().rev() returns not an IterMut
    // but a Rev(IterMut)! argh.
    AskBookIter(btree_map::IterMut<'a, u64, PriceBucket>), 
    BidBookIter(Rev<btree_map::IterMut<'a, u64, PriceBucket>>), 
    None
}

pub trait PriceBucketIter {
    // iterates through orders in price-time order.
    fn iter_mut(&mut self) -> IterVariant;
}

impl PriceBucketIter for AskBook {
    fn iter_mut(&mut self) -> IterVariant {
        IterVariant::AskBookIter( self.price_buckets.iter_mut() )
    }
}

impl PriceBucketIter for BidBook {
    fn iter_mut(&mut self) -> IterVariant {
        IterVariant::BidBookIter( self.price_buckets.iter_mut().rev() )
    }
}

impl BestPrice for AskBook {
    fn best_price(&self) -> u64 { // best price for ask is the min price
        if let Some(&price) = self.price_buckets.keys().nth(0) {
            price
        } else {0}
    }
}

impl BestPrice for BidBook {
    fn best_price(&self) -> u64 { // best price for bid is the max price
        if let Some(&price) = self.price_buckets.keys().last() {
            price
        } else {0}
    }
}

impl LimitOrderBook {
    pub fn new() -> LimitOrderBook {
        LimitOrderBook{ ask_book : AskBook::new(), bid_book : BidBook::new() }
    }

    pub fn best_bid(&self) -> u64 { return self.bid_book.best_price() }
    pub fn best_ask(&self) -> u64 { return self.ask_book.best_price() }

    fn check_and_do_cross_spread_walk<B1 : OrderManager, B2: BestPrice + OrderManager + PriceBucketIter>
        ( mut order : Order, 
               book : &mut B1, 
           opp_book : &mut B2, 
               func : fn(u64, u64) -> bool ) {
        if opp_book.best_price() > 0 && func( order.price, opp_book.best_price() ) {
            let residual_volume = LimitOrderBook::cross_spread_walk(&mut order, opp_book, func);
            order.volume = residual_volume;
        }

        // if order.volume is still +ve, the can be either there is no cross-spread walk done
        // or the cross-spread walk only filled part of the volume. In that case we continue to
        // add the left-over volume in a new order.
        if order.volume > 0 {
            book.add_order(order)
        }
    }

    fn cross_spread_walk<B:OrderManager+PriceBucketIter>
        ( order : &mut Order, book : &mut B, func : fn(u64, u64) -> bool ) -> u32 {
        let mut volume = order.volume;
        let mut orders_to_remove : Vec<&Order> = Vec::new();

        let price_bucket_iter = book.iter_mut();

        let mut it : Box<Iterator<Item=(&u64, &mut PriceBucket)>> = match price_bucket_iter {
            IterVariant::AskBookIter(x) => Box::new(x.into_iter()),
            IterVariant::BidBookIter(y) => Box::new(y.into_iter()),
            _ => unimplemented!()
        };

        for bucket_order in it.flat_map(|x| x.1.orders.iter_mut()) {

            if ! (volume > 0 && func(order.price, bucket_order.price) ) {
                break;
            }

            if volume >= bucket_order.volume {
                println!("Taking {} from order id {}, left {}", 
                         bucket_order.volume, bucket_order.order_id, volume - bucket_order.volume);
                volume -= bucket_order.volume;
                bucket_order.volume = 0;
                orders_to_remove.push(bucket_order);
            } else {
                bucket_order.volume -= volume;
                volume = 0;
            }
        }

        for o in orders_to_remove {
            book.remove_order(o);
        }

        volume
    }

    pub fn ask_iter(&mut self) -> btree_map::IterMut<u64, PriceBucket> {
        self.ask_book.price_buckets.iter_mut()
    }

    pub fn bid_iter(&mut self) -> btree_map::IterMut<u64, PriceBucket> {
        self.bid_book.price_buckets.iter_mut()
    }
}

impl OrderManager for LimitOrderBook {

    fn add_order( &mut self, order : Order ) {
        if order.side == -1 {
            //self.ask_book.add_order(order)
            LimitOrderBook::check_and_do_cross_spread_walk(
                order, &mut self.ask_book, &mut self.bid_book, |x, y| x <= y)
        }
        else {
            //self.bid_book.add_order(order)
            LimitOrderBook::check_and_do_cross_spread_walk(
                order, &mut self.bid_book, &mut self.ask_book, |x, y| x >= y)
        }
    }

    fn remove_order( &mut self, order : Order ) {
        if order.side == -1 {
            self.ask_book.remove_order(order)
        }
        else {
            self.bid_book.remove_order(order)
        }
    }
}




