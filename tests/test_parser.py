
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
				print i, msg


if __name__ == '__main__':
    unittest.main()