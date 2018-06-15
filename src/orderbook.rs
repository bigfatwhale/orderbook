
use std::collections::BTreeMap;
use std::collections::btree_map;
use std::slice;
use std::iter::{Iterator, IntoIterator};
use std::fmt;

#[derive(Clone)]
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

impl<'a> BookOrderIterMut<'a> {

    pub fn new<B:PriceBucketIter>( book: &'a mut B ) {

    }
}

pub trait PriceBucketIter {
    // iterates through orders in price-time order.
    fn iter_mut(&mut self) -> btree_map::IterMut<u64, PriceBucket>;
}

struct WrappedIterMut<T> {
    iter : T
}

impl<T> Iterator for WrappedIterMut<T> {
    fn next(&mut self) -> (u64, PriceBucket) {
        self.iter.next()
    }
}

impl PriceBucketIter for AskBook {
//    fn iter_mut(&mut self) -> btree_map::IterMut<u64, PriceBucket> {
    fn iter_mut(&mut self) -> WrappedIterMut {
        WrappedIterMut{ iter : self.price_buckets.iter_mut() }
    }
}

impl PriceBucketIter for BidBook {
//    fn iter_mut(&mut self) -> btree_map::IterMut<u64, PriceBucket> {
    fn iter_mut(&mut self) -> WrappedIterMut {
        WrappedIterMut{ iter : self.price_buckets.iter_mut().rev().into_iter() }
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

    fn check_and_do_cross_spread_walk<B1, B2: BestPrice + OrderManager>
        ( mut order : Order, 
               book : &mut B1, 
           opp_book : &mut B2, 
               func : fn(u64, u64) -> bool ) {
        if opp_book.best_price() > 0 && func( order.price, opp_book.best_price() ) {
            let residual_volume = LimitOrderBook::cross_spread_walk(order, opp_book, func);
            order.volume = residual_volume;
        }
    }

    fn cross_spread_walk<B:OrderManager>( mut order : Order, book : &mut B, 
                          func : fn(u64, u64) -> bool ) -> u32 {
        let volume = order.volume;
        let orders_to_remove : Vec<Order> = Vec::new();



        while volume > 0 && func(1, 10) {

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
            self.ask_book.add_order(order)
        }
        else {
            self.bid_book.add_order(order)
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




