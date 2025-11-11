#include "UIManager.h"

UIManager::UIManager(Adafruit_RA8875 &display, Game &gameRef, Communication &commRef)
: tft(display), game(gameRef), comm(commRef) {
  currentState = HOME;
}

void UIManager::begin() {
  tft.fillScreen(RA8875_BLACK);
  drawHomeScreen();
  tft.touchEnable(true); // <<< REQUIRED

}

void UIManager::update() {
    if (tft.touched()) {
        uint16_t rawX, rawY;
        tft.touchRead(&rawX, &rawY);

        uint16_t x = mapTouchX(rawX);
        uint16_t y = mapTouchY(rawY);

        handleTouch(x, y);
    }
}


void UIManager::drawHomeScreen() {
  tft.fillScreen(RA8875_BLACK);

  tft.textMode();
  tft.textColor(RA8875_WHITE, RA8875_BLACK);
  tft.textEnlarge(2);
  tft.textSetCursor(160, 120);
  tft.textWrite("Auto-Scoring Dartboard");

  drawButton(startButton);
}

void UIManager::handleHomeTouch() {
  uint16_t x, y;
  if (!readTouch(x, y)) return;
  if (buttonPressed(startButton, x, y)) {
    currentState = SELECT_GAME;  // we will implement this next
    tft.fillScreen(RA8875_BLUE);
    tft.textSetCursor(200, 200);
    tft.textColor(RA8875_WHITE, RA8875_BLUE);
    tft.textEnlarge(1);
    tft.textWrite("Game Select Screen (coming next)");
  }
}

bool UIManager::readTouch(uint16_t &x, uint16_t &y) {
  if (!tft.touched()) return false;
  tft.touchRead(&x, &y);
  return true;
}

bool UIManager::buttonPressed(const Button &btn, uint16_t x, uint16_t y) {
  return (x >= btn.x && x <= btn.x + btn.w &&
          y >= btn.y && y <= btn.y + btn.h);
}

void UIManager::drawButton(const Button &btn) {
  tft.fillRect(btn.x, btn.y, btn.w, btn.h, RA8875_WHITE);
  tft.drawRect(btn.x, btn.y, btn.w, btn.h, RA8875_BLACK);

  tft.textMode();
  tft.textColor(RA8875_BLACK, RA8875_WHITE);
  tft.textEnlarge(1);
  tft.textSetCursor(btn.x + 40, btn.y + 25);
  tft.textWrite(btn.label);
}



/*void UIManager::handleTouch(uint16_t x, uint16_t y) {

    // Home Screen Start Button
    if (currentState == HOME) {
        if (x >= 100 && x <= 300 && y >= 100 && y <= 180) {
            drawGameModeSelectScreen();
            currentState = SELECT_GAME;
        }
    }

    if (currentState == SELECT_GAME) {
        // We will add options here later
    }

    // Additional screens will be added later...
}*/

void UIManager::handleTouch(uint16_t x, uint16_t y) {
    Serial.print("Mapped Touch: ");
    Serial.print(x);
    Serial.print(", ");
    Serial.println(y);

    if (currentState == HOME) {
        if (x >= 40 && x <= 300 && y >= 40 && y <= 180) {
            Serial.println("Start button pressed!");
            drawGameModeSelectScreen();
            currentState = SELECT_GAME;
            return;
        }
    }

    if (currentState == SELECT_GAME) {
        // We will add options here later
        drawGameModeSelectScreen();
    }
}




uint16_t UIManager::mapTouchX(uint16_t rawX) {
    return map(rawX, 200, 3900, 0, 800);  // adjust after calibration
}

uint16_t UIManager::mapTouchY(uint16_t rawY) {
    return map(rawY, 200, 3800, 0, 480);  // adjust after calibration
}


void UIManager::drawGameModeSelectScreen() {
    tft.fillScreen(RA8875_BLUE);
    tft.textMode();
    tft.textSetCursor(100, 100);
    tft.textColor(RA8875_WHITE, RA8875_BLUE);
    tft.textEnlarge(1);
    tft.textWrite("Select Game Mode (placeholder)");
}


