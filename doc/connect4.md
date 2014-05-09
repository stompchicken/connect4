
The problem
===========

Solve connect 4 exactly, on my Macbook Air in no less than eight hours. It's a win for white. This is well known.

Minimax
-------
http://en.wikipedia.org/wiki/Minimax

    :::python
    def minimax(state):
        if state.is_terminal():
            return state.evaluate()
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

Alpha beta
----------

Minimax with upper/lower bounds.

    alphaBeta(state, alpha, beta) = { ...

Alpha-beta has fewer responsibilities than minimax. It no longer needs
to return the value of a state, it just needs to return *bounds* on
the value of the state and that allows it to do the job faster. If it
can prove that the state falls outside the [alpha, beta] bound it can
stop early, or *cutoff* the search at that point.

An implementation of alpha-beta that takes advantage of this:

    ::python
    def alpha_beta(state, alpha, beta):
        if state.is_terminal():
            return state.evaluate()
        else:
            value = VALUE_MAX if state.player == PLAYER_MIN else VALUE_MIN
            for child in state.children:
                child_value = alpha_beta(child, alpha, beta)
                if state.player == PLAYER_MAX:
                    value = max(child_value, value)
                else:
                    value = min(child_value, value)

            return value

If you look at things written about chess programming, the most
promising improvements to alpha-beta are either parallelisation of
regular alpha-beta or an algorithm called
[MTD(f)](http://en.wikipedia.org/wiki/MTD-f) which is supposed to be
more efficient.

Parallelisation of alpha-beta is not a particularly straightforward
thing. The introduction of a pruning step after exploring each child
state makes the algorithm explicitly serial, **that's why it's fast in
the first place**. In order to be able to parallelise alpha-beta you
have to be willing to give up on (at least some of) that pruning.

For me, MTD(f) was not an algorithm that made a lot of sense to me,
even after reading several detailed explanations. However, as
sometimes happens, the original paper on the algorithm gives by far
the best explanation of MTD(f). My brief summary is this: alpha-beta
is fastest when the space between alpha and beta is small (called the
search window). A small search window means more cutoffs and a smaller
time to complete but is also likely to be unhelpful if the true value
doesn't lie between alpha and beta.

Bitboards
---------
We're going to be generating the tree as we iterate over it to keep
the memory budget small. This means we are going to be:

1. testing whether a state is terminal, and
2. generating all children of a state (if it's non-terminal)

for every state we explore.

One very efficient way to do (1) is to use a *bitboard* structure to
represent the pieces on the board. The idea is that we keep two 64-bit
numbers that record, for each position on the board, whether a piece
is present in that position for each player. Here's the layout on a
Connect Four board:

             0  1  2  3  4  5  6
           |XX|XX|XX|XX|XX|XX|XX|
        5  |05|12|19|26|33|40|47|
        4  |04|11|18|25|32|39|46|
        3  |03|10|17|24|31|38|45|
        2  |02|09|16|23|30|37|44|
        1  |01|08|15|22|29|36|43|
        0  |00|07|14|21|28|35|42|

There are two slightly odd things here. Firstly, the ordering goes up
by rows first and then columns, which perhaps seems a little less
natural that column first. The second is those `XX` positions, which
correspond to indexes in the bitboard that are specially reserved to
always contain a zero, called the zero barrier. We'll get into the
reasons for both in a little bit.

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

As you can see, we have a bit set if there is a piece with another
pieces two positions to the left of it. Now let's do the trick again,
this time with a shift of one bit:

        |.|.|.|.|.|.|.|     |.|.|.|.|.|.|.|     |.|.|.|.|.|.|.|
        |.|.|.|.|.|.|.|     |.|.|X|.|X|.|.|     |.|.|.|.|.|.|.|
        |.|.|X|.|X|.|.|     |.|.|X|X|.|.|.|     |.|.|X|.|.|.|.|
        |.|.|X|X|.|.|.|     |.|.|.|.|.|.|.|     |.|.|.|.|.|.|.|
        |.|.|X|.|.|.|.|     |.|.|.|.|.|.|.|     |.|.|.|.|.|.|.|
        |.|.|X|X|.|.|.|  &  |.|.|.|.|.|.|.|  =  |.|.|.|.|.|.|.|

The final result will have a 1 in any position where there is a column
of four pieces. If we were to shift by the increments of the height of
the board instead of one, we can detect rows of four pieces and
shifting by `height+1` and `height-1` deals with diagonal lines of
four pieces.

This is where we get to the zero barrier, that row of zero bits across
the top.

We also need to check for a draw, which is just a bitwise OR of the
two bitboards and a comparison against a full bitboard.

Column order to keep track of height for each column, for quicker
move()

(Benchmark results)

Caching states
--------------

The same state can be arrived at through different sequence of moves

    |.|.|.|.|.|.|.|
    |.|.|.|.|.|.|.|
    |.|.|.|.|.|.|.|
    |.|.|.|.|.|.|.|
    |.|.|O|.|X|.|.|
    |.|.|X|.|O|.|.|

In this state, the first move must be the `X` in column 2, the second
move, however, could be either in column 2 or column 4 so this state
can be reached by two different paths in the game tree. We will be
doing a considerable amount of redundant work in working on states
that can be reached multiple ways in the game tree - at lower depths
the number of paths to reach the same state will be very large.

The approach we take here is to use a large cache structure that
stores states and their values. This is a common strategy in chess
programming, where they call it a *transposition table*, but I'm just
going to call it a cache.

The cache is implemented as a very simple hash table, using a fixed
size, closed addressing and linear probing to resolve hash collisions.
The nice thing about using a hash table as a cache is that we don't
have to care too much about occasional false negatives so we used a
fixed size probe and just give up after that if we don't find what we
want. This maintains a constant cost for lookup even when the hash
table gets full (although the hit rate goes down a lot).

* Hash function (Incremental Zobrist)
* Key/value structure
* Key trick
* Depth based replacement
* Performance evaluation
* CPU utilisation / cache misses

Pruning
-------

The last major thing we can do to improve performance is to make the
pruning of states more effective.

For example, starting from a empty Connect Four board, let's assume player 1 puts their first piece in the
central column. Solving from this state  you see that player 1 can
force a win from this state and so your work is done because you can't
get better than winning.

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
