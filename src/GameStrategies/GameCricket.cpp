#include "GameStrategies/GameCricket.h"
#include <Arduino.h>

// Initialize each player's Cricket stats
void GameCricket::initializePlayers(std::vector<Player>& players) {
    for (auto& p : players) {
        p.cricketHits = {{15,0},{16,0},{17,0},{18,0},{19,0},{20,0},{25,0}};
        p.score = 0;
        p.dartsThrown = 0;
    }
}

// Main throw logic
/*std::string GameCricket::processThrow(std::pair<int, int> location, Player& player) {
    int target = getTargetFromLocation(location);
    player.dartsThrown++;

    if (target == -1)
        return player.name + " hit an invalid section.";

    // If target isn’t used in Cricket
    if (player.cricketHits.find(target) == player.cricketHits.end())
        return player.name + " hit " + std::to_string(target) + " (not used in Cricket).";

    Player* opponent = getOpponent(player);

    //sanity check
    if (!opponent) {
        Serial.println("[DEBUG] Opponent is null!");
    } else {
        Serial.printf("[DEBUG] Opponent found: %s\n", opponent->name.c_str());
    }

    int& playerHits = player.cricketHits[target];
    int opponentHits = opponent ? opponent->cricketHits[target] : 3; // assume closed if no opponent

    std::string result = player.name + " hit " + std::to_string(target) + ".";

    // Not closed yet
    if (playerHits < 3) {
        playerHits++;
        result += " (" + std::to_string(playerHits) + "/3)";
    }
    // Already closed, can only score if opponent hasn't
    else if (opponentHits < 3) {
        player.score += target;
        result += " Scored " + std::to_string(target) + " points!";
    } else {
        result += " No score (both closed).";
    }

    return result;
}*/ 

std::string GameCricket::processThrow(std::pair<int, int> location, Player& player) {
    int target = getTargetFromLocation(location);
    player.dartsThrown++;

    if (target == -1)
        return player.name + " hit an invalid section.";

    if (player.cricketHits.find(target) == player.cricketHits.end())
        return player.name + " hit " + std::to_string(target) + " (not used in Cricket).";

    Player* opponent = getOpponent(player);
    int& playerHits = player.cricketHits[target];
    int opponentHits = opponent ? opponent->cricketHits[target] : 3;

    std::string result = player.name + " hit " + std::to_string(target) + ". ";

    if (playerHits < 3) {
        playerHits++;
        result += " (" + std::to_string(playerHits) + "/3)";
    } 
    else if (opponentHits < 3) {
        player.score += target;
        result += " Scored " + std::to_string(target) + " points!";
    } 
    else {
        result += " No score (both closed).";
    }

    // Check if game is over after this throw
    if (isGameOver(*playersRef)) {
        Player* winner = nullptr;
        // find player with all numbers closed and highest score
        for (auto& p : *playersRef) {
            bool allClosed = true;
            for (const auto& [num, hits] : p.cricketHits) {
                if (hits < 3) { allClosed = false; break; }
            }
            if (allClosed) {
                bool hasHighest = true;
                for (auto& opp : *playersRef) {
                    if (opp.name != p.name && opp.score > p.score) hasHighest = false;
                }
                if (hasHighest) winner = &p;
            }
        }
        if (winner) {
            result += " 🎉 " + winner->name + " WINS!";
        }
    }

    return result;
}


// Check if any player has closed all numbers and has the highest score
bool GameCricket::isGameOver(const std::vector<Player>& players) {
    for (const auto& p : players) {
        bool allClosed = true;
        for (const auto& [num, hits] : p.cricketHits) {
            if (hits < 3) {
                allClosed = false;
                break;
            }
        }

        if (allClosed) {
            // verify this player also has highest score
            bool hasHighest = true;
            for (const auto& opp : players) {
                if (opp.name != p.name && opp.score > p.score)
                    hasHighest = false;
            }
            if (hasHighest)
                return true;
        }
    }
    return false;
}

// Map board position → target number
int GameCricket::getTargetFromLocation(std::pair<int, int> location) {
    // ⚠️ Placeholder mapping — adapt to your board/sensor
    if (location == std::pair<int,int>(1,1)) return 20;
    if (location == std::pair<int,int>(1,2)) return 19;
    if (location == std::pair<int,int>(1,3)) return 18;
    if (location == std::pair<int,int>(2,1)) return 17;
    if (location == std::pair<int,int>(2,2)) return 16;
    if (location == std::pair<int,int>(2,3)) return 15;
    if (location == std::pair<int,int>(3,3)) return 25; // bullseye
    return -1;
}

// Simple lookup for 2-player setups
Player* GameCricket::getOpponent(Player& current) {
    if (!playersRef) return nullptr;
    for (auto& p : *playersRef) {
        if (&p != &current)
            return &p;
    }
    return nullptr;
}

void GameCricket::setPlayersRef(std::vector<Player>& players) {
    playersRef = &players;
}
