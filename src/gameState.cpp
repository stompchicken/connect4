#include "gameState.hpp"

#include <algorithm>
#include <cassert>


GameState::Hasher GameState::hasher;

GameState::GameState() : p1(0), p2(0), xorHash(0), player(PLAYER_MAX), depth(0), emptyPos() {
}

GameState::GameState(uint64 p1_, uint64 p2_, Player player_, Depth depth_) :
    p1(p1_ & Bitboard::zeroBarrier), p2(p2_ & Bitboard::zeroBarrier), player(player_), depth(depth_) {
    this->generateDerivedFields();
}

GameState::GameState(const GameState& other) {
    this->p1 = other.p1;
    this->p2 = other.p2;
    this->player = other.player;
    this->depth = other.depth;
    for(int i=0; i<WIDTH; i++) {
        this->emptyPos[i] = other.emptyPos[i];
    }
    this->xorHash = other.xorHash;
}

GameState& GameState::operator=(const GameState& other) {
    this->p1 = other.p1;
    this->p2 = other.p2;
    this->player = other.player;
    this->depth = other.depth;
    this->xorHash = other.xorHash;
    for(int i=0; i<WIDTH; i++) {
        this->emptyPos[i] = other.emptyPos[i];
    }
    return *this;
}

GameState GameState::random(Depth moves, unsigned width, unsigned height) {
    GameState board;
    do {
        board = GameState();
        for(unsigned i=0; i<moves; i++) {
            GameState nodeBuffer[WIDTH];
            board.children(nodeBuffer, width, height);
            unsigned offset = static_cast<unsigned>(rand());
            for(unsigned j=0; j<width; j++) {
                board = nodeBuffer[(j + offset) % width];
                if(board.isValid()) break;
            }
        }
    } while (board.evaluate() != VALUE_UNKNOWN);
    return board;
}

GameState GameState::parse(const std::string& text) {
    GameState state;
    state.p1 = Bitboard::parse(text, 'X');
    state.p2 = Bitboard::parse(text, 'O');
    state.generateDerivedFields();
    return state;
}


bool GameState::operator==(const GameState& rhs) const {
    return p1 == rhs.p1 &&
        p2 == rhs.p2 &&
        player == rhs.player &&
        depth == rhs.depth &&
        xorHash == rhs.xorHash &&
        std::equal(emptyPos, emptyPos + WIDTH, rhs.emptyPos);
}

bool GameState::operator!=(const GameState& rhs) const {
    return !(*this == rhs);
}

// Pass a pre-assigned buffer of size WIDTH
void GameState::children(GameState* buffer, unsigned width, unsigned height) const {
    for(unsigned col=0; col<WIDTH; col++) {

        GameState* child = buffer+col;
        unsigned row = emptyPos[col];
        if(row < height && col < width) {
            child->p1 = p1;
            child->p2 = p2;
            child->depth = depth;
            child->player = player;
            for(unsigned i=0; i<width; i++) {
                child->emptyPos[i] = this->emptyPos[i];
            }
            child->makeMove(row, col);
            unsigned index = Bitboard::toIndex(row, col);
            child->xorHash = hasher.incHash(xorHash, index, player);
        } else {
            child->player = PLAYER_INVALID;
        }
    }
}

Value GameState::evaluate() const {
    if((p1|p2) == Bitboard::zeroBarrier) {
        return VALUE_DRAW;
    }

    if(Bitboard::line4(p1) > 0) {
        return VALUE_MAX;
    } else if(Bitboard::line4(p2) > 0) {
        return VALUE_MIN;
    }

    return VALUE_UNKNOWN;
}

Value GameState::heuristic() const {
    Value val = evaluate();
    if(val == VALUE_MAX) {
        return 255;
    } else if(val == VALUE_MIN) {
        return 0;
    } else if(val == VALUE_DRAW) {
        return 128;
    } else {
        uint8 p13 = Bitboard::popcount(Bitboard::line3(p1 & ~p2));
        uint8 p23 = Bitboard::popcount(Bitboard::line3(p2 & ~p1));
        return (p13 - p23) + 128;
    }
}

GameState GameState::flipLeftRight() const {
    GameState flip;
    flip.p1 = Bitboard::flip(p1);
    flip.p2 = Bitboard::flip(p2);
    flip.generateDerivedFields();
    return flip;
}

std::string GameState::print() const {
    std::string text;
    uint64 mask;
    for(unsigned row=0; row<HEIGHT; row++) {
        for(unsigned col=0; col<WIDTH; col++) {
            mask = Bitboard::toMask((HEIGHT-1)-row, col);

            if(p1 & mask) {
                text.append("X|");
            } else if(p2 & mask) {
                text.append("O|");
            } else {
                text.append(".|");
            }
        }
        if(row != HEIGHT - 1) {
            text.append("\n");
        }
    }
    return text;
}

std::ostream& operator<<(std::ostream& os, const GameState& board) {
    os << "GameState: {";
    os << "p1=" << board.p1 << " ";
    os << "p2=" << board.p2 << " ";
    os << "player=" << (int)board.player << " ";
    os << "depth=" << (int)board.depth << " ";
    os << "xorHash=" << board.xorHash << " ";
    os << "emptyPos=";
    for(unsigned i=0; i<WIDTH; i++) {
        os << static_cast<int>(board.emptyPos[i]) << (i==WIDTH-1 ? "" : " ");
    }
    os << "}";
    return os;
}

void GameState::generateDerivedFields() {
    uint64 board = this->p1 | this->p2;
    this->depth = Bitboard::popcount(board);

    // Infer player from number of pieces on the board
    if(Bitboard::popcount(this->p1) > Bitboard::popcount(this->p2)) {
        this->player = PLAYER_MIN;
    } else {
        this->player = PLAYER_MAX;
    }

    // Generate hash
    this->xorHash = hasher.hash(this->p1, this->p2, this->player);

    // Generate empty positions
    uint64 mask;
    for(unsigned col=0; col<WIDTH; col++) {
        this->emptyPos[col] = HEIGHT;
        for(unsigned row=0; row<HEIGHT; row++) {
            mask = Bitboard::toMask(row, col);
            if((board & mask) == 0) {
                this->emptyPos[col] = static_cast<uint8>(row);
                break;
            }
        }
    }
}

void GameState::assertInvariants() const {
    assert((p1 & p2) == 0);
    assert(player == PLAYER_MAX || player == PLAYER_MIN);
    assert(((p1 & ~Bitboard::zeroBarrier) == 0) && ((p2 & ~Bitboard::zeroBarrier) == 0));
    assert(xorHash == hasher.hash(p1, p2, player));
}

inline void GameState::makeMove(unsigned row, unsigned col) {
    if(player == PLAYER_MAX) {
        this->p1 |= Bitboard::toMask(row, col);
        this->player = PLAYER_MIN;
    } else {
        this->p2 |= Bitboard::toMask(row, col);
        this->player = PLAYER_MAX;
    }
    this->depth += 1;
    this->emptyPos[col] += 1;

}

uint64 flipLeftRight(uint64 x) {
    uint64 output = 0;
    for(unsigned row=0; row<HEIGHT; row++) {
        for(unsigned col=0; col<WIDTH; col++) {
            if (x & Bitboard::toMask(row, col)) {
                output |= Bitboard::toMask(row, WIDTH-1-col);
            }
        }
    }
    return output;
}

