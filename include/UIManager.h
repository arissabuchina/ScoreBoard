#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <Adafruit_RA8875.h>
#include "Game.h"
#include "GameFactory.h"
#include "Communication.h"
#include "Dartboard.h"
#include <cmath>
#include "Scoring.h"

enum UIState {
  HOME,
  SELECT_GAME,
  SELECT_PLAYERS,
  PLAYING
};

struct Button {
  int x, y, w, h;
  const char* label;
};


class UIManager {
public:
  UIManager(Adafruit_RA8875 &display, Game &gameRef, Communication &commRef);

  void begin();
  void update();
  void handleTouch(uint16_t x, uint16_t y);

  uint16_t mapTouchX(uint16_t rawX);

  uint16_t mapTouchY(uint16_t rawY);



  float play_board_cx = 0.0f;
  float play_board_cy = 0.0f;
  float play_px_per_mm = 1.0f;
  float play_left_w = 0.0f;   // width of left pane where board sits

  // For fullscreen calibration board (if needed)
  float calib_board_cx = 0.0f;
  float calib_board_cy = 0.0f;
  float calib_px_per_mm = 1.0f;


  private:
  Adafruit_RA8875 &tft;
  Game &game;
  Communication &comm;
  UIState currentState;

  Button startButton = {250, 240, 300, 80, "Start Game"};

  void drawHomeScreen();
  void handleHomeTouch();

  bool readTouch(uint16_t &x, uint16_t &y);
  bool buttonPressed(const Button &btn, uint16_t x, uint16_t y);
  void drawButton(const Button &btn);

  void drawGameModeSelectScreen();

  void drawPlayerCountScreen();
  void drawPlayingScreen();
  void redrawScorePanel();
  void waitForTouchRelease();
  void flushTouchBuffer();
};

#endif
