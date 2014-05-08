
The problem
===========

Solve connect 4 exactly, on my Macbook Air in no less than eight hours. It's a win for white. This is well known.

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

    ::python
    def alphaBeta(state, alpha, beta):
        value = minimax(state)
        if value <= alpha:
            return alpha
        elif value >= beta:
            return beta
        else:
            return value

This means the algorithm can /stop early/ if it can prove v is outside those bounds. This turns out to be quite useful as the algorithm is recursive.

MTD-f (it's pointless)
Parallel alpha-beta (it's hard)

Bitboards
---------
We're going to be generating the tree as we iterate over it to keep the memory budget small. This means we are going to be:

1. testing whether a state is terminal, and
2. generating all children of a state (if it's non-terminal)

for every state we explore.

One very efficient way to do (1) is to use a *bitboard* structure to represent the pieces on the board. The idea is that we keep two 64-bit numbers that record, for each position on the board, whether a piece is present in that position for each player. Here's the layout on a Connect Four board:

             0  1  2  3  4  5  6
           |XX|XX|XX|XX|XX|XX|XX|
        5  |05|12|19|26|33|40|47|
        4  |04|11|18|25|32|39|46|
        3  |03|10|17|24|31|38|45|
        2  |02|09|16|23|30|37|44|
        1  |01|08|15|22|29|36|43|
        0  |00|07|14|21|28|35|42|

There are two slightly odd things here. Firstly, the ordering goes up by rows first and then columns, which perhaps seems a little less natural that column first. The second is those `XX` positions, which correspond to indexes in the bitboard that are specially reserved to always contain a zero, called the zero barrier. We'll get into the reasons for both in a little bit.

Here's an example board encoding:

        |.|.|.|.|.|.|.|
        |.|.|.|.|.|.|.|
        |.|.|.|.|.|.|.|
        |.|.|.|.|.|.|.|
        |.|.|.|.|.|.|.|
        |.|.|O|X|.|.|.|
        X = P1 (maximising player)
        O = P2 (minimising player)

        b1 = 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
        b2 = 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000

One advantage of a bitboard is it's very compact so allocation and copying of states is going to be cheap. The other big advatage is that testing whether a state is terminal can be implemented very efficiently with a handful of bitwise operations. To see this first think about a bitboard `b` and the expression `(b << 2) & b`

        |.|.|.|.|.|.|.|     |.|.|X|.|X|.|.|     |.|.|.|.|.|.|.|
        |.|.|.|.|.|.|.|     |.|.|X|X|.|.|.|     |.|.|.|.|.|.|.|
        |.|.|X|.|X|.|.|     |.|.|X|X|X|X|.|     |.|.|X|.|X|.|.|
        |.|.|X|X|.|.|.|     |.|.|X|X|X|X|.|     |.|.|X|X|.|.|.|
        |.|.|X|X|X|X|.|     |.|.|.|.|.|.|.|     |.|.|.|.|.|.|.|
        |.|.|X|X|X|X|.|  &  |.|.|.|.|.|.|.|  =  |.|.|.|.|.|.|.|

As you can see, we have a bit set if there is a piece with another pieces two positions to the left of it. Now let's do the trick again, this time with a shift of one bit:

        |.|.|.|.|.|.|.|     |.|.|.|.|.|.|.|     |.|.|.|.|.|.|.|
        |.|.|.|.|.|.|.|     |.|.|X|.|X|.|.|     |.|.|.|.|.|.|.|
        |.|.|X|.|X|.|.|     |.|.|X|X|.|.|.|     |.|.|X|.|.|.|.|
        |.|.|X|X|.|.|.|     |.|.|.|.|.|.|.|     |.|.|.|.|.|.|.|
        |.|.|X|.|.|.|.|     |.|.|.|.|.|.|.|     |.|.|.|.|.|.|.|
        |.|.|X|X|.|.|.|  &  |.|.|.|.|.|.|.|  =  |.|.|.|.|.|.|.|

The final result will have a 1 in any position where there is a column of four pieces. If we were to shift by the increments of the height of the board instead of one, we can detect rows of four pieces and shifting by `height+1` and `height-1` deals with diagonal lines of four pieces.

This is where we get to the zero barrier, that row of zero bits across the top.

We also need to check for a draw, which is just a bitwise OR of the two bitboards and a comparison against a full bitboard.

Column order to keep track of height for each column, for quicker move()

(Benchmark results)

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
