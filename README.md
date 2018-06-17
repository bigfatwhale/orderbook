# Fast orderbook 
Implementation (in C++/Rust/Python) of a orderbook data structure for LOB research / trading simulation capabilities. The Rust version is a mirror of the C++ implementation mainly for my own learning of how to do stuff in the language. The Python/C++ bindings allow the C++ implementation to be used in Python and facilitates doing stuff in Jupyter. 

The main C++ implementation features, 

 - Use of C++17 constructs, Boost C++ libraries and template meta-programming constructs like SFINAE etc. 
 - Message parser for [BATS PITCH](http://www.batstrading.com/resources/membership/BATS_PITCH_Specification.pdf) message format. This particular feed is a fixed length message of non\-control ASCII bytes. While binary messages can be cast directly into structs (being mindful of struct packing), we have to parse the ascii message. For this we experimented with Boost Spirit, which is quite nice and easy to use.
 - Message parser for [LOBSTER](https://lobsterdata.com/info/DataStructure.php) format. 

 - We prototype the use a van emde boas (veb) data structure for tracking price levels (across additions/deletions/executions) in the book and for navigating from one price level to the next one (while the orderbook evolves). This is partly possible because price levels are represented in 6.4 fixed point.

    Further more, tick size rules on various exchange such as [BATS](http://cdn.batstrading.com/resources/participant_resources/BATSEuro_FESE_Ticks.pdf) further restricts the universe of possible price levels. It might be possible to further improve the speed and memory use of the veb data structure exploiting this fact.

 - Template-based configuration of different order price-bucket managers. This allows us to swap in/out between the van emde boas data structure vs. the stock implementation.
 - Elimination of conditional branching through the use of SFINAE. 



