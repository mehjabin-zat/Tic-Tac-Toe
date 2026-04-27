#ifndef GAMESESSION_H
#define GAMESESSION_H

#include "Board.h"
#include "User.h"
#include "ScoreCard.h"
#include "FileManager.h"
#include <ctime>
#include <sstream>
#include <iomanip>

// ─── Game Result Enum ─────────────────────────────────────────────────────────
enum class GameResult { PLAYER1_WIN, PLAYER2_WIN, TIE, ONGOING };

// ─── GameSession ──────────────────────────────────────────────────────────────
class GameSession {
private:
    BaseUser*  p1;
    BaseUser*  p2;
    Player*    player1;
    Player*    player2;
    Board*     board;
    char       markerP1;
    char       markerP2;
    int        currentTurn;
    GameResult result;

    std::string getCurrentTimestamp() const {
        time_t now = time(nullptr);
        char buf[20];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", localtime(&now));
        return std::string(buf);
    }

    void printGameHeader() const {
        Utils::clearScreen();
        std::cout << "\n";
        std::cout << Color::CYAN << Color::BOLD
                  << "  +--------------------------+\n"
                  << "  |     TIC  TAC  TOE        |\n"
                  << "  +--------------------------+\n"
                  << Color::RESET;

        // Score line
        std::cout << "\n  "
                  << Color::RED   << Color::BOLD << markerP1 << " " << Color::RESET
                  << Color::BOLD  << player1->getUsername() << Color::RESET
                  << Color::DIM   << " (" << player1->getTotalPoints() << " pts)" << Color::RESET
                  << "   vs   "
                  << Color::BLUE  << Color::BOLD << markerP2 << " " << Color::RESET
                  << Color::BOLD  << player2->getUsername() << Color::RESET
                  << Color::DIM   << " (" << player2->getTotalPoints() << " pts)" << Color::RESET
                  << "\n";
        Utils::printLine('-', 40);
    }

    void showCongratsIfNeeded(Player* p) const {
        if (!p->hasEarnedCongrats()) return;
        Utils::clearScreen();
        std::string msg = p->getUsername() + " reached " + std::to_string(p->getWins()) + " wins!";
        std::cout << "\n\n" << Color::YELLOW << Color::BOLD;
        std::cout << "  +----------------------------------+\n";
        std::cout << "  |    *** CONGRATULATIONS! ***      |\n";
        std::cout << "  |                                  |\n";
        std::cout << "  |  " << std::left << std::setw(33) << msg << "|\n";
        std::cout << "  |  You are on fire! Keep it up!   |\n";
        std::cout << "  +----------------------------------+\n";
        std::cout << Color::RESET;
        Utils::sleep(2500);
    }

public:
    GameSession(BaseUser* user1, BaseUser* user2, char m1 = 'X', char m2 = 'O')
        : p1(user1), p2(user2),
          player1(dynamic_cast<Player*>(user1)),
          player2(dynamic_cast<Player*>(user2)),
          markerP1(m1), markerP2(m2),
          currentTurn(1), result(GameResult::ONGOING)
    {
        board = new Board();
    }

    ~GameSession() {
        delete board;
    }

    void play(std::vector<Player*>& allPlayers) {
        displayMatchup(*p1, *p2);
        Utils::sleep(1200);

        for (int moves = 0; moves < 9; moves++) {
            Player*     current = (currentTurn == 1) ? player1 : player2;
            char        marker  = (currentTurn == 1) ? markerP1 : markerP2;
            std::string color   = (currentTurn == 1) ? Color::RED : Color::BLUE;

            printGameHeader();
            std::cout << *board;

            // Input loop with validation
            while (true) {
                std::cout << "  " << color << Color::BOLD
                          << current->getUsername()
                          << Color::RESET << " (" << color << marker << Color::RESET
                          << "), enter slot [1-9]: ";
                int slot;
                if (!(std::cin >> slot)) {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << Color::RED << "  Invalid input.\n" << Color::RESET;
                    continue;
                }
                std::cin.ignore();
                if (slot < 1 || slot > 9) {
                    std::cout << Color::RED << "  Please enter 1-9.\n" << Color::RESET;
                    continue;
                }
                if (!board->placeMarker(slot, marker)) {
                    std::cout << Color::RED << "  Slot already taken! Try another.\n" << Color::RESET;
                    continue;
                }
                break;
            }

            char winner = board->checkWinner();
            if (winner != '\0') {
                printGameHeader();
                std::cout << *board;
                Utils::sleep(600);

                if (winner == 'T') {
                    // Tie
                    std::cout << "\n";
                    Utils::printLine('~', 40);
                    std::cout << Color::YELLOW << Color::BOLD
                              << "  It's a TIE! Well played, both!\n"
                              << Color::RESET;
                    Utils::printLine('~', 40);
                    Utils::beepTie();
                    result = GameResult::TIE;
                    if (player1) player1->recordTie();
                    if (player2) player2->recordTie();
                    recordToHistory("TIE");
                } else {
                    // winner == 'X' or 'O' — compare against actual markers
                    bool p1won      = (winner == markerP1);
                    Player*     winP   = p1won ? player1 : player2;
                    Player*     loseP  = p1won ? player2 : player1;
                    std::string wColor = p1won ? Color::RED : Color::BLUE;

                    std::cout << "\n";
                    Utils::printLine('*', 40);
                    std::cout << wColor << Color::BOLD
                              << "  >> " << winP->getUsername() << " WINS! <<\n"
                              << Color::RESET;
                    std::cout << Color::DIM
                              << "  " << loseP->getUsername() << " better luck next time.\n"
                              << Color::RESET;
                    Utils::printLine('*', 40);
                    Utils::beepWin();

                    result = p1won ? GameResult::PLAYER1_WIN : GameResult::PLAYER2_WIN;
                    winP->recordWin();
                    loseP->recordLoss();
                    recordToHistory(winP->getUsername() + " won");
                    showCongratsIfNeeded(winP);
                }

                FileManager::savePlayers(allPlayers);
                std::cout << Color::DIM << "\n  Press Enter to continue..." << Color::RESET;
                std::cin.get();
                Utils::sleep(400);
                return;
            }

            currentTurn = (currentTurn == 1) ? 2 : 1;
        }
    }

    void recordToHistory(const std::string& outcome) const {
        std::string ts   = getCurrentTimestamp();
        std::string rec1 = ts + " | vs " + player2->getUsername() + " | " + outcome;
        std::string rec2 = ts + " | vs " + player1->getUsername() + " | " + outcome;
        if (player1) { player1->addGameRecord(rec1); FileManager::appendGameRecord(player1->getUsername(), rec1); }
        if (player2) { player2->addGameRecord(rec2); FileManager::appendGameRecord(player2->getUsername(), rec2); }
    }

    GameResult getResult() const { return result; }
};

#endif
