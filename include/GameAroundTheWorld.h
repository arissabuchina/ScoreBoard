#pragma once
#include "GameStrategy.h"

class GameAroundTheWorld : public GameStrategy {
public:
    void initializePlayers(std::vector<Player>& players) override;
    std::string processThrow(std::pair<int, int> location, Player& player) override;
    bool isGameOver(const std::vector<Player>& players) override;

private:
    int getNumberFromLocation(std::pair<int, int> location);
};
