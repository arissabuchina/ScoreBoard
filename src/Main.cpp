/*#include <Arduino.h>
#include "Game.h"
#include "Communication.h"
#include "GameFactory.h"
#include "Calibration.h"

Game game;
Communication comm;
Calibration calib;

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
  


  
  String calibInput = getUserInput("Run calibration? (y/n): ");
  std::string choice = std::string(calibInput.c_str());

  if (choice == "y"|| choice == "Y") {
    // Demo true vs estimated points
    std::vector<std::pair<float, float>> truePts = {{1,1}, {2,2}, {3,3}};
    std::vector<std::pair<float, float>> estPts  = {{1.1,0.9}, {2.1,1.9}, {3.05,2.95}};

    calib.setLocations(truePts, estPts);
    calib.runDemo();
  }




  // Ask for game type
  std::shared_ptr<GameStrategy> strategy = nullptr;
  while (!strategy) {
    String input = getUserInput("\nEnter game type (501, 301, Cricket, AroundTheWorld): ");
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
  */

#include <Arduino.h>
#include <Adafruit_RA8875.h>
#include "UIManager.h"
#include "Game.h"
#include "Communication.h"

//driver pins
#define RA8875_CS   17
#define RA8875_RST  16
#define RA8875_INT 7

//joystick pins 
#define VRX_PIN 11
#define VRY_PIN 13

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RST);
Game game;
Communication comm;
UIManager ui(tft, game, comm);

void setup() {
  Serial.begin(115200);

  //sck, miso, mosi
  SPI.begin(3, 8, 18);

  if (!tft.begin(RA8875_800x480)) {
    Serial.println("Display not found");
    while (1);
  }

  tft.displayOn(true);
  tft.GPIOX(true);
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024);
  tft.PWM1out(255);

  //enable touch inside the RA8875 chip
  tft.touchEnable(true);

  //Setup the INT pin
  pinMode(RA8875_INT, INPUT);

  ui.begin();
  comm.begin();
  delay(5000);

}

void loop() 
{
  ui.update();
  //comm.update(); // handles dart detection

  delay(10);
}

