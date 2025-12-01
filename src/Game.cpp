#include "Game.h"
#include "GameStrategies/GameCricket.h"

Game::Game() {
    currentPlayerIndex = 0;
    gameOver = false;
}

void Game::addPlayer(std::string name) {
    players.push_back(Player(name));
}

void Game::reset() {
    players.clear();
    currentPlayerIndex = 0;
    gameOver = false;
}

void Game::setStrategy(std::shared_ptr<GameStrategy> s) {
    strategy = s;
}

void Game::initialize(){
    if (strategy) {
        strategy->setPlayersRef(players);
        strategy->initializePlayers(players);
    }
}

std::string Game::processLocation(int points) {
    if (!strategy || gameOver || players.empty()) return "Game Over or strategy not set.";

    Player &p = players[currentPlayerIndex];
    std::string result = strategy->processThrow(points, p);

    //if bust players turn is over, moves to next player and score is not changed 
    if(strategy->isBust == true) 
    {
        p.dartsThrown = 0;
        nextPlayer();
        strategy->isBust = false;
        return result;
    }

    if (strategy->isGameOver(players)) {
        gameOver = true;
        return result;
    }

    if (p.dartsThrown >= 3) {
        p.dartsThrown = 0;
        nextPlayer();
    }

    return result;
}

void Game::nextPlayer() {
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
}

std::string Game::getCurrentPlayerName() {
    return players.empty() ? "" : players[currentPlayerIndex].name;
}

int Game::getCurrentPlayerScore() {
    return players.empty() ? 0 : players[currentPlayerIndex].score;
}

std::vector<Player> Game::getAllPlayers() const {
    return players;
}

bool Game::isGameOver() const {
    return gameOver;
}

int Game::getCurrentPlayerIndex() const 
{ 
    return currentPlayerIndex; 
}

