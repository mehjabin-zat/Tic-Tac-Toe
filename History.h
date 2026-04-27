#ifndef HISTORY_H
#define HISTORY_H

#include <vector>
#include <string>
#include <iostream>
#include "Utils.h"

// ─── Template History Container ──────────────────────────────────────────────
// Stores a list of entries of any type T.
// Used for: GameRecord history, score snapshots, etc.

template <typename T>
class History {
private:
    std::vector<T> entries;
    size_t maxSize;

public:
    History(size_t max = 100) : maxSize(max) {}

    void add(const T& entry) {
        if (entries.size() >= maxSize)
            entries.erase(entries.begin()); // drop oldest if full
        entries.push_back(entry);
    }

    void display(const std::string& title = "History") const {
        std::cout << Color::BOLD << Color::CYAN << "\n  " << title << "\n" << Color::RESET;
        Utils::printLine('-', 36);
        if (entries.empty()) {
            std::cout << Color::DIM << "  No records found.\n" << Color::RESET;
            return;
        }
        int idx = 1;
        for (const auto& e : entries) {
            std::cout << "  " << Color::DIM << idx++ << ". " << Color::RESET << e << "\n";
        }
    }

    const std::vector<T>& getAll() const { return entries; }

    void clear() { entries.clear(); }

    size_t size() const { return entries.size(); }

    // Operator overload: append two histories
    History<T> operator+(const History<T>& other) const {
        History<T> combined(maxSize + other.maxSize);
        for (const auto& e : entries)       combined.add(e);
        for (const auto& e : other.entries) combined.add(e);
        return combined;
    }
};

#endif
