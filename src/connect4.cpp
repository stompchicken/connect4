#include "connect4.hpp"

#include <algorithm>
#include <cassert>

inline unsigned toIndex(unsigned row, unsigned col) {
    return (col * (HEIGHT+1)) + row;
}

inline unsigned toRow(unsigned index) {
    return index % (HEIGHT+1);
}

inline unsigned toCol(unsigned index) {
    return index / (HEIGHT+1);
}

inline uint64 toMask(unsigned row, unsigned col) {
    return (uint64)1 << toIndex(row, col);
}

int popcount(uint64 x)
{
    int c = 0;
    for (; x > 0; x &= x -1) c++;
    return c;
}

uint64 makeZeroBarrier() {
    uint64 z = 0;
    for(unsigned row=0; row<HEIGHT; row++) {
        for(unsigned col=0; col<WIDTH; col++) {
            z |= toMask(row, col);
        }
    }
    return z;
}

uint64 Connect4::zeroBarrier = makeZeroBarrier();
Connect4::Hasher Connect4::hasher;

Connect4::Connect4() : p1(0), p2(0), player(PLAYER_MAX), depth(0) {
    std::fill(emptyPos, emptyPos+WIDTH, 0);
    xorHash = 0;

}

Connect4::Connect4(uint64 p1_, uint64 p2_, int player_, int depth_) : p1(p1_ & zeroBarrier),
                                                                      p2(p2_ & zeroBarrier),
                                                                      player(player_),
                                                                      depth(depth_) {
    this->generateDerivedFields();
}

Connect4::Connect4(const Connect4& other) {
    this->p1 = other.p1;
    this->p2 = other.p2;
    this->player = other.player;
    this->depth = other.depth;
    this->xorHash = other.xorHash;
    std::copy(other.emptyPos, other.emptyPos+WIDTH, this->emptyPos);
}

Connect4& Connect4::operator=(const Connect4& other) {
    this->p1 = other.p1;
    this->p2 = other.p2;
    this->player = other.player;
    this->depth = other.depth;
    this->xorHash = other.xorHash;
    std::copy(other.emptyPos, other.emptyPos+WIDTH, this->emptyPos);
    return *this;
}

Connect4 Connect4::random(Depth moves) {
    Connect4 board;
    do {
        board = Connect4();
        for(unsigned i=0; i<moves; i++) {
            Connect4 nodeBuffer[WIDTH];
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

bool Connect4::operator==(const Connect4& rhs) const {
    return p1 == rhs.p1 &&
        p2 == rhs.p2 &&
        player == rhs.player &&
        depth == rhs.depth &&
        xorHash == rhs.xorHash &&
        std::equal(emptyPos, emptyPos + WIDTH, rhs.emptyPos);
}

bool Connect4::operator!=(const Connect4& rhs) const {
    return !(*this == rhs);
}

// Pass a pre-assigned buffer of size WIDTH
void Connect4::children(Connect4* buffer) const {
    for(unsigned col=0; col<WIDTH; col++) {
        Connect4* child = buffer+col;
        unsigned row = emptyPos[col];
        if(row < HEIGHT) {
            child->p1 = p1;
            child->p2 = p2;
            child->depth = depth;
            child->player = player;
            std::copy(emptyPos, emptyPos+WIDTH, child->emptyPos);
            child->makeMove(row, col);
            unsigned index = toIndex(row, col);
            child->xorHash = hasher.incHash(xorHash, index, player);
        } else {
            child->player = PLAYER_INVALID;
        }
    }

}

Value Connect4::evaluate() const {
    if(line4(p1) > 0) {
        return VALUE_MAX;
    } else if(line4(p2) > 0) {
        return VALUE_MIN;
    }

    if((p1|p2) == zeroBarrier) {
        return VALUE_DRAW;
    }

    return VALUE_UNKNOWN;
}

uint64 Connect4::hotspot3(uint64 p1, uint64 p2) const {
    uint64 vertical = (p1 & (p1 << 1) & (p1 << 2) & ~(p2 << 3)) |
        (p1 & (p1 >> 1) & (p1 >> 2) & ~(p2 >> 3));
    uint64 horizontal = (p1 & (p1 << (HEIGHT+1)) & (p1 << (2*(HEIGHT+1))) & ~(p2 << (3*(HEIGHT+1)))) |
        (p1 & (p1 >> (HEIGHT+1)) & (p1 >> (2*(HEIGHT+1))) & ~(p2 >> (3*(HEIGHT+1))));
    uint64 diagRight = (p1 & (p1 << (HEIGHT+2)) & (p1 << (2*(HEIGHT+2))) & ~(p2 << (3*(HEIGHT+2)))) |
        (p1 & (p1 >> (HEIGHT+2)) & (p1 >> (2*(HEIGHT+2))) & ~(p2 >> (3*(HEIGHT+2))));
    uint64 diagLeft = (p1 & (p1 << HEIGHT) & (p1 << (2*HEIGHT)) & ~(p2 << (3*HEIGHT))) |
        (p1 & (p1 >> HEIGHT) & (p1 >> (2*HEIGHT)) & ~(p2 >> (3*HEIGHT)));
    return vertical | horizontal | diagLeft | diagRight;
}

Value Connect4::heuristic() const {
    int p13 = popcount(hotspot3(p1, p2));
    int p23 = popcount(hotspot3(p2, p1));
    return (16*(p13 - p23)) + 128;
}

void Connect4::flipBoard() {
    uint64 temp = p2;
    p2 = p1;
    p1 = temp;
}

std::string Connect4::print() const {
    std::string text;
    uint64 mask;
    for(unsigned row=0; row<HEIGHT; row++) {
        for(unsigned col=0; col<WIDTH; col++) {
            mask = toMask((HEIGHT-1)-row, col);

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

void Connect4::parse(std::string text, int player_, int depth_) {
    p1 = p2 = 0;
    uint64 mask;
    unsigned col = 0;
    unsigned row = HEIGHT - 1;
    for(unsigned i = 0; i < text.size(); i++) {
        mask = toMask(row, col);
        if(text[i] == '\n') {
            row -= 1;
            col = 0;
        } else if(text[i] == '|') {
            col += 1;
        } else if(text[i] == 'X') {
            p1 |= mask;
        } else if(text[i] == 'O') {
            p2 |= mask;
        }
    }

    this->player = player_;
    this->depth = depth_;
    this->generateDerivedFields();
}

std::ostream& operator<<(std::ostream& os, const Connect4& board) {
    os << "Connect4: {";
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

void Connect4::generateDerivedFields() {
    this->xorHash = hasher.hash(this->p1, this->p2, this->player);

    uint64 board = this->p1 | this->p2;
    uint64 mask;
    for(unsigned col=0; col<WIDTH; col++) {
        this->emptyPos[col] = HEIGHT;
        for(unsigned row=0; row<HEIGHT; row++) {
            mask = toMask(row, col);
            if((board & mask) == 0) {
                this->emptyPos[col] = row;
                break;
            }
        }
    }
}

void Connect4::assertInvariants() const {
    assert((p1 & p2) == 0);
    assert(player == PLAYER_MAX || player == PLAYER_MIN);
    assert(((p1 & ~zeroBarrier) == 0) && ((p2 & ~zeroBarrier) == 0));
    assert(xorHash == hasher.hash(p1, p2, player));
}

void Connect4::makeMove(unsigned row, unsigned col) {
    if(player == PLAYER_MAX) {
        this->p1 |= toMask(row, col);
        this->player = PLAYER_MIN;
    } else {
        this->p2 |= toMask(row, col);
        this->player = PLAYER_MAX;
    }
    this->depth += 1;
    this->emptyPos[col] += 1;

}

uint64 Connect4::line4(uint64 b) const {
    uint64 x;

    x = (b << 2) & b;
    uint64 vertical = x & (x << 1);

    x = (b << (2*(HEIGHT+1))) & b;
    uint64 horizontal = x & (x << (HEIGHT+1));

    x = (b << (2*(HEIGHT+2))) & b;
    uint64 diagRight = x & (x << (HEIGHT+2));

    x = (b << (2*(HEIGHT))) & b;
    uint64 diagLeft = x & (x << (HEIGHT));

    return vertical || horizontal || diagLeft || diagRight;
}
/*
Connect4 genBoard(int moves) {
    Connect4 board;
    do {
        board = Connect4();
        for(int i=0; i<moves; i++) {
            Connect4 nodeBuffer[WIDTH];
            board.children(nodeBuffer);

            Connect4 newBoard = nodeBuffer[rand() % WIDTH];
            while(!newBoard.isValid()) {
                newBoard = nodeBuffer[rand() % WIDTH];
            }
            board = newBoard;
        }
    } while (board.evaluate() != VALUE_UNKNOWN);

    return board;
}
*/
