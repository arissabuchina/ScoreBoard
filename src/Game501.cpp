#include "Game501.h"
#include <string>

void Game501::initializePlayers(std::vector<Player>& players) {
    for (auto& p : players) {
        p.score = 501;
        p.dartsThrown = 0;
    }
}

std::string Game501::processThrow(std::pair<int, int> location, Player& player) {
    int points = getScoreFromLocation(location);
    int newScore = player.score - points;
    std::string result = player.name + " hit " + std::to_string(points) + " points.";

    if (newScore < 0) {
        result += " Bust! Score stays at " + std::to_string(player.score);
    } else if (newScore == 0) {
        player.score = 0;
        result += " 🎯 " + player.name + " wins!";
    } else {
        player.score = newScore;
        result += " Remaining: " + std::to_string(player.score);
    }

    player.dartsThrown++;
    return result;
}

bool Game501::isGameOver(const std::vector<Player>& players) {
    for (const auto& p : players) {
        if (p.score == 0) return true;
    }
    return false;
}

int Game501::getScoreFromLocation(std::pair<int, int> location) {
    if (location == std::pair<int, int>(1, 1)) return 21;
    if (location == std::pair<int, int>(2, 2)) return 40;
    if (location == std::pair<int, int>(3, 3)) return 60;
    if (location == std::pair<int, int>(4, 4)) return 20;
    return 0;
}
