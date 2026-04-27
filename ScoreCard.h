#ifndef SCORECARD_H
#define SCORECARD_H

#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "User.h"

// ─── ScoreCard ────────────────────────────────────────────────────────────────
class ScoreCard {
private:
    std::vector<Player*> players;           // STL vector  — ordered list
    std::map<std::string, Player*> lookup;  // STL map     — fast name lookup

public:
    ScoreCard()  {}
    ~ScoreCard() {}

    void registerPlayer(Player* p) {
        std::string key = Utils::toLower(p->getUsername());
        if (lookup.count(key)) return;      // already registered
        players.push_back(p);
        lookup[key] = p;
    }

    void removePlayer(const std::string& name) {
        std::string key = Utils::toLower(name);
        lookup.erase(key);
        players.erase(
            std::remove_if(players.begin(), players.end(),
                [&](Player* p) {
                    return Utils::toLower(p->getUsername()) == key;
                }),
            players.end()
        );
    }

    // Multi-level sort:
    // 1. Points (desc)  2. Wins (desc)  3. Ties (desc)
    // 4. Losses (asc)   5. Name (asc, alphabetical)
    void sortByPoints() {
        std::sort(players.begin(), players.end(),
            [](const Player* a, const Player* b) {
                if (a->getTotalPoints() != b->getTotalPoints())
                    return a->getTotalPoints() > b->getTotalPoints();
                if (a->getWins() != b->getWins())
                    return a->getWins() > b->getWins();
                if (a->getTies() != b->getTies())
                    return a->getTies() > b->getTies();
                if (a->getLosses() != b->getLosses())
                    return a->getLosses() < b->getLosses();
                return Utils::toLower(a->getUsername()) < Utils::toLower(b->getUsername());
            });
    }

    void displayLeaderboard() const {
        std::cout << Color::BOLD << Color::YELLOW
                  << "\n  [ LEADERBOARD ]\n" << Color::RESET;
        Utils::printLine('=', 52);
        std::cout << Color::BOLD
                  << "  " << std::left
                  << std::setw(16) << "Name"
                  << " | " << std::setw(5)  << "Wins"
                  << " | " << std::setw(6)  << "Losses"
                  << " | " << std::setw(5)  << "Ties"
                  << " | Points\n" << Color::RESET;
        Utils::printLine('-', 52);

        if (players.empty()) {
            std::cout << Color::DIM << "  No players yet.\n" << Color::RESET;
            return;
        }

        int rank = 1;
        for (const auto* p : players) {
            std::string rankStr;
            std::string rankColor;
            if      (rank == 1) { rankStr = " #1 "; rankColor = Color::YELLOW; }
            else if (rank == 2) { rankStr = " #2 "; rankColor = Color::WHITE;  }
            else if (rank == 3) { rankStr = " #3 "; rankColor = Color::RED;    }
            else                { rankStr = "    "; rankColor = Color::RESET;   }

            std::cout << rankColor << rankStr << Color::RESET
                      << std::left << std::setw(16) << p->getUsername()
                      << " | " << std::setw(5)  << p->getWins()
                      << " | " << std::setw(6)  << p->getLosses()
                      << " | " << std::setw(5)  << p->getTies()
                      << " | " << Color::BOLD   << p->getTotalPoints()
                      << Color::RESET << " pts\n";
            rank++;
        }
        Utils::printLine('=', 52);
        std::cout << Color::DIM
                  << "  Sorted by: Points > Wins > Ties > Fewer Losses > Name\n"
                  << Color::RESET;
    }

    // Operator+ : merge two scorecards
    ScoreCard operator+(const ScoreCard& other) const {
        ScoreCard merged;
        for (auto* p : players)       merged.registerPlayer(p);
        for (auto* p : other.players) merged.registerPlayer(p);
        return merged;
    }

    const std::vector<Player*>& getPlayers() const { return players; }

    // O(log n) lookup via map instead of linear scan
    Player* findPlayer(const std::string& name) const {
        auto it = lookup.find(Utils::toLower(name));
        return (it != lookup.end()) ? it->second : nullptr;
    }
};

#endif
