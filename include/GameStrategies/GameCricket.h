#pragma once
#include "GameStrategy.h"
#include <map>
#include <string>
#include <vector>

class GameCricket : public GameStrategy {
public:
    void initializePlayers(std::vector<Player>& players) override;
    std::string processThrow(int points, Player& player) ;
    bool isGameOver(const std::vector<Player>& players) override;

    // Optional: lets this strategy access all players to check opponent states
    void setPlayersRef(std::vector<Player>& players) override;

private:
    int getTargetFromLocation(std::pair<int, int> location);
    Player* getOpponent(Player& current);

    std::vector<Player>* playersRef = nullptr; // stored reference for opponent lookup
};
