#pragma once
#include "GameStrategy.h"

class GameAroundTheWorld : public GameStrategy {
public:
    void initializePlayers(std::vector<Player>& players) override;
    std::string processThrow(int points, Player& player) ;
    bool isGameOver(const std::vector<Player>& players) override;

private:
    int getNumberFromLocation(std::pair<int, int> location);
};
