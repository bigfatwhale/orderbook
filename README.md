# Fast orderbook 
Implementation of a orderbook data structure for LOB research / trading simulation capabilities. 

We attempt to prototype the use a van emde boas data structure for tracking price levels in the book and navigating from one non-empty price level to the next one. This is partly possible because price levels are represented in 6.4 fixed point. 

Further more, tick size rules on various exchange such as [BATS](http://cdn.batstrading.com/resources/participant_resources/BATSEuro_FESE_Ticks.pdf) further restricts the universe of possible price levels. 

All this means we can possible navigate the order book really fast!
