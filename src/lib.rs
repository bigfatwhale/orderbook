
#[macro_use]
extern crate nom;
use nom::IResult;

use std::str::FromStr;
use std::result::Result;

#[cfg(test)]
mod tests {
    //use super::parse_auction_summary;
    use super::AuctionSummary;
    use super::AddOrderMsg;
    use std::env;
    use std::fs::File;
    use std::io::BufRead;
    use std::io::BufReader;

    #[test]
    fn test_parse_auction_summary() {
        let msg = "28800168JAAPLSPOTC00010068000000020000";
        let res = AuctionSummary::parse_msg(msg);
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

        let res = AddOrderMsg::parse_msg(msg_long);
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
                let res = AuctionSummary::parse_msg(buf);
                println!("{:?}", res);
                assert!(res.is_ok());
                break;
            }
        }
    }
}

#[derive(Debug)]
pub struct AuctionSummary {
    timestamp    : u32, 
    msg_type     : char,
    symbol       : String, 
    auction_type : char, 
    price        : u64, 
    shares       : u32
}

#[derive(Debug)]
pub struct AddOrderMsg {
    timestamp    : u32, 
    msg_type     : char,
    order_id     : u64, 
    side         : char, 
    shares       : u32, 
    symbol       : String,  
    price        : u64, 
    display      : char,
    part_id      : String  
}

impl AuctionSummary {

    pub fn parse_msg( msg : &str ) -> Result<AuctionSummary, nom::Err<&str>> {

        let o = parse_auction_summary(msg);
        if o.is_ok() {
            Ok(o.unwrap().1)
        } else {
            Err(o.unwrap_err())
        }
    }
}

impl AddOrderMsg {

    pub fn parse_msg( msg : &str ) -> Result<AddOrderMsg, nom::Err<&str>> {

        let o = parse_add_order(msg);
        if o.is_ok() {
            Ok(o.unwrap().1)
        } else {
            Err(o.unwrap_err())
        }
    }
}

fn from_hex(input: &str) -> Result<u64, std::num::ParseIntError> {
    u64::from_str_radix(input, 36)
}

named!(parse_auction_summary<&str, AuctionSummary>,  
    do_parse!(
        ts : map_res!(take!(8),  FromStr::from_str) >>
        m  : char!('J')                             >>
        s  : map_res!(take!(8),  FromStr::from_str) >>
        a  : map_res!(take!(1),  FromStr::from_str) >>
        p  : map_res!(take!(10), FromStr::from_str) >>
        v  : map_res!(take!(10), FromStr::from_str) >>
        (AuctionSummary{ timestamp    : ts, 
                         msg_type     : m, 
                         symbol       : s, 
                         auction_type : a, 
                         price        : p, 
                         shares       : v 
                       } )  
    )
);

fn parse_opt_part_id( input : &str ) -> IResult<&str, String>
{
    if input.is_empty() {
        Ok(("", String::from("")))
    } else {
        let (first, last) = input.split_at(4);
        Ok((last, String::from(first)))
    }
}

named!(parse_add_order<&str, AddOrderMsg>,  
    do_parse!(
        ts : map_res!(take!(8),  FromStr::from_str) >>
        m  : alt!(char!('A') | char!('d'))          >>
        o  : map_res!(take!(12), from_hex)          >>
        s  : map_res!(take!(1),  FromStr::from_str) >>
        v  : map_res!(take!(6),  FromStr::from_str) >>
        t  : map_res!(take!(6),  FromStr::from_str) >>
        p  : map_res!(take!(10), FromStr::from_str) >>
        d  : map_res!(take!(1),  FromStr::from_str) >>
        pi : parse_opt_part_id                      >>
        (AddOrderMsg{ timestamp : ts, 
                      msg_type  : m, 
                      order_id  : o, 
                      side      : s,
                      shares    : v, 
                      symbol    : t, 
                      price     : p, 
                      display   : d,
                      part_id   : pi
                    })  
    )
);
