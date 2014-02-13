#include "gameState.hpp"

#include <algorithm>
#include <cassert>

uint64 Bitboard::parse(std::string text, char piece) {
    uint64 mask;
    unsigned col = 0;
    unsigned row = HEIGHT - 1;
    uint64 board = 0;
    for(unsigned i = 0; i < text.size(); i++) {
        mask = toMask(row, col);
        if(text[i] == '\n') {
            row -= 1;
            col = 0;
        } else if(text[i] == '|') {
            col += 1;
        } else if(text[i] == piece) {
            board |= mask;
        }
    }
    return board;
}

std::string Bitboard::print(uint64 b) {
    std::string text;
    uint64 mask;
    for(unsigned row=0; row<HEIGHT; row++) {
        for(unsigned col=0; col<WIDTH; col++) {
            mask = Bitboard::toMask((HEIGHT-1)-row, col);

            if(b & mask) {
                text.append("X|");
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


uint64 Bitboard::line4(uint64 b) {
    uint64 x = 0;

    x = (b << 2) & b;
    uint64 vertical = x & (x << 1);

    x = (b << (2*(HEIGHT+1))) & b;
    uint64 horizontal = x & (x << (HEIGHT+1));

    x = (b << (2*(HEIGHT+2))) & b;
    uint64 diagRight = x & (x << (HEIGHT+2));

    x = (b << (2*(HEIGHT))) & b;
    uint64 diagLeft = x & (x << (HEIGHT));

    return (vertical | horizontal | diagLeft | diagRight) & Bitboard::zeroBarrier;
}

uint64 Bitboard::line3(uint64 b) {
    uint64 x = 0;
    uint s = 0;
    uint64 l1, l2, l3;
    uint64 r1, r2, r3;

    // Vertical
    s = 1;
    l1 = b << 1;
    l2 = b << 2;
    l3 = b << 3;
    r1 = b >> 1;
    r2 = b >> 2;
    r3 = b >> 3;

    x |= (~b & l1 & l2 & l3) | (~b & l1 & l2 & r1) |
         (~b & r1 & r2 & l1) | (~b & r1 & r2 & r3);

    // Horizontal
    s = HEIGHT+1;
    l1 = b << 1*s;
    l2 = b << 2*s;
    l3 = b << 3*s;
    r1 = b >> 1*s;
    r2 = b >> 2*s;
    r3 = b >> 3*s;

    x |= (~b & l1 & l2 & l3) | (~b & l1 & l2 & r1) |
         (~b & r1 & r2 & l1) | (~b & r1 & r2 & r3);

    // Left diagonal
    s = HEIGHT;
    l1 = b << 1*s;
    l2 = b << 2*s;
    l3 = b << 3*s;
    r1 = b >> 1*s;
    r2 = b >> 2*s;
    r3 = b >> 3*s;

    x |= (~b & l1 & l2 & l3) | (~b & l1 & l2 & r1) |
         (~b & r1 & r2 & l1) | (~b & r1 & r2 & r3);

    // Right diagonal
    s = HEIGHT+2;
    l1 = b << 1*s;
    l2 = b << 2*s;
    l3 = b << 3*s;
    r1 = b >> 1*s;
    r2 = b >> 2*s;
    r3 = b >> 3*s;

    x |= (~b & l1 & l2 & l3) | (~b & l1 & l2 & r1) |
         (~b & r1 & r2 & l1) | (~b & r1 & r2 & r3);

    return x & Bitboard::zeroBarrier;
}

uint64 Bitboard::line2(uint64 b) {
    uint64 x = 0;
    uint s = 1;
    x |= ~b & (b << (1*s)) & (b << (2*s)) & ~(b << (3*s));
    s = HEIGHT;
    x |= ~b & (b << (1*s)) & (b << (2*s)) & ~(b << (3*s));
    s = HEIGHT+1;
    x |= ~b & (b << (1*s)) & (b << (2*s)) & ~(b << (3*s));
    s = HEIGHT+2;
    x |= ~b & (b << (1*s)) & (b << (2*s)) & ~(b << (3*s));
    return x;
}

uint64 makeZeroBarrier() {
    uint64 z = 0;
    for(unsigned row=0; row<HEIGHT; row++) {
        for(unsigned col=0; col<WIDTH; col++) {
            z |= Bitboard::toMask(row, col);
        }
    }
    return z;
}

uint64 Bitboard::zeroBarrier = makeZeroBarrier();

GameState::Hasher GameState::hasher;

GameState::GameState() : p1(0), p2(0), player(PLAYER_MAX), depth(0) {
    std::fill(emptyPos, emptyPos+WIDTH, 0);
    xorHash = 0;

}

GameState::GameState(uint64 p1_, uint64 p2_, int player_, int depth_) :
    p1(p1_ & Bitboard::zeroBarrier), p2(p2_ & Bitboard::zeroBarrier), player(player_), depth(depth_) {
    this->generateDerivedFields();
}

GameState::GameState(const GameState& other) {
    this->p1 = other.p1;
    this->p2 = other.p2;
    this->player = other.player;
    this->depth = other.depth;
    this->xorHash = other.xorHash;
    std::copy(other.emptyPos, other.emptyPos+WIDTH, this->emptyPos);
}

GameState& GameState::operator=(const GameState& other) {
    this->p1 = other.p1;
    this->p2 = other.p2;
    this->player = other.player;
    this->depth = other.depth;
    this->xorHash = other.xorHash;
    std::copy(other.emptyPos, other.emptyPos+WIDTH, this->emptyPos);
    return *this;
}

GameState GameState::random(Depth moves) {
    GameState board;
    do {
        board = GameState();
        for(unsigned i=0; i<moves; i++) {
            GameState nodeBuffer[WIDTH];
            board.children(nodeBuffer);
            int offset = rand();
            for(unsigned j=0; j<WIDTH; j++) {
                board = nodeBuffer[(j + offset) % WIDTH];
                if(board.isValid()) break;
            }
        }
    } while (board.evaluate() != VALUE_UNKNOWN);
    return board;
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
void GameState::children(GameState* buffer) const {
    for(unsigned col=0; col<WIDTH; col++) {
        GameState* child = buffer+col;
        unsigned row = emptyPos[col];
        if(row < HEIGHT) {
            child->p1 = p1;
            child->p2 = p2;
            child->depth = depth;
            child->player = player;
            std::copy(emptyPos, emptyPos+WIDTH, child->emptyPos);
            child->makeMove(row, col);
            unsigned index = Bitboard::toIndex(row, col);
            child->xorHash = hasher.incHash(xorHash, index, player);
        } else {
            child->player = PLAYER_INVALID;
        }
    }

}

Value GameState::evaluate() const {
    if(Bitboard::line4(p1) > 0) {
        return VALUE_MAX;
    } else if(Bitboard::line4(p2) > 0) {
        return VALUE_MIN;
    }

    if((p1|p2) == Bitboard::zeroBarrier) {
        return VALUE_DRAW;
    }

    return VALUE_UNKNOWN;
}

Value GameState::heuristic() const {
    Value val = evaluate();
    if(val != VALUE_UNKNOWN) {
        return val;
    } else {
        int p13 = Bitboard::popcount(Bitboard::line3(p1 & ~p2));
        int p23 = Bitboard::popcount(Bitboard::line3(p2 & ~p1));
        return (p13 - p23) + 128;
    }
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

void GameState::parse(std::string text, int player_, int depth_) {
    p1 = Bitboard::parse(text, 'X');
    p2 = Bitboard::parse(text, 'O');

    this->player = player_;
    this->depth = depth_;
    this->generateDerivedFields();
}

std::ostream& operator<<(std::ostream& os, const GameState& board) {
    os << "GameState: {";
    os << "p1=" << board.p1 << " ";
    os << "p2=" << board.p2 << " ";
    os << "player=" << board.player << " ";
    os << "depth=" << board.depth << " ";
    os << "xorHash=" << board.xorHash << " ";
    os << "emptyPos=";
    for(unsigned i=0; i<WIDTH; i++) {
        os << static_cast<int>(board.emptyPos[i]) << (i==WIDTH-1 ? "" : " ");
    }
    os << "}";
    return os;
}

void GameState::generateDerivedFields() {
    this->xorHash = hasher.hash(this->p1, this->p2, this->player);

    uint64 board = this->p1 | this->p2;
    uint64 mask;
    for(unsigned col=0; col<WIDTH; col++) {
        this->emptyPos[col] = HEIGHT;
        for(unsigned row=0; row<HEIGHT; row++) {
            mask = Bitboard::toMask(row, col);
            if((board & mask) == 0) {
                this->emptyPos[col] = row;
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

void GameState::makeMove(unsigned row, unsigned col) {
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

uint64 reverseBits(uint64 x) {
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

GameState GameState::flip() const {
    return GameState(reverseBits(p1), reverseBits(p2), flipPlayer(player), depth);
}

int flipPlayer(int player) {
    return player == PLAYER_MAX ? PLAYER_MIN : PLAYER_MAX;
}

Value flipValue(Value value) {
    if(value == VALUE_UNKNOWN) return VALUE_UNKNOWN;
    return 256 - value;
}