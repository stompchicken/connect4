![Travis build status](https://travis-ci.org/stompchicken/connect4.svg?branch=master)

Solving Connect Four
--------------------
This program computes the minimax value of the game Connect Four (it's a win for the first player). The algorithm is simple alpha-beta but there are a number of implementation tricks needed to get it to work quickly enough. The main ones are:
  * Bitboards to represent the game state and test for winning states
  * Caching of state values
  * Child reordering and early pruning

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
