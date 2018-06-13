
use std::collections::BTreeMap;

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
