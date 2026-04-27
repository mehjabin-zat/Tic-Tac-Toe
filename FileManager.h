#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "User.h"
#include "ScoreCard.h"

// ─── FileManager ──────────────────────────────────────────────────────────────
// Handles all file I/O. Saves and loads player data and game history.
// Format (players.dat): username|password|wins|losses|ties|points
// Format (history.dat): username|record

class FileManager {
private:
    static const std::string PLAYERS_FILE;
    static const std::string HISTORY_FILE;
    static const std::string ADMIN_FILE;

public:

    // ── Save all players ──────────────────────────────────────────────────────
    static void savePlayers(const std::vector<Player*>& players) {
        std::ofstream out(PLAYERS_FILE);
        if (!out) {
            std::cerr << Color::RED << "  [Error] Could not open " << PLAYERS_FILE << "\n" << Color::RESET;
            return;
        }
        for (const auto* p : players) {
            out << p->getUsername() << "|"
                << "pass" << "|"       // passwords stored as-is (project scope)
                << p->getWins()   << "|"
                << p->getLosses() << "|"
                << p->getTies()   << "|"
                << p->getTotalPoints() << "\n";
        }
    }

    // ── Load players into scorecard ───────────────────────────────────────────
    // Returns a vector of heap-allocated Player*. Caller owns memory.
    static std::vector<Player*> loadPlayers() {
        std::vector<Player*> players;
        std::ifstream in(PLAYERS_FILE);
        if (!in) return players; // no file yet = first run

        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::istringstream ss(line);
            std::string uname, pass, w, l, t, pts;
            if (std::getline(ss, uname, '|') &&
                std::getline(ss, pass,  '|') &&
                std::getline(ss, w,     '|') &&
                std::getline(ss, l,     '|') &&
                std::getline(ss, t,     '|') &&
                std::getline(ss, pts,   '|')) {
                Player* p = new Player(uname, pass);
                p->setWins(std::stoi(w));
                p->setLosses(std::stoi(l));
                p->setTies(std::stoi(t));
                p->setTotalPoints(std::stoi(pts));
                players.push_back(p);
            }
        }
        return players;
    }

    // ── Append a single game record ───────────────────────────────────────────
    static void appendGameRecord(const std::string& playerName, const std::string& record) {
        std::ofstream out(HISTORY_FILE, std::ios::app);
        if (!out) return;
        out << playerName << "|" << record << "\n";
    }

    // ── Load history for a specific player ────────────────────────────────────
    static History<std::string> loadHistory(const std::string& playerName) {
        History<std::string> hist(50);
        std::ifstream in(HISTORY_FILE);
        if (!in) return hist;

        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::istringstream ss(line);
            std::string name, record;
            if (std::getline(ss, name, '|') && std::getline(ss, record)) {
                if (Utils::toLower(name) == Utils::toLower(playerName))
                    hist.add(record);
            }
        }
        return hist;
    }

    // ── Check if player name exists ───────────────────────────────────────────
    static bool playerExists(const std::string& name) {
        std::ifstream in(PLAYERS_FILE);
        if (!in) return false;
        std::string line;
        while (std::getline(in, line)) {
            std::istringstream ss(line);
            std::string uname;
            std::getline(ss, uname, '|');
            if (Utils::toLower(uname) == Utils::toLower(name))
                return true;
        }
        return false;
    }

    // ── Delete a player from file ─────────────────────────────────────────────
    static void deletePlayer(const std::string& name, const std::vector<Player*>& remaining) {
        savePlayers(remaining); // rewrite file without the deleted player

        // Also strip their history entries
        std::ifstream in(HISTORY_FILE);
        std::string allLines;
        if (in) {
            std::string line;
            while (std::getline(in, line)) {
                if (line.empty()) continue;
                std::istringstream ss(line);
                std::string pname;
                std::getline(ss, pname, '|');
                if (Utils::toLower(pname) != Utils::toLower(name))
                    allLines += line + "\n";
            }
            in.close();
        }
        std::ofstream out(HISTORY_FILE);
        if (out) out << allLines;
    }

    // ── Save single player (update their record) ──────────────────────────────
    static void updatePlayer(const Player* player, const std::vector<Player*>& allPlayers) {
        savePlayers(allPlayers);
    }
};

const std::string FileManager::PLAYERS_FILE = "players.dat";
const std::string FileManager::HISTORY_FILE  = "history.dat";
const std::string FileManager::ADMIN_FILE    = "admin.dat";

#endif
