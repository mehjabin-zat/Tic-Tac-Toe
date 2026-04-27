#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <string>
#include "Utils.h"

// ─── Board ────────────────────────────────────────────────────────────────────
// Manages the 3x3 grid. Owns its data via a dynamically allocated 2D array
// (demonstrates pointer usage). Operator<< prints to any ostream.

class Board {
private:
    char** grid;       // pointer-based 2D array
    int size;

    std::string coloredCell(char c) const;

public:
    Board();
    Board(const Board& other);           // copy constructor
    Board& operator=(const Board& other);
    ~Board();                            // destructor frees dynamic memory

    void reset();
    bool placeMarker(int slot, char marker);
    char checkWinner() const;  // returns winning marker ('X'/'O'), 'T'=tie, '\0'=none
    bool isFull() const;
    char getCell(int row, int col) const;

    // Operator overload: print board to any ostream (cout, file, etc.)
    friend std::ostream& operator<<(std::ostream& os, const Board& b);

    // Operator== : compare two boards (same state)
    bool operator==(const Board& other) const;
};

#endif
