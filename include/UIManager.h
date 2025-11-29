#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <Adafruit_RA8875.h>
#include "Game.h"
#include "GameFactory.h"
#include "Communication.h"
#include "Dartboard.h"
#include <cmath>
#include "Scoring.h"

#define VRX_PIN 11
#define VRY_PIN 13

enum UIState {
  HOME,
  SELECT_GAME,
  SELECT_PLAYERS,
  PLAYING,
  CALIBRATION_SETUP,
  CALIBRATION
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
  int calIndex = 0;
  int NUM_CAL_POINTS = 5;

  private:
  Adafruit_RA8875 &tft;
  Game &game;
  Communication &comm;
  UIState currentState;
  Button startButton = {250, 240, 280, 80, "Start Game"};

  void drawHomeScreen();
  void handleHomeTouch();
  bool readTouch(uint16_t &x, uint16_t &y);
  bool buttonPressed(const Button &btn, uint16_t x, uint16_t y);
  void drawButton(const Button &btn);
  void drawGameModeSelectScreen();
  void drawPlayerCountScreen();
  void drawPlayingScreen();
  void redrawScorePanel();
  void drawCalibrationSetUpScreen();
  void drawCalibrationScreen();
  void waitForTouchRelease();
  void flushTouchBuffer();
  void updateCalibration();
  void updateJoystick();
  void updateCursor();
  void drawCursor();

  float joyX = 0, joyY = 0;      // normalized -1.0 to +1.0
  int cursorX = 400;             // start centered
  int cursorY = 240;
  int prevCursorX = -1;
  int prevCursorY = -1;

  // Patch buffer to store background pixels under cursor
  static const int PATCH_SIZE = 40;  // 40x40 area under cursor
  uint16_t cursorPatch[PATCH_SIZE * PATCH_SIZE];
  void saveCursorPatch(int x, int y);
  void restoreCursorPatch();
  uint16_t getDartboardPixelColor(int px, int py, int cx, int cy, float px_per_mm);

  ScoreResult sr;

  unsigned long elapsedTime = 0;
};

#endif
