
#[macro_use]
extern crate nom;

use nom::IResult;

use std::str::FromStr;
use std::result::Result;
use std::convert::Into;

#[cfg(test)]
mod tests {
    //use super::parse_auction_summary;
    use super::AuctionSummaryMsg;
    use super::AuctionUpdateMsg;
    use super::AddOrderMsg;
    use super::OrderCancelMsg;
    use super::OrderExecutedMsg;
    use super::RetailPriceImproveMsg;
    use super::TradeBreakMsg;
    use super::TradeMsg;
    use super::TradingStatusMsg;
    use super::BATSMsgFactory;
    use std::env;
    use std::fs::File;
    use std::io::BufRead;
    use std::io::BufReader;

    #[test]
    fn test_parse_auction_summary() {
        let msg = "28800168JAAPLSPOTC00010068000000020000";
        let res = AuctionSummaryMsg::parse_msg(msg);
        println!("{:?}", res);
        assert!(res.is_ok());
    }

    #[test]
    fn test_parse_add_order() {
        let msg = "28800168A1K27GA00000YS000100AAPL  0001831900Y";
        let msg_long = "28800169d1K27GA00000YS000100AAPL  0001831900YBAML";

        let res = AddOrderMsg::parse_msg(msg);
        println!("{:?}", res);
        assert!(res.is_ok());

        let o = res.unwrap();        
        assert_eq!(o.timestamp, 28800168);
        assert_eq!( o.msg_type, 'A');
        assert_eq!( o.order_id,  204969015920664610);
        assert_eq!( o.side,     'S');
        assert_eq!( o.shares,   100);
        assert_eq!( o.symbol,   "AAPL  ");
        assert_eq!( o.price,    1831900);
        assert_eq!( o.display,  'Y');

        let res = AddOrderMsg::parse_msg(msg_long);
        println!("{:?}", res);
        assert!(res.is_ok());
    }

    #[test]
    fn test_parse_auction_update() {
        let msg = "28800168IAAPLSPOTC00010068000000020000000001000000015034000001309800"; 
        let res = AuctionUpdateMsg::parse_msg(msg);
        println!("{:?}", res);
        assert!(res.is_ok());    
    }

    #[test]
    fn test_parse_order_cancel() {
        let msg = "28800168X1K27GA00000Y000500"; 
        let res = OrderCancelMsg::parse_msg(msg);
        println!("{:?}", res);
        assert!(res.is_ok());    
    }

    #[test]
    fn test_parse_order_executed() {
        let msg = "28800168E1K27GA00000Y0001001K27GA00000K"; 
        let res = OrderExecutedMsg::parse_msg(msg);
        println!("{:?}", res);
        assert!(res.is_ok());    
    }

    #[test]
    fn test_parse_retail_price_improve() {
        let msg = "28800168RAAPLSPOTS"; 
        let res = RetailPriceImproveMsg::parse_msg(msg);
        println!("{:?}", res);
        assert!(res.is_ok());    
    }

    #[test]
    fn test_parse_trade_break() {
        let msg = "28800168B1K27GA00000Y"; 
        let res = TradeBreakMsg::parse_msg(msg);
        println!("{:?}", res);
        assert!(res.is_ok());    
    }

    #[test]
    fn test_parse_trade() {
        let msg = "28800168P1K27GA00000YB000300AAPL  00018319001K27GA00000Z"; 
        let res = TradeMsg::parse_msg(msg);
        println!("{:?}", res);
        assert!(res.is_ok());    

        let msg = "28800168r1K27GA00000YB000300AAPLSPOT00018319001K27GA00000Z"; 
        let res = TradeMsg::parse_msg(msg);
        println!("{:?}", res);
        assert!(res.is_ok());    
    }

    #[test]
    fn test_parse_trade_status() {
        let msg = "28800168HAAPLSPOTT0XY"; 
        let res = TradingStatusMsg::parse_msg(msg);
        println!("{:?}", res);
        assert!(res.is_ok());    
    }

    #[test]
    fn test_parse_file() {
        let path = env::current_dir().unwrap();
        
        println!("The current directory is {}", path.display());
        let filename = "src/pitch_example_data";
        let f = File::open(filename).expect("file not found");
        let f = BufReader::new(f);

        for line in f.lines() {
            let msg = line.unwrap();
            let buf = msg.as_str();
            if buf.chars().nth(8) == Some('J') {
                let res = AuctionSummaryMsg::parse_msg(buf);
                println!("{:?}", res);
                assert!(res.is_ok());
                break;
            }
        }
    }

    #[test]
    fn test_factory() {
        let obj = BATSMsgFactory::parse("28800168A1K27GA00000YS000100AAPL  0001831900Y");
        println!("Return result from msg factory {:?}", obj);
        let msg_obj : Option<AddOrderMsg> = obj.into();
        assert!(msg_obj.is_some());
        println!("After into {:?}", msg_obj);

        let obj = BATSMsgFactory::parse("28800168JAAPLSPOTC00010068000000020000");
        println!("Return result from msg factory {:?}", obj);
        let msg_obj : Option<AuctionSummaryMsg> = obj.into();
        println!("After into {:?}", msg_obj);
        assert!(msg_obj.is_some());
    }
}

macro_rules! create_into_function {
    ($objname : ident) => (
        impl Into<Option<$objname>> for BATSMessage {
            fn into(self) -> Option<$objname> {
                match self {
                    BATSMessage::$objname(u) => Some(u), 
                    _ => None,
                }
            }
        }  
    )
}

macro_rules! create_parse_impl {
    ($objname : ident, $parse_func : ident) => (
        impl $objname {
            pub fn parse_msg( msg : &str ) -> Result<$objname, nom::Err<&str>> {
                let o = $parse_func(msg);
                if o.is_ok() {
                    Ok(o.unwrap().1)
                } else {
                    Err(o.unwrap_err())
                }
            }
        }
    )
}

#[derive(Debug)]
pub enum BATSMessage { // For implementing message factory
    AuctionSummaryMsg(AuctionSummaryMsg), 
    AddOrderMsg(AddOrderMsg),
    AuctionUpdateMsg(AuctionUpdateMsg),
    OrderCancelMsg(OrderCancelMsg),
    OrderExecutedMsg(OrderExecutedMsg),
    RetailPriceImproveMsg(RetailPriceImproveMsg),
    TradeBreakMsg(TradeBreakMsg), 
    TradeMsg(TradeMsg),
    TradingStatusMsg(TradingStatusMsg)
}

// use macros to generate into functions for all msgs
create_into_function!(AddOrderMsg);
create_into_function!(AuctionSummaryMsg);
create_into_function!(AuctionUpdateMsg);
create_into_function!(OrderCancelMsg);
create_into_function!(OrderExecutedMsg);
create_into_function!(RetailPriceImproveMsg);
create_into_function!(TradeBreakMsg);
create_into_function!(TradeMsg);
create_into_function!(TradingStatusMsg);

// use macros to generate impl parse_msg functions for all msgs
create_parse_impl!(AddOrderMsg, parse_add_order);
create_parse_impl!(AuctionSummaryMsg, parse_auction_summary);
create_parse_impl!(AuctionUpdateMsg, parse_auction_update);
create_parse_impl!(OrderCancelMsg, parse_order_cancel);
create_parse_impl!(OrderExecutedMsg, parse_order_executed);
create_parse_impl!(RetailPriceImproveMsg, parse_retail_price_improve);
create_parse_impl!(TradeBreakMsg, parse_trade_break);
create_parse_impl!(TradeMsg, parse_trade);
create_parse_impl!(TradingStatusMsg, parse_trading_status);

pub struct BATSMsgFactory {} // this coupled with impl below makes it like a 
                             // factory method exposed via a static class method.
impl BATSMsgFactory {
    pub fn parse( msg : &str ) -> BATSMessage {
        let code = &msg[8..9];
        let obj = match code {
            "A" => BATSMessage::AddOrderMsg( AddOrderMsg::parse_msg(msg).unwrap() ), 
            "d" => BATSMessage::AddOrderMsg( AddOrderMsg::parse_msg(msg).unwrap() ),
            "J" => BATSMessage::AuctionSummaryMsg( AuctionSummaryMsg::parse_msg(msg).unwrap() ),
            "I" => BATSMessage::AuctionUpdateMsg( AuctionUpdateMsg::parse_msg(msg).unwrap() ),
            "X" => BATSMessage::OrderCancelMsg( OrderCancelMsg::parse_msg(msg).unwrap() ),
            "E" => BATSMessage::OrderExecutedMsg( OrderExecutedMsg::parse_msg(msg).unwrap() ),
            "R" => BATSMessage::RetailPriceImproveMsg( RetailPriceImproveMsg::parse_msg(msg).unwrap() ),
            "B" => BATSMessage::TradeBreakMsg( TradeBreakMsg::parse_msg(msg).unwrap() ),
            "P" => BATSMessage::TradeMsg( TradeMsg::parse_msg(msg).unwrap() ),
            "r" => BATSMessage::TradeMsg( TradeMsg::parse_msg(msg).unwrap() ),
            "H" => BATSMessage::TradingStatusMsg( TradingStatusMsg::parse_msg(msg).unwrap() ),
            &_ => unimplemented!(),
        };
        obj
    }
}

#[derive(Debug)]
pub struct AuctionSummaryMsg {
    timestamp    : u32, 
    msg_type     : char,
    symbol       : String, 
    auction_type : char, 
    price        : u64, 
    shares       : u32
}

#[derive(Debug)]
pub struct AddOrderMsg {
    timestamp : u32, 
    msg_type  : char,
    order_id  : u64, 
    side      : char, 
    shares    : u32, 
    symbol    : String,  
    price     : u64, 
    display   : char,
    part_id   : String  
}

#[derive(Debug)]
pub struct AuctionUpdateMsg {
    timestamp          : u32, 
    msg_type           : char,
    symbol             : String,
    auction_type       : char,
    reference_price    : u64,
    buyshares          : u32, 
    sellshares         : u32, 
    indicative_price   : u64, 
    auction_only_price : u64
}

#[derive(Debug)]
pub struct OrderCancelMsg {
    timestamp : u32, 
    msg_type  : char,
    order_id  : u64, 
    shares    : u32
}

#[derive(Debug)]
pub struct OrderExecutedMsg {
    timestamp : u32, 
    msg_type  : char,
    order_id  : u64, 
    shares    : u32, 
    exec_id   : u64
}

#[derive(Debug)]
pub struct RetailPriceImproveMsg {
    timestamp            : u32, 
    msg_type             : char,
    symbol               : String, 
    retail_price_improve : char
}

#[derive(Debug)]
pub struct TradeBreakMsg {
    timestamp : u32, 
    msg_type  : char,
    exec_id   : u64
}

#[derive(Debug)]
pub struct TradeMsg {
    timestamp : u32, 
    msg_type  : char,
    order_id  : u64,
    side      : char,
    shares    : u32, 
    symbol    : String, 
    price     : u64,
    exec_id   : u64
}

#[derive(Debug)]
pub struct TradingStatusMsg {
    timestamp      : u32, 
    msg_type       : char,
    symbol         : String, 
    halt_status    : char, 
    reg_sho_action : u8, 
    reserved1      : char, 
    reserved2      : char 
}

fn from_base36(input: &str) -> Result<u64, std::num::ParseIntError> {
    u64::from_str_radix(input, 36)
}

fn parse_opt_part_id( input : &str ) -> IResult<&str, String>
{
    if input.is_empty() {
        Ok(("", String::from("")))
    } else {
        let (first, last) = input.split_at(4);
        Ok((last, String::from(first)))
    }
}

named!(parse_auction_summary<&str, AuctionSummaryMsg>,  
    do_parse!(
        _1 : map_res!(take!(8),  FromStr::from_str) >>
        _2 : char!('J')                             >>
        _3 : map_res!(take!(8),  FromStr::from_str) >>
        _4 : map_res!(take!(1),  FromStr::from_str) >>
        _5 : map_res!(take!(10), FromStr::from_str) >>
        _6 : map_res!(take!(10), FromStr::from_str) >>
        (AuctionSummaryMsg{ timestamp    : _1, 
                            msg_type     : _2, 
                            symbol       : _3, 
                            auction_type : _4, 
                            price        : _5, 
                            shares       : _6 
                       } )  
    )
);

named!(parse_add_order<&str, AddOrderMsg>,  
    do_parse!(
        _1 : map_res!(take!(8),  FromStr::from_str) >>
        _2 : alt!(char!('A') | char!('d'))          >>
        _3 : map_res!(take!(12), from_base36)       >>
        _4 : map_res!(take!(1),  FromStr::from_str) >>
        _5 : map_res!(take!(6),  FromStr::from_str) >>
        _6 : map_res!(take!(6),  FromStr::from_str) >>
        _7 : map_res!(take!(10), FromStr::from_str) >>
        _8 : map_res!(take!(1),  FromStr::from_str) >>
        _9 : parse_opt_part_id                      >>
        (AddOrderMsg{ timestamp : _1, 
                      msg_type  : _2, 
                      order_id  : _3, 
                      side      : _4,
                      shares    : _5, 
                      symbol    : _6, 
                      price     : _7, 
                      display   : _8,
                      part_id   : _9
                    })  
    )
);

named!(parse_auction_update<&str, AuctionUpdateMsg>,  
    do_parse!(
        _1 : map_res!(take!(8),  FromStr::from_str)                  >>
        _2 : char!('I')                                              >>
        _3 : map_res!(take!(8), FromStr::from_str)                   >>
        _4 : alt!(char!('O') | char!('C') | char!('H') | char!('I')) >>
        _5 : map_res!(take!(10),  FromStr::from_str)                 >>
        _6 : map_res!(take!(10),  FromStr::from_str)                 >>
        _7 : map_res!(take!(10), FromStr::from_str)                  >>
        _8 : map_res!(take!(10),  FromStr::from_str)                 >>
        _9 : map_res!(take!(10), FromStr::from_str)                  >>
        (AuctionUpdateMsg{ timestamp          : _1, 
                           msg_type           : _2, 
                           symbol             : _3, 
                           auction_type       : _4,
                           reference_price    : _5, 
                           buyshares          : _6, 
                           sellshares         : _7, 
                           indicative_price   : _8,
                           auction_only_price : _9
                    })  
    )
);

named!(parse_order_cancel<&str, OrderCancelMsg>,  
    do_parse!(
        _1 : map_res!(take!(8),  FromStr::from_str) >>
        _2 : char!('X')                             >>
        _3 : map_res!(take!(12), from_base36)       >>
        _4 : map_res!(take!(6),  FromStr::from_str) >>
        (OrderCancelMsg{ timestamp : _1, 
                         msg_type  : _2, 
                         order_id  : _3, 
                         shares    : _4,
                    })  
    )
);

named!(parse_order_executed<&str, OrderExecutedMsg>,  
    do_parse!(
        _1 : map_res!(take!(8),  FromStr::from_str) >>
        _2 : char!('E')                             >>
        _3 : map_res!(take!(12), from_base36)       >>
        _4 : map_res!(take!(6),  FromStr::from_str) >>
        _5 : map_res!(take!(12), from_base36)       >>
        (OrderExecutedMsg{ timestamp : _1, 
                         msg_type    : _2, 
                         order_id    : _3, 
                         shares      : _4,
                         exec_id     : _5
                    })  
    )
);

named!(parse_retail_price_improve<&str, RetailPriceImproveMsg>,  
    do_parse!(
        _1 : map_res!(take!(8), FromStr::from_str)                   >>
        _2 : char!('R')                                              >>
        _3 : map_res!(take!(8), FromStr::from_str)                   >>
        _4 : alt!(char!('B') | char!('A') | char!('S') | char!('N')) >>
        
        (RetailPriceImproveMsg{ timestamp            : _1, 
                                msg_type             : _2, 
                                symbol               : _3, 
                                retail_price_improve : _4,
                         
                    })  
    )
);

named!(parse_trade_break<&str, TradeBreakMsg>,  
    do_parse!(
        _1 : map_res!(take!(8), FromStr::from_str) >>
        _2 : char!('B')                            >>
        _3 : map_res!(take!(12), from_base36)      >>  
        (TradeBreakMsg{ timestamp : _1, 
                        msg_type  : _2, 
                        exec_id   : _3, 
                    })  
    )
);

named!(parse_trade<&str, TradeMsg>,  
    do_parse!(
        _1 : map_res!(take!(8),  FromStr::from_str) >>
        _2 : alt!(char!('P') | char!('r') )         >>
        _3 : map_res!(take!(12), from_base36)       >>  
        _4 : alt!(char!('B') | char!('S') )         >>
        _5 : map_res!(take!(6),  FromStr::from_str) >>
        _6 : map_res!(take!( if _2 == 'P' {6} else {8} ),  
                                 FromStr::from_str) >>
        _7 : map_res!(take!(10), FromStr::from_str) >>
        _8 : map_res!(take!(12), from_base36)       >>  
        (TradeMsg{ timestamp : _1, 
                   msg_type  : _2, 
                   order_id  : _3, 
                   side      : _4, 
                   shares    : _5, 
                   symbol    : _6, 
                   price     : _7, 
                   exec_id   : _8, 
                    })  
    )
);

named!(parse_trading_status<&str, TradingStatusMsg>,  
    do_parse!(
        _1 : map_res!(take!(8), FromStr::from_str)      >>
        _2 : char!('H')                                 >>
        _3 : map_res!(take!(8), FromStr::from_str)      >>  
        _4 : alt!(char!('H') | char!('Q') | char!('T')) >>  
        _5 : map_res!(take!(1), FromStr::from_str)      >>
        _6 : map_res!(take!(1), FromStr::from_str)      >>
        _7 : map_res!(take!(1), FromStr::from_str)      >>
        (TradingStatusMsg{ timestamp      : _1, 
                           msg_type       : _2, 
                           symbol         : _3, 
                           halt_status    : _4, 
                           reg_sho_action : _5, 
                           reserved1      : _6, 
                           reserved2      : _7
                    })  
    )
);



