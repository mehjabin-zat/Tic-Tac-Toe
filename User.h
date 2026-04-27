#ifndef USER_H
#define USER_H

#include <string>
#include <iostream>
#include "Utils.h"
#include "History.h"

// ─── Base User ────────────────────────────────────────────────────────────────
class BaseUser {
protected:
    std::string username;
    std::string passwordHash; // simple stored password (plain for this project scope)
    bool caseSensitiveLogin;

public:
    BaseUser(const std::string& uname, const std::string& pass, bool caseSensitive);
    virtual ~BaseUser();

    virtual void displayDashboard() const = 0;  // pure virtual → polymorphism
    virtual std::string getRole() const = 0;

    bool checkLogin(const std::string& uname, const std::string& pass) const;

    std::string getUsername() const;
    std::string getDisplayName() const;

    // Operator == : compare usernames (case-insensitive for Player, sensitive for Admin)
    bool operator==(const BaseUser& other) const;

    // Friend function: show matchup info (needs private access to both users)
    friend void displayMatchup(const BaseUser& a, const BaseUser& b);
};

// ─── Player ──────────────────────────────────────────────────────────────────
class Player : public BaseUser {
private:
    int wins;
    int losses;
    int ties;
    int totalPoints;       // 5 pts per win
    char preferredMarker;
    History<std::string> gameHistory;

public:
    Player(const std::string& uname, const std::string& pass);
    ~Player();

    void displayDashboard() const override;
    std::string getRole() const override { return "Player"; }

    // Game result recording
    void recordWin();
    void recordLoss();
    void recordTie();
    void addGameRecord(const std::string& record);

    // Getters
    int getWins()        const { return wins; }
    int getLosses()      const { return losses; }
    int getTies()        const { return ties; }
    int getTotalPoints() const { return totalPoints; }
    const History<std::string>& getHistory() const { return gameHistory; }

    // Setters (used when loading from file)
    void setWins(int w)        { wins = w; }
    void setLosses(int l)      { losses = l; }
    void setTies(int t)        { ties = t; }
    void setTotalPoints(int p) { totalPoints = p; }

    bool hasEarnedCongrats() const { return wins > 0 && wins % 5 == 0; }

    // Operator overload: compare players by points (for leaderboard sorting)
    bool operator>(const Player& other) const;
    bool operator<(const Player& other) const;

    // Stream output
    friend std::ostream& operator<<(std::ostream& os, const Player& p);
};

// ─── Admin ────────────────────────────────────────────────────────────────────
class Admin : public BaseUser {
private:
    std::string adminId;

public:
    Admin(const std::string& uname, const std::string& pass, const std::string& id);
    ~Admin();

    void displayDashboard() const override;
    std::string getRole() const override { return "Admin"; }
    std::string getAdminId() const { return adminId; }
};

// ─── Friend Function Declaration ─────────────────────────────────────────────
void displayMatchup(const BaseUser& a, const BaseUser& b);

#endif
