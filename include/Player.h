#pragma once
#include <string>
#include <map>

struct Player {
    std::string name;
    int score = 0;
    int dartsThrown = 0;

    Player(std::string n) : name(n) {}
    std::map<int, int> cricketHits; // For Cricket: tracks how many times 15–20 and 25 are hit
};
