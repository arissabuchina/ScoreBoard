#pragma once
#include "GameStrategy.h"

class Game301 : public GameStrategy {
public:
    void initializePlayers(std::vector<Player>& players) override;
    std::string processThrow(int points, Player& player) ;
    bool isGameOver(const std::vector<Player>& players) override;

private:
    int getScoreFromLocation(std::pair<int, int> location);
};
