#include "GameStrategies/Game301.h"
#include <string>

void Game301::initializePlayers(std::vector<Player>& players) {
    for (auto& p : players) {
        p.score = 301;
        p.dartsThrown = 0;
    }
}

std::string Game301::processThrow(int points, Player& player) {
    int newScore = player.score - points;
    std::string result = player.name + " hit " + std::to_string(points) + " points. ";

    if (newScore < 0) 
    {
        result += " Bust! Score stays at " + std::to_string(player.score) + "Turn over.";
        //game.nextPlayer();
    } else if (newScore == 0) {
        player.score = 0;
        result += player.name + " WINS!";
    } else {
        player.score = newScore;
        result += " Remaining: " + std::to_string(player.score);
    }

    player.dartsThrown++;
    return result;
}

bool Game301::isGameOver(const std::vector<Player>& players) {
    for (const auto& p : players) {
        if (p.score == 0) return true;
    }
    return false;
}


