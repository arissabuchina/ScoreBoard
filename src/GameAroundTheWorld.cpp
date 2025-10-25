#include "GameAroundTheWorld.h"

void GameAroundTheWorld::initializePlayers(std::vector<Player>& players) {
    for (auto& p : players) {
        p.score = 0;          // Not used, but can display progress
        p.dartsThrown = 0;
        // We can store current target in score temporarily or add a new field
        // but for simplicity we’ll reuse score as the “target number - 1”.
        p.score = 1;          // Start at 1
    }
}

std::string GameAroundTheWorld::processThrow(std::pair<int, int> location, Player& player) {
    int hitNumber = getNumberFromLocation(location);
    std::string result = player.name + " aimed for " + std::to_string(player.score) + " and hit " + std::to_string(hitNumber) + ".";

    if (hitNumber == player.score) {
        player.score++;
        result += " 🎯 Nice! Next target: " + std::to_string(player.score <= 20 ? player.score : 20);
    } else {
        result += " Miss!";
    }

    player.dartsThrown++;
    return result;
}

bool GameAroundTheWorld::isGameOver(const std::vector<Player>& players) {
    for (const auto& p : players) {
        if (p.score > 20) {  // hit 20 and advanced beyond
            return true;
        }
    }
    return false;
}

// Dummy mapping for now — replace with real coordinate-to-number logic later
int GameAroundTheWorld::getNumberFromLocation(std::pair<int, int> location) {
    // For simulation: map location.x to a board number 1–20
    // Example: (1,1)->1, (2,2)->2, etc.
    int mapping = location.first; 
    return (mapping % 20) == 0 ? 20 : (mapping % 20);
}
