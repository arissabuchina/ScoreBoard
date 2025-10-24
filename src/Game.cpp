#include "Game.h"

Game::Game() {
    currentPlayerIndex = 0;
    gameOver = false;
}

void Game::addPlayer(std::string name) {
    players.push_back(Player(name));
}

void Game::reset() {
    /*for (auto &p : players) {
        p.score = 501;
        p.dartsThrown = 0;
    }*/
    players.clear();
    currentPlayerIndex = 0;
    gameOver = false;
}

std::string Game::processLocation(std::pair<int, int> location) {
    if (gameOver || players.empty()) return "Game Over!";

    Player &p = players[currentPlayerIndex];
    int points = getScoreFromLocation(location);
    int newScore = p.score - points;
    std::string result = p.name + " hit " + std::to_string(points) + " points.";

    if (newScore < 0) {
        result += " Bust! Score stays at " + std::to_string(p.score);
    } else if (newScore == 0) {
        p.score = newScore;
        result += " 🎯 " + p.name + " wins!";
        gameOver = true;
        return result;
    } else {
        p.score = newScore;
        result += " Remaining: " + std::to_string(p.score);
    }

    // End of turn: move to next player
    p.dartsThrown++;
    if (p.dartsThrown >= 3 || newScore <= 0) {
        p.dartsThrown = 0;
        nextPlayer();
    }

    return result;
}

void Game::nextPlayer() {
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
}

std::string Game::getCurrentPlayerName() {
    if (players.empty()) return "";
    return players[currentPlayerIndex].name;
}

int Game::getCurrentPlayerScore() {
    if (players.empty()) return 0;
    return players[currentPlayerIndex].score;
}

std::vector<Player> Game::getAllPlayers() const {
    return players;
}

bool Game::isGameOver() const {
    return gameOver;
}

// Basic scoring map (replace with actual dartboard logic later)
int Game::getScoreFromLocation(std::pair<int, int> location) {
    if (location == std::pair<int, int>(1, 1)) return 21;
    if (location == std::pair<int, int>(2, 2)) return 40;
    if (location == std::pair<int, int>(3, 3)) return 60;
    if (location == std::pair<int, int>(4, 4)) return 20;
    return 0;
}
