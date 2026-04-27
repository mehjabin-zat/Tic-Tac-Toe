#include "Board.h"

// ─── Constructor / Destructor ────────────────────────────────────────────────

Board::Board() : size(3) {
    grid = new char*[size];
    for (int i = 0; i < size; i++)
        grid[i] = new char[size];
    reset();
}

Board::Board(const Board& other) : size(other.size) {
    grid = new char*[size];
    for (int i = 0; i < size; i++) {
        grid[i] = new char[size];
        for (int j = 0; j < size; j++)
            grid[i][j] = other.grid[i][j];
    }
}

Board& Board::operator=(const Board& other) {
    if (this == &other) return *this;
    // free old
    for (int i = 0; i < size; i++) delete[] grid[i];
    delete[] grid;
    // copy new
    size = other.size;
    grid = new char*[size];
    for (int i = 0; i < size; i++) {
        grid[i] = new char[size];
        for (int j = 0; j < size; j++)
            grid[i][j] = other.grid[i][j];
    }
    return *this;
}

Board::~Board() {
    for (int i = 0; i < size; i++)
        delete[] grid[i];
    delete[] grid;
}

// ─── Reset ────────────────────────────────────────────────────────────────────

void Board::reset() {
    char num = '1';
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            grid[i][j] = num++;
}

// ─── Place Marker ─────────────────────────────────────────────────────────────

bool Board::placeMarker(int slot, char marker) {
    if (slot < 1 || slot > 9) return false;
    int row = (slot - 1) / 3;
    int col = (slot - 1) % 3;
    if (grid[row][col] == 'X' || grid[row][col] == 'O')
        return false;
    grid[row][col] = marker;
    return true;
}

// ─── Check Winner ─────────────────────────────────────────────────────────────
// Returns winning marker ('X' or 'O'), 'T' for tie, '\0' for no result yet

char Board::checkWinner() const {
    // Check rows and columns
    for (int i = 0; i < 3; i++) {
        // Row
        if (grid[i][0] == grid[i][1] && grid[i][1] == grid[i][2]
            && (grid[i][0] == 'X' || grid[i][0] == 'O'))
            return grid[i][0];
        // Column
        if (grid[0][i] == grid[1][i] && grid[1][i] == grid[2][i]
            && (grid[0][i] == 'X' || grid[0][i] == 'O'))
            return grid[0][i];
    }
    // Diagonals
    if (grid[0][0] == grid[1][1] && grid[1][1] == grid[2][2]
        && (grid[0][0] == 'X' || grid[0][0] == 'O'))
        return grid[0][0];
    if (grid[0][2] == grid[1][1] && grid[1][1] == grid[2][0]
        && (grid[0][2] == 'X' || grid[0][2] == 'O'))
        return grid[0][2];

    if (isFull()) return 'T'; // tie
    return '\0';              // game still going
}

bool Board::isFull() const {
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (grid[i][j] != 'X' && grid[i][j] != 'O')
                return false;
    return true;
}

char Board::getCell(int row, int col) const {
    return grid[row][col];
}

// ─── Colored Cell Helper ──────────────────────────────────────────────────────

std::string Board::coloredCell(char c) const {
    if (c == 'X') return Color::RED  + Color::BOLD + std::string(1, c) + Color::RESET;
    if (c == 'O') return Color::BLUE + Color::BOLD + std::string(1, c) + Color::RESET;
    return Color::DIM + std::string(1, c) + Color::RESET;
}

// ─── Operator<< : Print Board ─────────────────────────────────────────────────

std::ostream& operator<<(std::ostream& os, const Board& b) {
    os << "\n";
    for (int i = 0; i < 3; i++) {
        os << "   ";
        for (int j = 0; j < 3; j++) {
            os << " " << b.coloredCell(b.grid[i][j]) << " ";
            if (j < 2) os << Color::DIM << "|" << Color::RESET;
        }
        os << "\n";
        if (i < 2) {
            os << "   " << Color::DIM << "---+---+---" << Color::RESET << "\n";
        }
    }
    os << "\n";
    return os;
}

// ─── Operator== ───────────────────────────────────────────────────────────────

bool Board::operator==(const Board& other) const {
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (grid[i][j] != other.grid[i][j]) return false;
    return true;
}
