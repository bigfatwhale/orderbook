# BATS PITCH Parser
Simple C++ BATS PITCH message parser built using the Boost Spirit framework. 

Mainly as a proof-of-concept. Not aiming for HFT-grade code - although I know how to do C with inline-assembly if I wanted to. Just using this to learn the spirit framework and hoping to keep the code simple and neat and fast enough :)

The PITCH specification is available here: http://www.batstrading.com/resources/membership/BATS_PITCH_Specification.pdf

We also provide C++ bindings for exposing all the message classes to Python. This is mainly done via the Boost.Python framework. 

The dependencies for building the project are as follows, 

1. Boost.
2. Python dev headers/libraries.
3. CMake.


