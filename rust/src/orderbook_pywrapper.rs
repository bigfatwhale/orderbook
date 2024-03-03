use orderbook;

use pyo3::prelude::*;

use orderbook::OrderManager;

#[pymethods]
impl orderbook::Order {
    #[new]
    fn py_new( id : u64, 
        price    : u64, 
        volume   : u32, 
        side     : i8, 
        part_id  : String ) -> Self {
            orderbook::Order{id, price, volume, side, part_id}
        }
}

#[pyclass]
pub struct LimitOrderBook {
    pub lob: orderbook::LimitOrderBook
}

impl LimitOrderBook {
    pub fn new() -> LimitOrderBook {
        LimitOrderBook{lob: orderbook::LimitOrderBook::new()}
    }    
}

#[pymethods]
impl LimitOrderBook {
    #[new]
    fn py_new() -> Self {
        LimitOrderBook::new()
    }

    fn add_order(&mut self, order : orderbook::Order ) -> Vec<orderbook::Execution>{
        
        self.lob.add_order(order)
    }

    fn best_bid(&self) -> u64 {
        self.lob.best_bid()
    }

    fn best_ask(&self) -> u64 {
        self.lob.best_ask()
    }
}

#[pymodule]
fn rustpy_orderbook(_py: Python<'_>, m: &PyModule) -> PyResult<()> {
    m.add_class::<orderbook::Order>()?;
    m.add_class::<LimitOrderBook>()?;
    Ok(())
}