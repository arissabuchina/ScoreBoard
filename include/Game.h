
#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include <vector>
#include <string>
#include <memory>
#include "Player.h"
#include "GameStrategy.h"


class Game {
public:
    Game();
    void addPlayer(std::string name);
    void reset();
    void setStrategy(std::shared_ptr<GameStrategy> strategy);
    void initialize();

    std::string processLocation(int points);
    std::string getCurrentPlayerName();
    int getCurrentPlayerScore();
    std::vector<Player> getAllPlayers() const;
    bool isGameOver() const;

    void startGame();
    int getCurrentPlayerIndex() const;

private:
    void nextPlayer();

    std::vector<Player> players;
    int currentPlayerIndex;
    bool gameOver;
    std::shared_ptr<GameStrategy> strategy;
};

#endif // GAME_H