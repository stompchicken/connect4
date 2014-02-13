#include "gtest/gtest.h"

#include "game.hpp"
#include "connect4.hpp"

using namespace testing;

class NodeOrderingTest : public Test { };

TEST_F(NodeOrderingTest, Sorting) {
    srand(8088);

    for(int n=0; n < 100; n++) {
        Connect4 nodes[WIDTH];
        for(uint i=0; i<WIDTH; i++) {
            nodes[i] = Connect4::random();
        }

        NodeOrdering orderingMax(nodes, PLAYER_MAX);
        NodeOrdering orderingMin(nodes, PLAYER_MIN);

        for(uint i=1; i<WIDTH; i++) {
            const Connect4& n1 = nodes[orderingMax.move(i-1)];
            const Connect4& n2 = nodes[orderingMax.move(i)];
            ASSERT_TRUE(n1.heuristic() >= n2.heuristic());
        }

        for(uint i=1; i<WIDTH; i++) {
            const Connect4& n1 = nodes[orderingMin.move(i-1)];
            const Connect4& n2 = nodes[orderingMin.move(i)];
            ASSERT_TRUE(n1.heuristic() <= n2.heuristic());
        }
    }
}
