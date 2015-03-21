![Travis build status](https://travis-ci.org/stompchicken/connect4.svg?branch=master)

Solving Connect Four
--------------------
Computing an optimal strategy for Connect Four.

Usage
-----
Warning: This will take 2Gb of RAM and several hours to complete!
    
    sudo apt-get install ncurses-dev
    make
    bin/connect4 0 2048

Tests
-----

    make DEBUG=1
    bin/unit_test
    bin/prop_test
