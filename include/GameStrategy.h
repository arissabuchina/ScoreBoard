#pragma once
#include <string>
#include <utility>
#include <vector>
#include "Player.h"

class GameStrategy {
public:
    virtual ~GameStrategy() = default;
    virtual void initializePlayers(std::vector<Player>& players) = 0;
    virtual std::string processThrow(int points, Player& player) = 0;
    virtual bool isGameOver(const std::vector<Player>& players) = 0;

    virtual void setPlayersRef(std::vector<Player>& players) { /* default: do nothing */ }

    bool isBust = false;
};
