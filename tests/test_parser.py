
import bats_api
import unittest

class ParserTest( unittest.TestCase ):

    def setUp(self):
        self.parser = bats_api.BATSPitchMsgParser()

    def test_bulkLoad(self):
        ''' test loading 20k msgs from pitch_example_data. test will pass as long as
        no exceptions are thrown.
        '''
        with open( 'pitch_example_data' ) as f:
            for i, line in enumerate(f):
                line = line.strip()
                msg = self.parser.parse_msg(line)
                if msg is None: 
                    raise Exception('parse failure for line : %s' % line)

    def test_parse_trade_break(self):

        msg = self.parser.parse_msg( "S28800168B1K27GA00000Y" )
        #int('1K27GA00000Y', 36) -> 204969015920664610 
        self.assertEqual( msg.timestamp, 28800168 )
        self.assertEqual( msg.msgtype, 'B')
        self.assertEqual( msg.execId, 204969015920664610 )

    def test_parse_trade_status(self):

        msg = self.parser.parse_msg( "S28800168HAAPLSPOTT0XY")
        self.assertEqual( msg.timestamp, 28800168 )
        self.assertEqual( msg.msgtype, 'H')
        self.assertEqual( msg.symbol, "AAPLSPOT") # ticker symbol
        self.assertEqual( msg.halt_status, 'T')   # halted status
        self.assertEqual( msg.reserved1, 'X')
        self.assertEqual( msg.reserved2, 'Y')

    def test_parse_retail_price_improve(self): 

        msg = self.parser.parse_msg( "S28800168RAAPLSPOTS" )

        self.assertEqual( msg.timestamp, 28800168 )
        self.assertEqual( msg.msgtype, 'R')
        self.assertEqual( msg.symbol, "AAPLSPOT") # ticker symbol
        self.assertEqual( msg.retail_price_improve, 'S') # S : sell-side RPI
        
    def test_parse_order_cancel(self): 

        msg = self.parser.parse_msg("S28800168X1K27GA00000Y000500")

        self.assertEqual( msg.timestamp, 28800168 )
        self.assertEqual( msg.msgtype, 'X')
        self.assertEqual( msg.orderId, 204969015920664610) 
        self.assertEqual( msg.shares, 500) 

    def test_parse_order_execute(self): 

        msg = self.parser.parse_msg("S28800168E1K27GA00000Y0001001K27GA00000K")

        self.assertEqual( msg.timestamp, 28800168 )
        self.assertEqual( msg.msgtype, 'E')
        self.assertEqual( msg.orderId, 204969015920664610) # ticker symbol
        self.assertEqual( msg.shares, 100) 
        self.assertEqual( msg.execId, 204969015920664596) # S : sell-side RPI

    def test_parse_auction_summary(self): 

        msg = self.parser.parse_msg("S28800168JAAPLSPOTC00010068000000020000")

        self.assertEqual( msg.timestamp, 28800168 )
        self.assertEqual( msg.msgtype, 'J')
        self.assertEqual( msg.symbol, "AAPLSPOT")
        self.assertEqual( msg.auction_type, 'C')
        self.assertEqual( msg.price, 1006800)
        self.assertEqual( msg.shares, 20000)

    def test_parse_auction_update(self): 

        msg = self.parser.parse_msg("S28800168IAAPLSPOTC00010068000000020000000001000000015034000001309800")

        self.assertEqual( msg.timestamp, 28800168 )
        self.assertEqual( msg.msgtype, 'I')
        self.assertEqual( msg.symbol, "AAPLSPOT")
        self.assertEqual( msg.auction_type, 'C')
        self.assertEqual( msg.reference_price, 1006800)
        self.assertEqual( msg.buyshares, 20000)
        self.assertEqual( msg.sellshares, 10000)
        self.assertEqual( msg.indicative_price, 1503400)
        self.assertEqual( msg.auction_only_price, 1309800)

    def test_parse_add_order(self): 

        msg = self.parser.parse_msg("S28800168A1K27GA00000YS000100AAPL  0001831900Y")

        self.assertEqual( msg.timestamp, 28800168 )
        self.assertEqual( msg.msgtype, 'A')
        self.assertEqual( msg.orderId, 204969015920664610)
        self.assertEqual( msg.side, 'S')
        self.assertEqual( msg.shares, 100)
        self.assertEqual( msg.symbol, "AAPL  ")
        self.assertEqual( msg.price, 1831900)
        self.assertEqual( msg.display, 'Y')

        msg = self.parser.parse_msg("S28800168d1K27GA00000YS000100AAPL  0001831900YBAML")

        self.assertEqual( msg.timestamp, 28800168 )
        self.assertEqual( msg.msgtype, 'd')
        self.assertEqual( msg.orderId, 204969015920664610)
        self.assertEqual( msg.side, 'S')
        self.assertEqual( msg.shares, 100)
        self.assertEqual( msg.symbol, "AAPL  ")
        self.assertEqual( msg.price, 1831900)
        self.assertEqual( msg.display, 'Y')
        self.assertEqual( msg.partId, "BAML")

    def test_parse_trade(self): 

        msg = self.parser.parse_msg("S28800168P1K27GA00000YB000300AAPL  00018319001K27GA00000Z");

        self.assertEqual( msg.timestamp, 28800168 );
        self.assertEqual( msg.msgtype, 'P');
        self.assertEqual( msg.orderId, 204969015920664610);
        self.assertEqual( msg.side, 'B');
        self.assertEqual( msg.shares, 300);
        self.assertEqual( msg.symbol, "AAPL  ");
        self.assertEqual( msg.price, 1831900);
        self.assertEqual( msg.execId, 204969015920664611); # python : int("1K27GA00000Z", 36) -> 204969015920664611

        msg = self.parser.parse_msg("S28800168r1K27GA00000YB000300AAPLSPOT00018319001K27GA00000Z");

        self.assertEqual( msg.timestamp, 28800168 );
        self.assertEqual( msg.msgtype, 'r');
        self.assertEqual( msg.orderId, 204969015920664610);
        self.assertEqual( msg.side, 'B');
        self.assertEqual( msg.shares, 300);
        self.assertEqual( msg.symbol, "AAPLSPOT");
        self.assertEqual( msg.price, 1831900);
        self.assertEqual( msg.execId, 204969015920664611 ); 









if __name__=='__main__':
    unittest.main()







