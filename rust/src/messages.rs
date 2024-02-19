use nom::{
    branch::alt,
    bytes::complete::take,
    character::complete::{anychar, char},
    combinator::{map_res, opt},
    error::{Error, ErrorKind},
    sequence::tuple,
    Finish, IResult,
};

use typenum::{U1, U10, U4, U6, U8};

use std;
use std::convert::Into;
use std::result::Result;
use std::str::FromStr;

macro_rules! create_into_function {
    ($objname : ident) => {
        impl Into<Option<$objname>> for BATSMessage {
            fn into(self) -> Option<$objname> {
                match self {
                    BATSMessage::$objname(u) => Some(u),
                    _ => None,
                }
            }
        }
    };
}

macro_rules! create_parse_impl {
    ($objname : ident, $parse_func : ident) => {
        impl $objname {
            pub fn parse_msg(msg: &str) -> Result<(&str, $objname), Error<&str>> {
                let o = $parse_func(msg);
                o.finish()
            }
        }
    };
}

#[derive(Debug)]
pub enum BATSMessage {
    // For implementing message factory
    AuctionSummaryMsg(AuctionSummaryMsg),
    AddOrderMsg(AddOrderMsg),
    AuctionUpdateMsg(AuctionUpdateMsg),
    OrderCancelMsg(OrderCancelMsg),
    OrderExecutedMsg(OrderExecutedMsg),
    RetailPriceImproveMsg(RetailPriceImproveMsg),
    TradeBreakMsg(TradeBreakMsg),
    TradeMsg(TradeMsg),
    TradingStatusMsg(TradingStatusMsg),
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
    pub fn parse(msg: &str) -> BATSMessage {
        let code = &msg[8..9];
        let obj = match code {
            "A" => BATSMessage::AddOrderMsg(AddOrderMsg::parse_msg(msg).unwrap().1),
            "d" => BATSMessage::AddOrderMsg(AddOrderMsg::parse_msg(msg).unwrap().1),
            "J" => BATSMessage::AuctionSummaryMsg(AuctionSummaryMsg::parse_msg(msg).unwrap().1),
            "I" => BATSMessage::AuctionUpdateMsg(AuctionUpdateMsg::parse_msg(msg).unwrap().1),
            "X" => BATSMessage::OrderCancelMsg(OrderCancelMsg::parse_msg(msg).unwrap().1),
            "E" => BATSMessage::OrderExecutedMsg(OrderExecutedMsg::parse_msg(msg).unwrap().1),
            "R" => BATSMessage::RetailPriceImproveMsg(RetailPriceImproveMsg::parse_msg(msg).unwrap().1),
            "B" => BATSMessage::TradeBreakMsg(TradeBreakMsg::parse_msg(msg).unwrap().1),
            "P" => BATSMessage::TradeMsg(TradeMsg::parse_msg(msg).unwrap().1),
            "r" => BATSMessage::TradeMsg(TradeMsg::parse_msg(msg).unwrap().1),
            "H" => BATSMessage::TradingStatusMsg(TradingStatusMsg::parse_msg(msg).unwrap().1),
            &_ => unimplemented!(),
        };
        obj
    }
}

#[derive(Debug)]
pub struct AuctionSummaryMsg {
    pub timestamp: u32,
    pub msg_type: char,
    pub symbol: String,
    pub auction_type: char,
    pub price: u64,
    pub shares: u32,
}

#[derive(Debug)]
pub struct AddOrderMsg {
    pub timestamp: u32,
    pub msg_type: char,
    pub id: u64,
    pub side: char,
    pub shares: u32,
    pub symbol: String,
    pub price: u64,
    pub display: char,
    pub part_id: String,
}

#[derive(Debug)]
pub struct AuctionUpdateMsg {
    pub timestamp: u32,
    pub msg_type: char,
    pub symbol: String,
    pub auction_type: char,
    pub reference_price: u64,
    pub buyshares: u32,
    pub sellshares: u32,
    pub indicative_price: u64,
    pub auction_only_price: u64,
}

#[derive(Debug)]
pub struct OrderCancelMsg {
    pub timestamp: u32,
    pub msg_type: char,
    pub id: u64,
    pub shares: u32,
}

#[derive(Debug)]
pub struct OrderExecutedMsg {
    pub timestamp: u32,
    pub msg_type: char,
    pub id: u64,
    pub shares: u32,
    pub exec_id: u64,
}

#[derive(Debug)]
pub struct RetailPriceImproveMsg {
    pub timestamp: u32,
    pub msg_type: char,
    pub symbol: String,
    pub retail_price_improve: char,
}

#[derive(Debug)]
pub struct TradeBreakMsg {
    pub timestamp: u32,
    pub msg_type: char,
    pub exec_id: u64,
}

#[derive(Debug)]
pub struct TradeMsg {
    pub timestamp: u32,
    pub msg_type: char,
    pub id: u64,
    pub side: char,
    pub shares: u32,
    pub symbol: String,
    pub price: u64,
    pub exec_id: u64,
}

#[derive(Debug)]
pub struct TradingStatusMsg {
    pub timestamp: u32,
    pub msg_type: char,
    pub symbol: String,
    pub halt_status: char,
    pub reg_sho_action: u8,
    pub reserved1: char,
    pub reserved2: char,
}

fn from_base36(input: &str) -> Result<u64, std::num::ParseIntError> {
    u64::from_str_radix(input, 36)
}

fn parse_chunk<K: typenum::Unsigned, T: FromStr>(input: &str) -> IResult<&str, T> {
    map_res(take(K::to_usize()), T::from_str)(input)
}

pub fn parse_auction_summary(input: &str) -> IResult<&str, AuctionSummaryMsg> {
    let Ok((_1, (timestamp, msg_type, symbol, auction_type, price, shares))) = tuple((
        parse_chunk::<U8, u32>,
        char('J'),
        parse_chunk::<U8, String>,
        alt((char('O'), char('C'), char('H'), char('I'))),
        parse_chunk::<U10, u64>,
        parse_chunk::<U10, u32>,
    ))(input) else {
        return Err(nom::Err::Error(Error::new("parse error", ErrorKind::Fail)));
    };

    Ok((
        _1,
        AuctionSummaryMsg {
            timestamp,
            msg_type,
            symbol: symbol.to_string(),
            auction_type,
            price,
            shares,
        },
    ))
}

pub fn parse_add_order(input: &str) -> IResult<&str, AddOrderMsg> {
    let Ok((_1, (timestamp, msg_type, id, side, shares, symbol, price, display, part_id))) =
        tuple((
            parse_chunk::<U8, u32>,
            alt((char('A'), char('d'))),
            map_res(take(12usize), from_base36),
            alt((char('B'), char('S'))),
            parse_chunk::<U6, u32>,
            parse_chunk::<U6, String>,
            parse_chunk::<U10, u64>,
            char('Y'),
            opt(parse_chunk::<U4, String>),
        ))(input)
    else {
        // the below too a long while to figure out. why this craziness??
        return Err(nom::Err::Error(Error::new("parse error", ErrorKind::Fail)));
    };

    let part_id = match part_id {
        Some(p) => p,
        None => String::from(""),
    };

    Ok((
        _1,
        AddOrderMsg {
            timestamp,
            msg_type,
            id,
            side,
            shares,
            symbol,
            price,
            display,
            part_id,
        },
    ))
}

pub fn parse_auction_update(input: &str) -> IResult<&str, AuctionUpdateMsg> {
    let Ok((
        _1,
        (
            timestamp,
            msg_type,
            symbol,
            auction_type,
            reference_price,
            buyshares,
            sellshares,
            indicative_price,
            auction_only_price,
        ),
    )) = tuple((
        parse_chunk::<U8, u32>,
        char('I'),
        parse_chunk::<U8, String>,
        alt((char('O'), char('C'), char('H'), char('I'))),
        parse_chunk::<U10, u64>,
        parse_chunk::<U10, u32>,
        parse_chunk::<U10, u32>,
        parse_chunk::<U10, u64>,
        parse_chunk::<U10, u64>,
    ))(input)
    else {
        // the below too a long while to figure out. why this craziness??
        return Err(nom::Err::Error(Error::new("parse error", ErrorKind::Fail)));
    };

    Ok((
        _1,
        AuctionUpdateMsg {
            timestamp,
            msg_type,
            symbol,
            auction_type,
            reference_price,
            buyshares,
            sellshares,
            indicative_price,
            auction_only_price,
        },
    ))
}

pub fn parse_order_cancel(input: &str) -> IResult<&str, OrderCancelMsg> {
    let Ok((_1, (timestamp, msg_type, id, shares))) = tuple((
        parse_chunk::<U8, u32>,
        char('X'),
        map_res(take(12usize), from_base36),
        parse_chunk::<U6, u32>,
    ))(input) else {
        // the below too a long while to figure out. why this craziness??
        return Err(nom::Err::Error(Error::new("parse error", ErrorKind::Fail)));
    };

    Ok((
        _1,
        OrderCancelMsg {
            timestamp,
            msg_type,
            id,
            shares,
        },
    ))
}

pub fn parse_order_executed(input: &str) -> IResult<&str, OrderExecutedMsg> {
    let Ok((_1, (timestamp, msg_type, id, shares, exec_id))) = tuple((
        parse_chunk::<U8, u32>,
        char('E'),
        map_res(take(12usize), from_base36),
        parse_chunk::<U6, u32>,
        map_res(take(12usize), from_base36),
    ))(input) else {
        // the below too a long while to figure out. why this craziness??
        return Err(nom::Err::Error(Error::new("parse error", ErrorKind::Fail)));
    };

    Ok((
        _1,
        OrderExecutedMsg {
            timestamp,
            msg_type,
            id,
            shares,
            exec_id,
        },
    ))
}

pub fn parse_retail_price_improve(input: &str) -> IResult<&str, RetailPriceImproveMsg> {
    let Ok((_1, (timestamp, msg_type, symbol, retail_price_improve))) = tuple((
        parse_chunk::<U8, u32>,
        char('R'),
        parse_chunk::<U8, String>,
        alt((char('B'), char('A'), char('S'), char('N'))),
    ))(input) else {
        // the below too a long while to figure out. why this craziness??
        return Err(nom::Err::Error(Error::new("parse error", ErrorKind::Fail)));
    };

    Ok((
        _1,
        RetailPriceImproveMsg {
            timestamp,
            msg_type,
            symbol,
            retail_price_improve,
        },
    ))
}

pub fn parse_trade_break(input: &str) -> IResult<&str, TradeBreakMsg> {
    let Ok((_1, (timestamp, msg_type, exec_id))) = tuple((
        parse_chunk::<U8, u32>,
        char('B'),
        map_res(take(12usize), from_base36),
    ))(input) else {
        // the below too a long while to figure out. why this craziness??
        return Err(nom::Err::Error(Error::new("parse error", ErrorKind::Fail)));
    };

    Ok((
        _1,
        TradeBreakMsg {
            timestamp,
            msg_type,
            exec_id,
        },
    ))
}

pub fn parse_trade(input: &str) -> IResult<&str, TradeMsg> {
    let Ok((rest, (timestamp, msg_type))) =
        tuple((parse_chunk::<U8, u32>, alt((char('P'), char('r')))))(input)
    else {
        // the below too a long while to figure out. why this craziness??
        return Err(nom::Err::Error(Error::new("parse error", ErrorKind::Fail)));
    };

    let symbol_len = if msg_type == 'P' { 6usize } else { 8usize };

    let Ok((_1, (id, side, shares, symbol, price, exec_id))) = tuple((
        map_res(take(12usize), from_base36),
        char('B'),
        parse_chunk::<U6, u32>,
        map_res(take(symbol_len), String::from_str),
        parse_chunk::<U10, u64>,
        map_res(take(12usize), from_base36),
    ))(rest) else {
        // the below too a long while to figure out. why this craziness??
        return Err(nom::Err::Error(Error::new("parse error", ErrorKind::Fail)));
    };

    Ok((
        _1,
        TradeMsg {
            timestamp,
            msg_type,
            id,
            side,
            shares,
            symbol,
            price,
            exec_id,
        },
    ))
}

pub fn parse_trading_status(input: &str) -> IResult<&str, TradingStatusMsg> {
    let Ok((_1, (timestamp, msg_type, symbol, halt_status, reg_sho_action, reserved1, reserved2))) =
        tuple((
            parse_chunk::<U8, u32>,
            char('H'),
            parse_chunk::<U8, String>,
            alt((char('H'), char('Q'), char('T'))),
            parse_chunk::<U1, u8>,
            anychar,
            anychar,
        ))(input)
    else {
        // the below too a long while to figure out. why this craziness??
        return Err(nom::Err::Error(Error::new("parse error", ErrorKind::Fail)));
    };

    Ok((
        _1,
        TradingStatusMsg {
            timestamp,
            msg_type,
            symbol,
            halt_status,
            reg_sho_action,
            reserved1,
            reserved2,
        },
    ))
}
