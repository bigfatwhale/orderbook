
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

pub struct Book {

    price_buckets : BTreeMap<u64, PriceBucket>

}

pub trait OrderManager {

    fn add_order( &mut self, order : Order );
    fn remove_order( &mut self, order : Order );

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

impl OrderManager for Book {

    fn add_order( &mut self, order : Order ) {
        {
            if let Some(bucket) = self.price_buckets.get_mut(&order.price) {
                bucket.add_order(order);
                return; // needed for the else clause to not complain about second borrow.
            }
        }
         
        let price = order.price;
        let price_bucket = PriceBucket::from_order(order);
        self.price_buckets.insert( price, price_bucket );
    }

    fn remove_order( &mut self, order : Order ) {
        if self.price_buckets.contains_key(&order.price) {
            let opt_bucket = self.price_buckets.get_mut(&order.price);
            opt_bucket.unwrap().remove_order(order);
        }
    }
}

impl Book {

    pub fn new() -> Book {
        Book{ price_buckets : BTreeMap::new() }
    }

    pub fn volume_at_price_level( &self, price : u64 ) -> u32 {
        if let Some(b) = self.price_buckets.get(&price) {
            b.volume()
        } else {
            0
        }
    }
}





