
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

	pub fn new(price_level : u64) -> PriceBucket {
		PriceBucket{ price_level : price_level, orders : Vec::new() }
	} 

	pub fn volume(&self) -> u32 {
		self.orders.iter().map(|x| x.volume ).sum()
	} 
}


