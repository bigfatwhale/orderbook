#[macro_use]
extern crate nom;

use std::str::FromStr;

#[cfg(test)]
mod tests {
	use super::parse_auction_summary;
	use std::fs::File;
	use std::io::BufRead;
	use std::io::BufReader;

    #[test]
    fn test_parse_auction_summary() {
		let msg = "28800168JAAPLSPOTC00010068000000020000";
		let res = parse_auction_summary(msg);
		println!("{:?}", res);
		assert!(res.is_ok());
    }

    #[test]
    fn test_parse_file() {
    	let filename = "/Users/unclechu/code/rust_projects/bats_api/src/pitch_example_data";
    	let f = File::open(filename).expect("file not found");
    	let f = BufReader::new(f);

    	for line in f.lines() {
    		let msg = line.unwrap();
    		let buf = msg.as_str();
    		if buf.chars().nth(8) == Some('J') {
    			let res = parse_auction_summary(buf);
    			assert!(res.is_ok());
    			break;
    		}
    	}
    }
}

#[derive(Debug)]
struct AuctionSummary {
	timestamp    : u32, 
	msg_type     : char,
	symbol       : String, 
	auction_type : char, 
	price        : u64, 
	shares       : u32
}

named!(parse_auction_summary<&str, AuctionSummary>,  
	do_parse!(
		ts : map_res!(take!(8),  FromStr::from_str) >>
		m  : char!('J')                             >>
		s  : map_res!(take!(8),  FromStr::from_str) >>
		a  : map_res!(take!(1),  FromStr::from_str) >>
		p  : map_res!(take!(10), FromStr::from_str) >>
		v  : map_res!(take!(10), FromStr::from_str) >>
		(AuctionSummary{ timestamp: ts, 
						 msg_type : m, 
						 symbol   : s, 
						 auction_type : a, 
					     price : p, 
					 	 shares : v 
					   } )  
	)
);
