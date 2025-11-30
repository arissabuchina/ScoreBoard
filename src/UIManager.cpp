#include "UIManager.h"

constexpr int SCREEN_W = 800;
constexpr int SCREEN_H = 480;


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

        Serial.print("RAW Touch: ");
        Serial.print(rawX);
        Serial.print(", ");
        Serial.println(rawY);

        uint16_t x = mapTouchX(rawX);
        uint16_t y = mapTouchY(rawY);

        handleTouch(x, y);
    }

    if (currentState == CALIBRATION) {
        updateJoystick();    // reads joystick + updates joyX, joyY
        updateCursor();      // updates cursorX, cursorY based on joyX, joyY
        drawCursor();          // draws the cursor on screen
    }
  
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

void UIManager::handleTouch(uint16_t x, uint16_t y) {
  Serial.print("Mapped Touch: ");
  Serial.print(x);
  Serial.print(", ");
  Serial.println(y);

  switch(currentState) {
    case HOME:
    {
      if (x >= 200 && x <= 450 && y >= 200 && y <= 350) {
        Serial.println("Start button pressed!");
        
        currentState = SELECT_GAME;
        flushTouchBuffer();
        waitForTouchRelease();
        drawGameModeSelectScreen();
        
      }
      break;
    }  
    case SELECT_GAME:
    {
      // Left side zone only (adjust later if needed)
      if (x >= 0 && x <= 400) 
      {

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

        flushTouchBuffer();
        waitForTouchRelease();
        currentState = SELECT_PLAYERS;
        drawPlayerCountScreen();
        
      }
      break;
    }
    case SELECT_PLAYERS:
    {
      int numPlayers = 0;
      if (x >= 0 && x <= 400) 
      {

        if (y >= 100 && y <= 180) {
          numPlayers = 1;
          Serial.println("1 Player");
          //set players
          game.addPlayer("Player 1");
        }
        else if (y >= 200 && y <= 260) {
          numPlayers = 2;
          Serial.println("2 Players");

          game.addPlayer("Player 1");
          game.addPlayer("Player 2");
        }
        else if (y >= 300 && y <= 360) {
          numPlayers = 3;
          Serial.println("3 Players");

          game.addPlayer("Player 1");
          game.addPlayer("Player 2");
          game.addPlayer("Player 3");
        }
        else if (y >= 400 && y <= 460) {
          numPlayers = 4;
          Serial.println("4 Players");

          game.addPlayer("Player 1");
          game.addPlayer("Player 2");
          game.addPlayer("Player 3");
          game.addPlayer("Player 4");
        }
        else {
          return; // touched outside buttons
        }


        flushTouchBuffer();
        waitForTouchRelease();
        currentState = PLAYING;
        drawPlayingScreen();

        //initialize game here?? should not call back again 
        game.initialize();
      }
      break;
    }
    case PLAYING:
    {
      
      if (x >= 600 && x <= 800 && y >= 400 && y <= 480) 
      {
        NUM_CAL_POINTS = 3;
        Serial.println("Calibration button pressed!");
        comm.calibrating = true;
        flushTouchBuffer();
        waitForTouchRelease();
        currentState = CALIBRATION_SETUP;
        drawCalibrationSetUpScreen(); 

      }
      break;
    }
    case CALIBRATION_SETUP:
    {
      if(x >= 0 && x <= 100 && y >= 250 && y <= 400)
      {
        Serial.println("Calibration set up!");
        flushTouchBuffer();
        waitForTouchRelease();
        currentState = CALIBRATION;
        drawCalibrationScreen(); 
      }

      break;
    } 
    case CALIBRATION:
    {
      struct CalPoint {
      int px;
      int py;
      };

      CalPoint calPoints[NUM_CAL_POINTS];
      
      calPoints[NUM_CAL_POINTS];

      // Calibration touch handling here
      if (x >= 0 && y >= 0)
      {
        // store calibration point
        if (calIndex < NUM_CAL_POINTS) {
            calPoints[calIndex].px = cursorX;
            Serial.print(cursorX);
            Serial.print(" ");
            calPoints[calIndex].py = cursorY;
            Serial.print(cursorY);
            calIndex++;

            // little confirmation blink
            tft.fillCircle(cursorX, cursorY, 8, RA8875_GREEN);
            flushTouchBuffer();
            waitForTouchRelease();

            if (calIndex == NUM_CAL_POINTS) {
                // move on to calibration compute stage
                //run calibration with points
                calIndex = 0;
                comm.calibrating = false;

                flushTouchBuffer();
                waitForTouchRelease();
                currentState = PLAYING;
                drawPlayingScreen();
            }
        }
      }
      break;
    } 
      
  }

}

//calibrating screen touch coordinates to actual pixel coordinates
uint16_t UIManager::mapTouchX(uint16_t rawX) {
    return map(rawX, 47, 981, 0, 800);  // adjust after calibration
}

uint16_t UIManager::mapTouchY(uint16_t rawY) {
    return map(rawY, 147, 920, 0, 480);  // adjust after calibration
}

void UIManager::waitForTouchRelease() {
    // Wait until the RA8875 no longer reports a press
    while (tft.touched()) {
        delay(10);
    }

    // Ensure it STAYS unpressed for ~150ms (no bounces)
    unsigned long t = millis();
    while (millis() - t < 150) {
        if (tft.touched()) {  
            t = millis();     // restart wait
        }
        delay(5);
    }
}

void UIManager::flushTouchBuffer() {
    uint16_t x, y;

    // Read until no touches left
    while (tft.touched()) {
        tft.touchRead(&x, &y);
        delay(5);
    }
}

void UIManager::drawButton(const Button &btn) {
  tft.fillRect(btn.x, btn.y, btn.w, btn.h, RA8875_GREEN);
  tft.drawRect(btn.x, btn.y, btn.w, btn.h, RA8875_BLACK);

  tft.textMode();
  tft.textColor(RA8875_WHITE, RA8875_GREEN);
  tft.textEnlarge(1);
  tft.textSetCursor(btn.x + 40, btn.y + 25);
  tft.textWrite(btn.label);
}

void UIManager::drawHomeScreen() 
{
  tft.fillScreen(RA8875_WHITE);

  tft.textMode();
  tft.textColor(RA8875_BLACK, RA8875_WHITE);
  tft.textEnlarge(2);
  tft.textSetCursor(150, 120);
  tft.textWrite("Auto-Scoring Dartboard");

  drawButton(startButton);

}

void UIManager::drawGameModeSelectScreen() 
{
    
    tft.fillScreen(RA8875_WHITE);
    tft.textMode();
    tft.textColor(RA8875_BLUE, RA8875_WHITE);
    tft.textEnlarge(1);

    tft.textSetCursor(100, 50);
    tft.textWrite("Select Game Mode:");

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
    tft.textColor(RA8875_BLUE, RA8875_WHITE);
    tft.textEnlarge(1);

    tft.textSetCursor(100, 50);
    tft.textWrite("Select Number of Players:");

    tft.textSetCursor(100, 100);
    tft.textWrite("1 Player");

    tft.textSetCursor(100, 200);
    tft.textWrite("2 Players");

    tft.textSetCursor(100, 300);
    tft.textWrite("3 Players");

    tft.textSetCursor(100, 400);
    tft.textWrite("4 Players");
}

void UIManager::drawPlayingScreen() 
{
    // Start comm simulation
    comm.startSimulation();

    tft.fillScreen(RA8875_BLACK);

    //for splitting screen
    int leftW = SCREEN_W / 2;  // 400
    int leftH = SCREEN_H;      // 480
    int boardCX = leftW / 2;
    int boardCY = leftH / 2;

    // compute px/mm for scaling
    float px_per_mm = (float)leftH / (2.0f * DartConfig::R_double_out);
    if (px_per_mm * (2.0f * DartConfig::R_double_out) > leftW) {
        px_per_mm = (leftW - 10) / (2.0f * DartConfig::R_double_out);
    }

    // for hit drawing
    this->play_board_cx = boardCX;
    this->play_board_cy = boardCY;
    this->play_px_per_mm = px_per_mm;
    this->play_left_w = leftW;

    //drawing board for showing hots
    drawDartboardScaled(tft, boardCX, boardCY, px_per_mm, true);

    //vertical separation line
    tft.drawLine(leftW, 0, leftW, SCREEN_H, RA8875_WHITE);

    //score header
    tft.textMode();
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textEnlarge(1);
    tft.textSetCursor(leftW + 12, 8);
    tft.textWrite("Scores");

    //initial score panel
    redrawScorePanel();

    //callback for new locations
    comm.onNewLocation([this, leftW](std::pair<float,float> loc) {
        
        //saving each coordinate location 
        float x_mm = (float)loc.first;
        float y_mm = (float)loc.second;
        
        //compute score from location
        sr = computeScoreFromMM(x_mm, y_mm);

        Serial.println("X and Y:");
        Serial.println(x_mm);
        Serial.println(y_mm);
        Serial.println("Points Scored: ");
        Serial.println(sr.total);

        //hit marker on board 
        int px = mmToPxX(x_mm, this->play_board_cx, this->play_px_per_mm);
        int py = mmToPxY(y_mm, this->play_board_cy, this->play_px_per_mm);
        drawHitMarker(tft, px, py, RA8875_YELLOW);


        //update game state
        std::string result = game.processLocation(sr.total);
        Serial.println("Game Results: ");
        Serial.println(result.c_str());

        //update right panel scores
        redrawScorePanel();

        //show last throw
        int lastY = SCREEN_H - 72;
        tft.fillRect(leftW + 8, lastY, SCREEN_W - leftW - 16, 40, RA8875_BLACK);
        tft.textSetCursor(leftW + 12, lastY);
        char buf[128];
        if (sr.multiplier == 0) {
            snprintf(buf, sizeof(buf), "Last: MISS (r=%.1fmm)", sr.r_mm);
        } else if (sr.baseValue == 50 || sr.baseValue == 25) {
            snprintf(buf, sizeof(buf), "Last: %d pts (%s) r=%.1fmm", sr.total, sr.reason, sr.r_mm);
        } else {
            snprintf(buf, sizeof(buf), "Last: %d x%d = %d (%s) r=%.1fmm", sr.baseValue, sr.multiplier, sr.total, sr.reason, sr.r_mm);
        }
        tft.textWrite(buf);

        //game over check
        if (game.isGameOver()) {
            Serial.println("GAME OVER! Resetting the board.");
            tft.textColor(RA8875_WHITE, RA8875_RED);
            tft.textSetCursor(250, 240);
            tft.textEnlarge(2);
            tft.textWrite("GAME OVER!");
        }

        //time from recieving location to score calculated for TESTING!! 
        elapsedTime = millis() - comm.lastReceiveTime;
        Serial.println("Time: ");
        Serial.println(elapsedTime);
        Serial.println();
        Serial.println();
        //avg of time - show that would be under the 100ms requirement for real-time
    });

    // Calibration button
    tft.textColor(RA8875_WHITE, RA8875_BLUE);
    tft.textEnlarge(1);
    tft.textSetCursor(630, 450);
    tft.textWrite("Calibrate");
 
}

void UIManager::redrawScorePanel() {
    int leftW = this->play_left_w;
    int x0 = leftW + 12;
    int y0 = 50;
    int yStep = 28;

    //clear just the score half
    tft.fillRect(leftW + 2, 40, SCREEN_W - leftW - 20, SCREEN_H - 80, RA8875_BLACK);


    auto players = game.getAllPlayers();
    tft.textMode();           // Ensure using text layer
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textEnlarge(1);

    for (size_t i = 0; i < players.size(); ++i) {
        const auto &p = players[i];
        tft.textSetCursor(x0, y0 + i*yStep);

        if ((int)i == game.getCurrentPlayerIndex()) {
            tft.textWrite("> ");   // mark current player
        } else {
            tft.textWrite("  ");
        }

        char buf[64];
        snprintf(buf, sizeof(buf), "%s : %d", p.name.c_str(), p.score);
        tft.textWrite(buf);
    }

    //show current player
    tft.textSetCursor(x0, y0 + players.size()*yStep + 8);
    tft.textWrite(game.getCurrentPlayerName().c_str());
    tft.textWrite(" ");
    {
        char buf[32];
        snprintf(buf, sizeof(buf), " %d", sr.total);
        tft.textWrite(buf);
    }
    tft.textWrite(" points");

}

void UIManager::drawCalibrationSetUpScreen()
{
  tft.fillScreen(RA8875_WHITE);

  tft.textMode();
  tft.textColor(RA8875_BLUE, RA8875_WHITE);
  tft.textEnlarge(1);

  tft.textSetCursor(40, 40);
  tft.textWrite("Calibration Setup");

  tft.textSetCursor(40, 100);
  tft.textWrite("Choose number of calibration points:");

  tft.textSetCursor(40, 300);
  tft.textWrite("3");

  tft.textSetCursor(40, 400);
  tft.textWrite("4");

  tft.textSetCursor(40, 500);
  tft.textWrite("5");

  tft.textSetCursor(40, 600);
  tft.textWrite("6");
}

void UIManager::drawCalibrationScreen() 
{
    tft.fillScreen(RA8875_BLACK);

    float R_mm = DartConfig::R_double_out;
    float px_per_mm = 480.0f / (2.0f * R_mm);  

    // center of 800x480
    int cx = 400; 
    int cy = 240;

    drawDartboardScaled(tft, cx, cy, px_per_mm, true);

    //instructions
    tft.textMode();
    tft.textEnlarge(0);
    tft.textSetCursor(10, 5);
    tft.textWrite("Move cursor / Press screen to record point");

    //start cursor centered
    cursorX = cx;
    cursorY = cy;
}

// Map joystick raw value into -1.0 ... +1.0
float normalizeJoy(int val, int minV, int midV, int maxV)
{
    if (val < midV) {
        return -(float)(midV - val) / (float)(midV - minV);
    } else {
        return (float)(val - midV) / (float)(maxV - midV);
    }
}

void UIManager::updateJoystick() {
    int rawX = analogRead(VRX_PIN);  // 0–4095
    int rawY = analogRead(VRY_PIN);

    const int centerX = 1884;
    const int centerY = 1951;
    const int minVal = 86;
    const int maxVal = 4095;

    // Normalize to [-1, +1]
    joyX = (float)(rawX - centerX) / (maxVal - centerX);
    joyY = (float)(rawY - centerY) / (maxVal - centerY);

    // Deadzone
    const float dead = 0.08f;
    if (fabs(joyX) < dead) joyX = 0;
    if (fabs(joyY) < dead) joyY = 0;

}

void UIManager::updateCursor() {
    // adjust
    const float speed = 8.0; 

    cursorX += joyX * speed;
    cursorY += joyY * speed;

    // clamp to screen
    if (cursorX < 0) cursorX = 0;
    if (cursorX > 799) cursorX = 799;
    if (cursorY < 0) cursorY = 0;
    if (cursorY > 479) cursorY = 479;
}

void UIManager::drawCursor()
{
    // 2. Draw cursor at the new location
    tft.fillCircle(cursorX, cursorY, 5, RA8875_RED);
    tft.drawCircle(cursorX, cursorY, 7, RA8875_WHITE);
}

