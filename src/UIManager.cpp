#include "UIManager.h"

UIManager::UIManager(Adafruit_RA8875 &display, Game &gameRef, Communication &commRef)
: tft(display), game(gameRef), comm(commRef) {
  currentState = HOME;
}

void UIManager::begin() {
  tft.fillScreen(RA8875_BLACK);

  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // Enable TFT backlight control
  tft.PWM1out(255);

  drawHomeScreen();
  tft.touchEnable(true); // <<< REQUIRED

}



void UIManager::update() 
{
    if (tft.touched()) 
    {
        uint16_t rawX, rawY;
        tft.touchRead(&rawX, &rawY);

        // --- ADD THIS ---
        Serial.print("RAW Touch: ");
        Serial.print(rawX);
        Serial.print(", ");
        Serial.println(rawY);
        // ----------------

        uint16_t x = mapTouchX(rawX);
        uint16_t y = mapTouchY(rawY);

        handleTouch(x, y);
    }
}



void UIManager::drawHomeScreen() 
{
  tft.fillScreen(RA8875_BLACK);

  tft.textMode();
  tft.textColor(RA8875_WHITE, RA8875_BLACK);
  tft.textEnlarge(2);
  tft.textSetCursor(160, 120);
  tft.textWrite("Auto-Scoring Dartboard");

  drawButton(startButton);

}

void UIManager::handleHomeTouch() 
{
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

  Serial.print("Mapped Touch: ");
  Serial.print(x);
  Serial.print(", ");
  Serial.println(y);

  if (currentState == HOME) 
  {
    if (x >= 200 && x <= 450 && y >= 200 && y <= 350) 
    {
      Serial.println("Start button pressed!");
      currentState = SELECT_GAME;
      drawGameModeSelectScreen();
      waitForTouchRelease();
      return;
    }
  }

  if (currentState == SELECT_GAME) 
  {
        
    if (x >= 0 && x <= 300) 
    {
      if (y >= 0 && y <= 180) 
      {
        game.setStrategy(GameFactory::createGame("501"));
        Serial.println("501 selected");
      } 
      else if (y >= 200 && y <= 280) 
      {
        game.setStrategy(GameFactory::createGame("301"));
        Serial.println("301 selected");
      } 
      else if (y >= 300 && y <= 380) 
      {
        game.setStrategy(GameFactory::createGame("Cricket"));
        Serial.println("Cricket selected");
      } 
      else if (y >= 400 && y <= 480) 
      {
        game.setStrategy(GameFactory::createGame("AroundTheWorld"));
        Serial.println("Around The World selected");
      } 
      else 
      {
        return; // touch outside buttons
      }

      // After selection, move to player count screen
      currentState = SELECT_PLAYERS;
      drawPlayerCountScreen();
      waitForTouchRelease();

    }

  }

}*/

void UIManager::handleTouch(uint16_t x, uint16_t y) {
  Serial.print("Mapped Touch: ");
  Serial.print(x);
  Serial.print(", ");
  Serial.println(y);

  switch(currentState) {

    case HOME:
      if (x >= 200 && x <= 450 && y >= 200 && y <= 350) {
        Serial.println("Start button pressed!");
        
        currentState = SELECT_GAME;
        drawGameModeSelectScreen();
        waitForTouchRelease();
        
      }
      break;


    case SELECT_GAME:
      // Left side zone only (adjust later if needed)
      if (x >= 0 && x <= 400) {

        if (y >= 100 && y <= 160) {
          game.setStrategy(GameFactory::createGame("501"));
          Serial.println("501 selected");
        }
        else if (y >= 200 && y <= 260) {
          game.setStrategy(GameFactory::createGame("301"));
          Serial.println("301 selected");
        }
        else if (y >= 300 && y <= 360) {
          game.setStrategy(GameFactory::createGame("Cricket"));
          Serial.println("Cricket selected");
        }
        else if (y >= 400 && y <= 460) {
          game.setStrategy(GameFactory::createGame("AroundTheWorld"));
          Serial.println("Around the World selected");
        }
        else {
          return; // touched outside buttons
        }

        currentState = SELECT_PLAYERS;
        drawPlayerCountScreen();
        waitForTouchRelease();
        
      }
      break;


    case SELECT_PLAYERS:
      // Add handler logic here later
      
      break;
  }
}



uint16_t UIManager::mapTouchX(uint16_t rawX) {
    return map(rawX, 47, 981, 0, 800);  // adjust after calibration
}

uint16_t UIManager::mapTouchY(uint16_t rawY) {
    return map(rawY, 147, 920, 0, 480);  // adjust after calibration
}


void UIManager::drawGameModeSelectScreen() 
{
    
    tft.fillScreen(RA8875_GREEN);
    tft.textMode();
    tft.textColor(RA8875_WHITE, RA8875_GREEN);
    tft.textEnlarge(1);

    tft.textSetCursor(100, 100);
    tft.textWrite("501");

    tft.textSetCursor(100, 200);
    tft.textWrite("301");

    tft.textSetCursor(100, 300);
    tft.textWrite("Cricket");

    tft.textSetCursor(100, 400);
    tft.textWrite("Around The World");
}

void UIManager::drawPlayerCountScreen() {
    tft.fillScreen(RA8875_WHITE);
    tft.textMode();
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textEnlarge(1);

    tft.textSetCursor(100, 100);
    tft.textWrite("Select Number of Players:");

    tft.textSetCursor(100, 200);
    tft.textWrite("1 Player");

    tft.textSetCursor(100, 300);
    tft.textWrite("2 Players");

    tft.textSetCursor(100, 400);
    tft.textWrite("3 Players");

    tft.textSetCursor(100, 500);
    tft.textWrite("4 Players");
}


void UIManager::waitForTouchRelease() {
    delay(5000); 
}




