#include <Arduino.h>
#include "Game.h"
#include "Communication.h"

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
  Serial.println("--------------------------------------");
  Serial.println("Setup your game below:\n");

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

  Serial.println("\nGame setup complete! Starting simulation...");
  Serial.println("--------------------------------------\n");

  // Connect Communication → Game
  comm.onNewLocation([](std::pair<int, int> location) {
    std::string result = game.processLocation(location);
    Serial.println(result.c_str()); // convert std::string → const char*

    // Show all player scores after each round
    Serial.println("\nCurrent Scores:");
    for (auto &p : game.getAllPlayers()) {
      Serial.printf("  %s: %d\n", p.name.c_str(), p.score); // convert std::string → const char*
    }

    Serial.println("-------------------");

    // Stop simulation if someone won
    if (game.isGameOver()) {
      Serial.println("GAME OVER! Resetting the board to play again.");
      delay(2000);
      game.reset();
      setup(); // Call setup again to reinitialize after game over
    }
  });

  // Start the simulation
  comm.startSimulation();
  
}

void loop() {
  comm.update();
}
