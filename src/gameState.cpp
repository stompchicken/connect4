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

uint64 makeBaseBarrier() {
    uint64 base = 0;
    for(unsigned col=0; col<WIDTH; col++) {
        base |= Bitboard::toMask(0, col);
    }
    return base;
}

uint64 Bitboard::zeroBarrier = makeZeroBarrier();
uint64 Bitboard::baseBarrier = makeBaseBarrier();

GameState::Hasher GameState::hasher;

GameState::GameState() : p1(0), p2(0), player(PLAYER_MAX), depth(0), emptyPos(), xorHash(0) {
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

GameState GameState::random(Depth moves) {
    GameState board;
    do {
        board = GameState();
        for(unsigned i=0; i<moves; i++) {
            GameState nodeBuffer[WIDTH];
            board.children(nodeBuffer);
            unsigned offset = static_cast<unsigned>(rand());
            for(unsigned j=0; j<WIDTH; j++) {
                board = nodeBuffer[(j + offset) % WIDTH];
                if(board.isValid()) break;
            }
        }
    } while (board.evaluate() != VALUE_UNKNOWN);
    return board;
}

GameState GameState::parse(std::string text) {
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
void GameState::children(GameState* buffer) const {
    for(unsigned col=0; col<WIDTH; col++) {
        GameState* child = buffer+col;
        unsigned row = emptyPos[col];
        if(row < HEIGHT) {
            child->p1 = p1;
            child->p2 = p2;
            child->depth = depth;
            child->player = player;
            for(int i=0; i<WIDTH; i++) {
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

uint8 GameState::heuristic() const {
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
    flip.p1 = Bitboard::flipLeftRight(p1);
    flip.p2 = Bitboard::flipLeftRight(p2);
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

int flipPlayer(Player player) {
    return player == PLAYER_MAX ? PLAYER_MIN : PLAYER_MAX;
}

std::string printPlayer(Player player) {
    return player == PLAYER_MAX ? "PLAYER_MAX" : "PLAYER_MIN";
}


Value flipValue(Value value) {
    if(value == VALUE_UNKNOWN) return VALUE_UNKNOWN;
    else if(value == VALUE_MAX) return VALUE_MIN;
    else if(value == VALUE_MIN) return VALUE_MAX;
    else return VALUE_DRAW;
}

std::string printValue(Value value) {
    if(value == VALUE_UNKNOWN) return "VALUE_UNKNOWN";
    else if(value == VALUE_MAX) return "VALUE_MAX";
    else if(value == VALUE_MIN) return "VALUE_MIN";
    else return "VALUE_DRAW";

}

std::ostream& operator<<(std::ostream &output, const Moves &moves) {
    output << "{ Moves ";
    for(unsigned i=0; i<WIDTH; i++) {
        output << "(" << moves.move[i] << ") ";
    }
    output << "}";
    return output;
}

#define SWAP(x, y) if(value[x] < value[y]) { tempMove = move[y]; tempValue = value[y]; move[y] = move[x]; value[y] = value[x]; move[x] = tempMove; value[x] = tempValue; }

uint8 staticMoveOrder[WIDTH] = {2, 4, 6, 7, 5, 3};
//uint8 staticMoveOrder[WIDTH] = {7, 6, 5, 4, 3, 2};
unsigned Moves::killerMove[DEPTH_MAX] = {MOVE_INVALID};

void Moves::reorder(unsigned bestMove) {
    for(unsigned i=0; i<WIDTH; i++) {
        move[i] = i;
        value[i] = staticMoveOrder[i];

        if(i == killerMove[depth]) { value[i] = 10; }
//        if(i == bestMove) { value[i] = 11; }
    }

    // Sorting network
    unsigned tempMove;
    unsigned tempValue;
#if WIDTH == 7
    SWAP(1, 2); SWAP(0, 2); SWAP(0, 1); SWAP(3, 4);
    SWAP(5, 6); SWAP(3, 5); SWAP(4, 6); SWAP(4, 5);
    SWAP(0, 4); SWAP(0, 3); SWAP(1, 5); SWAP(2, 6);
    SWAP(2, 5); SWAP(1, 3); SWAP(2, 4); SWAP(2, 3);
#else
    SWAP(1, 2); SWAP(0, 2); SWAP(0, 1); SWAP(4, 5);
    SWAP(3, 5); SWAP(3, 4); SWAP(0, 3); SWAP(1, 4);
    SWAP(2, 5); SWAP(2, 4); SWAP(1, 3); SWAP(2, 3);
#endif
}
