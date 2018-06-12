
use std::collections::BTreeMap;

pub struct Order {
	pub order_id : u64, 
	pub price    : u64, 
	pub volume   : u32, 
	pub side     : i8, 
	pub part_id  : String, 
}

trait OrderManager {

	fn add_order( &mut self, order : Order );
	fn remove_order( &mut self, order : Order );

}

pub struct PriceBucket {

	price_level : u64, 
	orders      : Vec<Order>, 

}

pub struct Book {

	price_buckets : BTreeMap<u64, PriceBucket>

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

