
from orderbook_api import *
import unittest


class ParserTest( unittest.TestCase ):

    def test_simple(self):

		lob = LimitOrderBook()

		o = Order(1000, 3222, 4444, BookType.BUY, 3001)

		lob.addOrder(o)

		bucket_iter  = iter(lob.bids)
		price_bucket = bucket_iter.next()

		self.assertEqual( price_bucket.pricePoint, 3222)

		orderIter = iter(price_bucket)
		order = orderIter.next()
		self.assertEqual( order.orderId, 1000)

if __name__=='__main__':
    unittest.main()