#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "Utils.h"
#include "User.h"
#include "Board.h"
#include "ScoreCard.h"
#include "History.h"
#include "GameSession.h"
#include "FileManager.h"

// ─── Admin credentials (case-sensitive login) ─────────────────────────────────
const std::string ADMIN_USERNAME = "Admin";
const std::string ADMIN_PASSWORD = "admin123";
const std::string ADMIN_ID       = "ADM-001";

// ─── Global state ─────────────────────────────────────────────────────────────
std::vector<Player*> allPlayers;
ScoreCard scoreCard;
Admin*    adminUser = nullptr;

// ─── Forward declarations ─────────────────────────────────────────────────────
void    mainMenu();
Player* loginOrRegisterPlayer();
bool    adminLogin();
void    playerMenu(Player* p);
void    adminMenu();
void    playGame(Player* p1, Player* p2);
Player* selectOpponent(Player* self);
void    showLeaderboard();
void    showPlayerHistory(Player* p);
void    deletePlayer();
void    cleanupMemory();

// ─────────────────────────────────────────────────────────────────────────────
//  Entry Point
// ─────────────────────────────────────────────────────────────────────────────
int main() {
    Utils::enableANSI();

    allPlayers = FileManager::loadPlayers();
    for (auto* p : allPlayers)
        scoreCard.registerPlayer(p);

    adminUser = new Admin(ADMIN_USERNAME, ADMIN_PASSWORD, ADMIN_ID);

    mainMenu();

    cleanupMemory();
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Main Menu
// ─────────────────────────────────────────────────────────────────────────────
void mainMenu() {
    while (true) {
        Utils::clearScreen();
        std::cout << "\n\n";
        std::cout << Color::CYAN << Color::BOLD;
        std::cout << "  ============================\n";
        std::cout << "       TIC  TAC  TOE\n";
        std::cout << "  ============================\n";
        std::cout << Color::RESET << "\n";
        std::cout << "   [1]  Player Login / Register\n";
        std::cout << "   [2]  Admin Login\n";
        std::cout << "   [3]  View Leaderboard\n";
        std::cout << "   [0]  Exit\n\n";
        Utils::printLine('-', 32);

        int choice = Utils::getInt("   > ", 0, 3);

        if (choice == 0) {
            Utils::clearScreen();
            std::cout << "\n\n" << Color::DIM
                      << "  Thanks for playing. Goodbye!\n\n"
                      << Color::RESET;
            Utils::sleep(800);
            break;
        } else if (choice == 1) {
            Player* p = loginOrRegisterPlayer();
            if (p) playerMenu(p);
        } else if (choice == 2) {
            if (adminLogin()) adminMenu();
        } else if (choice == 3) {
            showLeaderboard();
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Player Login / Register
// ─────────────────────────────────────────────────────────────────────────────
Player* loginOrRegisterPlayer() {
    Utils::printHeader("Player Login");
    std::string name = Utils::getString("  Enter your name: ");
    if (name.empty()) return nullptr;

    Player* found = scoreCard.findPlayer(name);   // uses map lookup

    if (found) {
        std::cout << Color::GREEN << "\n  Welcome back, "
                  << found->getUsername() << "!\n" << Color::RESET;
        Utils::sleep(900);
        return found;
    } else {
        std::cout << Color::CYAN << "\n  New player! Registering \""
                  << name << "\"...\n" << Color::RESET;
        Utils::sleep(700);
        Player* newP = new Player(name, "");
        allPlayers.push_back(newP);
        scoreCard.registerPlayer(newP);
        FileManager::savePlayers(allPlayers);
        return newP;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Admin Login
// ─────────────────────────────────────────────────────────────────────────────
bool adminLogin() {
    Utils::printHeader("Admin Login");
    std::cout << Color::DIM << "  (Username and password are case-sensitive)\n\n" << Color::RESET;
    std::string uname = Utils::getString("  Username : ");
    std::string pass  = Utils::getString("  Password : ");

    if (adminUser->checkLogin(uname, pass)) {
        std::cout << Color::GREEN << "\n  Access granted. Welcome, Admin.\n" << Color::RESET;
        Utils::sleep(800);
        return true;
    }
    std::cout << Color::RED << "\n  Invalid credentials.\n" << Color::RESET;
    Utils::sleep(1200);
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Player Menu
// ─────────────────────────────────────────────────────────────────────────────
void playerMenu(Player* p) {
    while (true) {
        Utils::printHeader("Player Menu");
        p->displayDashboard();   // virtual dispatch

        std::cout << "\n";
        Utils::printLine('-', 32);
        std::cout << "   [1]  Play a Game\n";
        std::cout << "   [2]  My Game History\n";
        std::cout << "   [3]  Leaderboard\n";
        std::cout << "   [0]  Logout\n\n";

        int choice = Utils::getInt("   > ", 0, 3);

        if      (choice == 0) break;
        else if (choice == 1) {
            Player* opp = selectOpponent(p);
            if (opp) playGame(p, opp);
        }
        else if (choice == 2) showPlayerHistory(p);
        else if (choice == 3) showLeaderboard();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Select Opponent
// ─────────────────────────────────────────────────────────────────────────────
Player* selectOpponent(Player* self) {
    Utils::printHeader("Select Opponent");

    std::vector<Player*> others;
    for (auto* p : allPlayers)
        if (p != self) others.push_back(p);

    if (others.empty()) {
        std::cout << Color::YELLOW
                  << "  No other players found.\n"
                  << "  Go back and register a second player first.\n"
                  << Color::RESET;
        Utils::sleep(1800);
        return nullptr;
    }

    std::cout << "  Choose your opponent:\n\n";
    for (size_t i = 0; i < others.size(); i++) {
        std::cout << "   [" << (i + 1) << "]  "
                  << Color::BOLD << others[i]->getUsername() << Color::RESET
                  << Color::DIM  << " - " << others[i]->getTotalPoints() << " pts"
                  << Color::RESET << "\n";
    }
    std::cout << "   [0]  Back\n\n";

    int choice = Utils::getInt("   > ", 0, (int)others.size());
    if (choice == 0) return nullptr;
    return others[choice - 1];
}

// ─────────────────────────────────────────────────────────────────────────────
//  Play Game  —  includes rematch loop
// ─────────────────────────────────────────────────────────────────────────────
void playGame(Player* p1, Player* p2) {
    // Marker selection once — carries across rematches
    Utils::printHeader("Choose Marker");
    std::cout << "  " << Color::BOLD << p1->getUsername() << Color::RESET
              << ", pick your marker:\n\n";
    std::cout << "   [1]  " << Color::RED  << Color::BOLD << "X" << Color::RESET << "\n";
    std::cout << "   [2]  " << Color::BLUE << Color::BOLD << "O" << Color::RESET << "\n\n";

    int pick = Utils::getInt("   > ", 1, 2);
    char m1  = (pick == 1) ? 'X' : 'O';
    char m2  = (m1 == 'X') ? 'O' : 'X';

    BaseUser* bu1 = static_cast<BaseUser*>(p1);
    BaseUser* bu2 = static_cast<BaseUser*>(p2);

    // ── Rematch loop ──────────────────────────────────────────────────────────
    while (true) {
        GameSession session(bu1, bu2, m1, m2);
        session.play(allPlayers);
        scoreCard.sortByPoints();

        // Ask for rematch
        std::cout << "\n";
        Utils::printLine('-', 32);
        std::cout << "   [1]  Rematch (same markers)\n";
        std::cout << "   [2]  Rematch (swap markers)\n";
        std::cout << "   [0]  Back to Menu\n\n";

        int again = Utils::getInt("   > ", 0, 2);
        if (again == 0) break;
        if (again == 2) {
            std::swap(m1, m2);
            std::swap(bu1, bu2);
            std::swap(p1, p2);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Admin Menu
// ─────────────────────────────────────────────────────────────────────────────
void adminMenu() {
    while (true) {
        Utils::printHeader("Admin Panel");
        adminUser->displayDashboard();   // virtual dispatch -> Admin version

        std::cout << "\n";
        Utils::printLine('-', 32);
        std::cout << "   [1]  Full Leaderboard\n";
        std::cout << "   [2]  View a Player's History\n";
        std::cout << "   [3]  All Player Stats\n";
        std::cout << "   [4]  Delete a Player\n";
        std::cout << "   [0]  Logout\n\n";

        int choice = Utils::getInt("   > ", 0, 4);

        if (choice == 0) break;
        else if (choice == 1) {
            showLeaderboard();
        }
        else if (choice == 2) {
            Utils::printHeader("Player History Lookup");
            std::string name = Utils::getString("  Player name: ");
            Player* p = scoreCard.findPlayer(name);
            if (!p) {
                std::cout << Color::RED << "  Player not found.\n" << Color::RESET;
                Utils::sleep(1000);
            } else {
                showPlayerHistory(p);
            }
        }
        else if (choice == 3) {
            Utils::printHeader("All Player Stats");
            scoreCard.sortByPoints();
            if (allPlayers.empty()) {
                std::cout << Color::DIM << "  No players registered yet.\n" << Color::RESET;
            } else {
                for (auto* p : allPlayers)
                    std::cout << "  " << *p << "\n";
            }
            std::cout << "\n" << Color::DIM << "  Press Enter to continue..." << Color::RESET;
            std::cin.get();
        }
        else if (choice == 4) {
            deletePlayer();
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Delete Player  (admin only)
// ─────────────────────────────────────────────────────────────────────────────
void deletePlayer() {
    Utils::printHeader("Delete Player");

    if (allPlayers.empty()) {
        std::cout << Color::DIM << "  No players to delete.\n" << Color::RESET;
        Utils::sleep(1000);
        return;
    }

    // List current players
    std::cout << "  Registered players:\n\n";
    for (size_t i = 0; i < allPlayers.size(); i++) {
        std::cout << "   [" << (i + 1) << "]  "
                  << Color::BOLD << allPlayers[i]->getUsername() << Color::RESET
                  << "\n";
    }
    std::cout << "   [0]  Cancel\n\n";

    int choice = Utils::getInt("   > ", 0, (int)allPlayers.size());
    if (choice == 0) return;

    Player* target = allPlayers[choice - 1];
    std::string name = target->getUsername();

    // Confirm
    std::cout << Color::RED << "\n  Delete \"" << name
              << "\"? This cannot be undone.\n" << Color::RESET;
    std::cout << "   [1]  Yes, delete\n";
    std::cout << "   [0]  Cancel\n\n";

    int confirm = Utils::getInt("   > ", 0, 1);
    if (confirm == 0) {
        std::cout << Color::DIM << "  Cancelled.\n" << Color::RESET;
        Utils::sleep(800);
        return;
    }

    // Remove from scorecard (vector + map)
    scoreCard.removePlayer(name);

    // Remove from allPlayers and free memory
    allPlayers.erase(
        std::remove(allPlayers.begin(), allPlayers.end(), target),
        allPlayers.end());
    delete target;

    // Rewrite players file and strip history
    FileManager::deletePlayer(name, allPlayers);

    std::cout << Color::GREEN << "\n  \"" << name << "\" has been deleted.\n" << Color::RESET;
    Utils::sleep(1200);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Leaderboard
// ─────────────────────────────────────────────────────────────────────────────
void showLeaderboard() {
    Utils::printHeader("Leaderboard");
    scoreCard.sortByPoints();
    scoreCard.displayLeaderboard();
    std::cout << "\n" << Color::DIM << "  Press Enter to continue..." << Color::RESET;
    std::cin.get();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Game History
// ─────────────────────────────────────────────────────────────────────────────
void showPlayerHistory(Player* p) {
    Utils::printHeader("Game History");
    History<std::string> hist = FileManager::loadHistory(p->getUsername());
    hist.display(p->getUsername() + "'s Games");
    std::cout << "\n" << Color::DIM << "  Press Enter to continue..." << Color::RESET;
    std::cin.get();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Cleanup
// ─────────────────────────────────────────────────────────────────────────────
void cleanupMemory() {
    for (auto* p : allPlayers)
        delete p;
    allPlayers.clear();
    delete adminUser;
}
