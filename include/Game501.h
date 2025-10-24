#pragma once
#include "GameStrategy.h"

class Game501 : public GameStrategy {
public:
    void initializePlayers(std::vector<Player>& players) override;
    std::string processThrow(std::pair<int, int> location, Player& player) override;
    bool isGameOver(const std::vector<Player>& players) override;

private:
    int getScoreFromLocation(std::pair<int, int> location);
};
