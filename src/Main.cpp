#include <Arduino.h>
#include "Game.h"
#include "Communication.h"
#include "GameFactory.h"

Game game;
Communication comm;

// Helper function to get user input from Serial
String getUserInput(const char* prompt) {
  Serial.print(prompt);
  while (!Serial.available()) {
    delay(10);
  }
  String input = Serial.readStringUntil('\n');
  input.trim();
  return input;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Welcome to the Auto-Scoring Dart Game!");
  Serial.println("Setup your game below:\n");
  Serial.println("--------------------------------------");
  

  // Ask for game type
  std::shared_ptr<GameStrategy> strategy = nullptr;
  while (!strategy) {
    String input = getUserInput("Enter game type (501, 301, Cricket, AroundTheWorld): ");
    std::string gameType = std::string(input.c_str());
    strategy = GameFactory::createGame(gameType);
    if (!strategy) {
      Serial.println("Invalid game type. Try again.");
    } else {
      game.setStrategy(strategy);
      Serial.printf("\nSelected game: %s\n\n", gameType.c_str());
    }
  }

  // Ask for number of players
  int numPlayers = 0;
  while (numPlayers < 1 || numPlayers > 4) {
    String input = getUserInput("Enter number of players (1–4): ");
    numPlayers = input.toInt();
    if (numPlayers < 1 || numPlayers > 4) {
      Serial.println("Invalid input. Please enter between 1 and 4 players.");
    }
  }

  // Add each player's name
  for (int i = 1; i <= numPlayers; i++) {
    String name = getUserInput(("\nEnter name for Player " + String(i) + ": ").c_str());
    std::string playerName = std::string(name.c_str());   // convert Arduino String → std::string
    game.addPlayer(playerName);
  }

  game.initialize();

  Serial.println("\nGame setup complete! Starting simulation...");
  Serial.println("--------------------------------------\n");

  comm.onNewLocation([](std::pair<int, int> location) {
    std::string result = game.processLocation(location);
    Serial.println(result.c_str());

    if (game.isGameOver()) {
      Serial.println("GAME OVER! Resetting the board to play again.");
      Serial.println("-------------------");
      delay(2000);
      game.reset();
      setup();  // Restart setup for a new game
    }

    Serial.println("\nCurrent Scores:");
    for (auto &p : game.getAllPlayers()) {
      Serial.printf("  %s: %d\n", p.name.c_str(), p.score);
    }

    Serial.println("-------------------");

    
  });

  comm.startSimulation();
}

void loop() {
  comm.update();
}
