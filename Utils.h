#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>

#ifdef _WIN32
    #include <windows.h>
#endif

// ─── ANSI Color Codes ───────────────────────────────────────────────────────
namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[91m";   // bright red
    const std::string BLUE    = "\033[94m";   // bright blue
    const std::string YELLOW  = "\033[93m";   // bright yellow
    const std::string GREEN   = "\033[92m";   // bright green
    const std::string CYAN    = "\033[96m";   // bright cyan
    const std::string MAGENTA = "\033[95m";   // bright magenta
    const std::string WHITE   = "\033[97m";   // bright white
    const std::string BOLD    = "\033[1m";
    const std::string DIM     = "\033[2m";
}

// ─── Utility Functions ───────────────────────────────────────────────────────
namespace Utils {

    inline void enableANSI() {
        #ifdef _WIN32
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD dwMode = 0;
            GetConsoleMode(hOut, &dwMode);
            SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        #endif
    }

    inline void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    inline void sleep(int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    inline void beepWin() { std::cout << "\a"; std::cout.flush(); }
    inline void beepTie() { std::cout << "\a"; sleep(200); std::cout << "\a"; std::cout.flush(); }

    inline std::string toLower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }

    inline std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end   = s.find_last_not_of(" \t\r\n");
        return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
    }

    inline void printLine(char c = '-', int len = 44) {
        std::cout << Color::DIM;
        for (int i = 0; i < len; i++) std::cout << c;
        std::cout << Color::RESET << "\n";
    }

    // Centered text inside a fixed-width box
    inline void printCentered(const std::string& text, int width = 44) {
        int padding = (width - (int)text.size()) / 2;
        if (padding < 0) padding = 0;
        std::cout << std::string(padding, ' ') << text << "\n";
    }

    inline void printHeader(const std::string& title) {
        clearScreen();
        std::cout << "\n";
        printLine('=', 44);
        printCentered(title, 44);
        printLine('=', 44);
        std::cout << "\n";
    }

    // Safe integer input
    inline int getInt(const std::string& prompt, int minVal, int maxVal) {
        int val;
        while (true) {
            std::cout << prompt;
            if (std::cin >> val && val >= minVal && val <= maxVal) {
                std::cin.ignore();
                return val;
            }
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << Color::RED << "  Invalid. Enter a number between "
                      << minVal << " and " << maxVal << ".\n" << Color::RESET;
        }
    }

    inline std::string getString(const std::string& prompt) {
        std::string s;
        std::cout << prompt;
        std::getline(std::cin, s);
        return trim(s);
    }
}

#endif
