#pragma once
#include <string>

struct Player {
    std::string name;
    int score = 0;
    int dartsThrown = 0;

    Player(std::string n) : name(n) {}
};
