
The problem
===========

Solve connect 4 exactly, on my Macbook Air in no less than eight hours. It's a win for white. This is well known.

Algorithm
=========

Minimax
-------
http://en.wikipedia.org/wiki/Minimax

    :::python
    def minimax(state):
        value = state.evaluate()
        if value != None:
            return value
        else:
            value = VALUE_MAX if state.player == PLAYER_MIN else VALUE_MIN
            for child in state.children:
                child_value = minimax(child)
                if state.player == PLAYER_MAX:
                    value = max(child_value, value)
                else:
                    value = min(child_value, value)
            return value

Cost linear in the size of the game tree, but we can do better.

Diagram OXO

I use this to test alpha beta on the principle that it would be hard to screw it up.

Alpha beta pruning
------------------

Minimax with upper/lower bounds.

Algorithm's response is:
    v <= alpha: alpha
    alpha < v < beta: v
    v >= beta: beta

This means the algorithm can /stop early/ if it can prove v is outside those bounds.

MTD-f (it's pointless)
Parallel alpha-beta (it's hard)

Implementation
==============

Bitboards
---------
You do it for every node explored, so it better be fast
A 64 bit number can hold player piece positions
Draw testing with 8 bitshifts, 8 ANDS, etc.
Zero barrier to stop wrap arounds
Column order to keep track of height for each column, for quicker move()
(Benchmark results)

        Bitboard structure, column major with zero barrier at max height

        X = P1 (maximising player)
        O = P2 (minimising player)

             0  1  2  3  4  5  6
           |XX|XX|XX|XX|XX|XX|XX|
        5  |05|12|19|26|33|40|47|
        4  |04|11|18|25|32|39|46|
        3  |03|10|17|24|31|38|45|
        2  |02|09|16|23|30|37|44|
        1  |01|08|15|22|29|36|43|
        0  |00|07|14|21|28|35|42|


Caching states
--------------
The same state can be arrived at through differnet sequence of moves
e.g. OXO
So we cache values to reduce redundant work, can result in 100-1000x perf. improvement

Closed address, linear probing hash table
Packed 64-bit key/value
key(49b)|lower|upper|move|depth
No resizing
Zobrist incremental hashing
Clever key trick
Depth based replacement
Move stored to compute principle variation

Pruning
-------
Principles of pruning: if you can find the best move first you save work
e.g. central column in c4 is a player 1 win, once you know that, why bother anywhere else?
Early pruning of children by calls to evaluate()
Static move ordering, prefer centre moves
Killer heuristic
History heuristic
LR symmetry
Iterative deepening
(Perf. improvements)

Performance
===================
Fundamental problem is the memory access in cache get (40% run time)
Pre-fetching
Pooling (helped a bit)
Sorting networks (surprisingly fast)
Lots of evaluation on 6x5
ncurses based output
-O3 -ftree-vectorise
Relationship to cache size
Profiling: Callgrind is the greatest, instruments is okay
