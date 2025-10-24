#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include <vector>
#include <string>

struct Player {
    std::string name;
    int score;
    int dartsThrown;

    // Default constructor
    Player() : name(""), score(501), dartsThrown(0) {}

    // Constructor that accepts a name
    Player(const std::string &playerName)
        : name(playerName), score(501), dartsThrown(0) {}
};

class Game {
private:
    std::vector<Player> players;
    int currentPlayerIndex;
    bool gameOver;

    int getScoreFromLocation(std::pair<int, int> location);
    void nextPlayer();

public:
    Game();

    void addPlayer(std::string name);
    void reset();

    std::string processLocation(std::pair<int, int> location);

    std::string getCurrentPlayerName();
    int getCurrentPlayerScore();
    std::vector<Player> getAllPlayers() const;

    bool isGameOver() const;
};

#endif // GAME_H
