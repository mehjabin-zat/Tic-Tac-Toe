#include "User.h"
#include <iomanip>

// ═══════════════════════════════════════════════════════
//  BaseUser
// ═══════════════════════════════════════════════════════

BaseUser::BaseUser(const std::string& uname, const std::string& pass, bool caseSensitive)
    : username(uname), passwordHash(pass), caseSensitiveLogin(caseSensitive) {}

BaseUser::~BaseUser() {}

bool BaseUser::checkLogin(const std::string& uname, const std::string& pass) const {
    std::string storedName = caseSensitiveLogin ? username : Utils::toLower(username);
    std::string inputName  = caseSensitiveLogin ? uname    : Utils::toLower(uname);
    return (storedName == inputName) && (passwordHash == pass);
}

std::string BaseUser::getUsername() const { return username; }
std::string BaseUser::getDisplayName() const { return username; }

bool BaseUser::operator==(const BaseUser& other) const {
    if (caseSensitiveLogin)
        return username == other.username;
    return Utils::toLower(username) == Utils::toLower(other.username);
}

// Friend function — accesses private members of both users
void displayMatchup(const BaseUser& a, const BaseUser& b) {
    Utils::printLine('~', 40);
    std::cout << Color::BOLD
              << "  " << Color::RED    << a.username
              << Color::WHITE << "  vs  "
              << Color::BLUE  << b.username
              << Color::RESET << "\n";
    Utils::printLine('~', 40);
}

// ═══════════════════════════════════════════════════════
//  Player
// ═══════════════════════════════════════════════════════

Player::Player(const std::string& uname, const std::string& pass)
    : BaseUser(uname, pass, false), // case-insensitive login
      wins(0), losses(0), ties(0), totalPoints(0),
      preferredMarker('X'), gameHistory(50) {}

Player::~Player() {}

void Player::recordWin() {
    wins++;
    totalPoints += 5;
}

void Player::recordLoss() {
    losses++;
    if (totalPoints >= 2)
        totalPoints -= 2;
    else
        totalPoints = 0;
}

void Player::recordTie() {
    ties++;
    totalPoints += 2;
}

void Player::addGameRecord(const std::string& record) {
    gameHistory.add(record);
}

void Player::displayDashboard() const {
    std::cout << Color::BOLD << Color::CYAN
              << "\n  Player: " << username << Color::RESET << "\n";
    Utils::printLine('-', 36);
    std::cout << "  Wins     : " << Color::GREEN  << wins        << Color::RESET << "\n";
    std::cout << "  Losses   : " << Color::RED    << losses      << Color::RESET << "\n";
    std::cout << "  Ties     : " << Color::YELLOW << ties        << Color::RESET << "\n";
    std::cout << "  Points   : " << Color::BOLD   << totalPoints << Color::RESET << " pts\n";
    int played = wins + losses + ties;
    if (played > 0) {
        double wr = (wins * 100.0) / played;
        std::cout << "  Win Rate : " << std::fixed << std::setprecision(1) << wr << "%\n";
    }
    std::cout << Color::DIM << "  (Win +5 | Tie +2 | Loss -2)\n" << Color::RESET;
}

bool Player::operator>(const Player& other) const {
    return totalPoints > other.totalPoints;
}
bool Player::operator<(const Player& other) const {
    return totalPoints < other.totalPoints;
}

std::ostream& operator<<(std::ostream& os, const Player& p) {
    os << std::left << std::setw(16) << p.username
       << " | W:" << std::setw(3) << p.wins
       << " L:" << std::setw(3) << p.losses
       << " T:" << std::setw(3) << p.ties
       << " | " << p.totalPoints << " pts";
    return os;
}

// ═══════════════════════════════════════════════════════
//  Admin
// ═══════════════════════════════════════════════════════

Admin::Admin(const std::string& uname, const std::string& pass, const std::string& id)
    : BaseUser(uname, pass, true), // case-sensitive login
      adminId(id) {}

Admin::~Admin() {}

void Admin::displayDashboard() const {
    std::cout << Color::BOLD << Color::MAGENTA
              << "\n  [ADMIN] " << username
              << Color::DIM << "  (ID: " << adminId << ")"
              << Color::RESET << "\n";
    Utils::printLine('-', 36);
    std::cout << "  Access : Full scorecard & history\n";
}
