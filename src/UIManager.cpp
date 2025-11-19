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
      if (x >= 200 && x <= 450 && y >= 200 && y <= 350) {
        Serial.println("Start button pressed!");
        
        currentState = SELECT_GAME;
        flushTouchBuffer();
        waitForTouchRelease();
        drawGameModeSelectScreen();
        
      }
      break;


    case SELECT_GAME:
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


    case SELECT_PLAYERS:
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
      }
      break;
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
  tft.fillRect(btn.x, btn.y, btn.w, btn.h, RA8875_WHITE);
  tft.drawRect(btn.x, btn.y, btn.w, btn.h, RA8875_BLACK);

  tft.textMode();
  tft.textColor(RA8875_BLACK, RA8875_WHITE);
  tft.textEnlarge(1);
  tft.textSetCursor(btn.x + 40, btn.y + 25);
  tft.textWrite(btn.label);
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

void UIManager::drawGameModeSelectScreen() 
{
    
    tft.fillScreen(RA8875_BLACK);
    tft.textMode();
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
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
    tft.fillScreen(RA8875_BLACK);
    tft.textMode();
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
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

/*
void UIManager::drawPlayingScreen() 
{
    tft.fillScreen(RA8875_BLACK);
    tft.textMode();
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textEnlarge(1);

    // --- Header ---
    tft.textSetCursor(280, 20);
    tft.textWrite("Game In Progress");

    tft.textSetCursor(100, 80);
    tft.textWrite("Scores:");

    //comm.begin();

    // --- Initialize game ---
    game.initialize();

    // --- Draw initial score area ---
    int yStart = 120;
    int yStep = 40;

    for (size_t i = 0; i < game.getAllPlayers().size(); i++) {
        const auto& p = game.getAllPlayers()[i];
        tft.textSetCursor(100, yStart + i * yStep);
        tft.textWrite((p.name + ": " + std::to_string(p.score)).c_str());
    }

    // --- Listen for location updates ---
    comm.onNewLocation([this, yStart, yStep](std::pair<int, int> location) 
    {
        std::string result = game.processLocation(location);
        Serial.println(result.c_str());

        // --- Update score display ---
        tft.fillRect(80, yStart, 400, 200, RA8875_BLACK);  // Clear score area
        tft.textMode();
        tft.textColor(RA8875_WHITE, RA8875_BLACK);
        tft.textEnlarge(1);

        auto players = game.getAllPlayers();
        for (size_t i = 0; i < players.size(); i++) {
            const auto& p = players[i];
            tft.textSetCursor(100, yStart + i * yStep);
            std::string line = p.name + ": " + std::to_string(p.score);
            tft.textWrite(line.c_str());
        }

        // --- Optional: show last throw result ---
        tft.fillRect(100, 320, 600, 40, RA8875_BLACK);
        tft.textSetCursor(100, 320);
        tft.textWrite(("Last throw: " + result).c_str());

        // --- Game over check ---
        if (game.isGameOver()) {
            Serial.println("GAME OVER! Resetting the board to play again.");

            tft.fillScreen(RA8875_RED);
            tft.textColor(RA8875_WHITE, RA8875_RED);
            tft.textSetCursor(250, 240);
            tft.textEnlarge(2);
            tft.textWrite("GAME OVER!");

            //delay(3000);
            //game.reset();
            //drawHomeScreen();
        }
    });

    comm.startSimulation();
}
*/

void UIManager::drawPlayingScreen() 
{
    tft.fillScreen(RA8875_BLACK);
    tft.textMode();
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.textEnlarge(1);

    // Left area: small board (leftHalfW x SCREEN_H)
    int leftW = SCREEN_W / 2; // 400
    int leftH = SCREEN_H;     // 480
    int boardCX = leftW / 2;
    int boardCY = leftH / 2;

    // compute px_per_mm for half-screen: fit height primarily
    float px_per_mm_half = (float)leftH / (2.0f * DartConfig::R_double_out); // same approach as earlier
    // ensure width fits
    if (px_per_mm_half * (2.0f * DartConfig::R_double_out) > leftW) {
        px_per_mm_half = (leftW - 10) / (2.0f * DartConfig::R_double_out);
    }

    // save these to member vars for use during hits
    this->play_board_cx = boardCX;
    this->play_board_cy = boardCY;
    this->play_px_per_mm = px_per_mm_half;
    this->play_left_w = leftW;

    // draw small scaled dartboard
    drawDartboardScaled(tft, boardCX, boardCY, px_per_mm_half, true);

    // vertical separator
    tft.drawLine(leftW, 0, leftW, SCREEN_H, RA8875_WHITE);

    // right panel header
    tft.setCursor(leftW + 12, 8);
    tft.setTextSize(2);
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.print("Scores / Info");

    // initial draw of players
    redrawScorePanel();
    
    // attach comm callback to show hits live
    comm.onNewLocation([this, leftW](std::pair<int,int> loc) {
        // loc = (x_mm, y_mm) integers (math coords +Y up) as per your system
        float x_mm = (float)loc.first;
        float y_mm = (float)loc.second;

        // compute score
        ScoreResult sr = computeScoreFromMM(x_mm, y_mm);
        Serial.println(sr.total);

        // Draw hit on left board (convert mm->px)
        int px = mmToPxX(x_mm, this->play_board_cx, this->play_px_per_mm);
        int py = mmToPxY(y_mm, this->play_board_cy, this->play_px_per_mm);

        // plot temporary marker
        drawHitMarker(tft, px, py, RA8875_YELLOW);

        // Update game logic: pass int mm coords into Game
        //std::pair<int,int> intLoc = { (int)roundf(x_mm), (int)roundf(y_mm) };

        
        std::string result = game.processLocation(sr.total);
        
        
        //std::string result = game.processLocation(intLoc);

        //Serial.println(result.c_str());

        // Update score panel
        //redrawScorePanel();

        // show last throw summary
        tft.fillRect(leftW + 8, SCREEN_H - 80, SCREEN_W - leftW - 16, 72, RA8875_BLACK);
        tft.setCursor(leftW + 12, SCREEN_H - 72);
        tft.setTextSize(1);
        char buf[128];
        if (sr.multiplier == 0) {
            snprintf(buf, sizeof(buf), "Last: MISS (r=%.1fmm)", sr.r_mm);
        } else if (sr.baseValue == 50 || sr.baseValue == 25) {
            snprintf(buf, sizeof(buf), "Last: %d pts (%s) r=%.1fmm", sr.total, sr.reason, sr.r_mm);
        } else {
            snprintf(buf, sizeof(buf), "Last: %d x%d = %d (%s) r=%.1fmm", sr.baseValue, sr.multiplier, sr.total, sr.reason, sr.r_mm);
        }
        tft.print(buf);

        // optional: small delay so marker visible, then optionally redraw board area to clear marker or keep it
        delay(300);
        // redraw small board to remove marker (or comment out if you want markers to persist)
        drawDartboardScaled(tft, this->play_board_cx, this->play_board_cy, this->play_px_per_mm, true);
        redrawScorePanel();
    });

    // start comm simulation (if not already)
    comm.startSimulation();
}

// helper to redraw the right-side score panel (players)
void UIManager::redrawScorePanel() {
    int leftW = this->play_left_w;
    int x0 = leftW + 12;
    int y0 = 40;
    int yStep = 28;
    tft.fillRect(leftW + 2, 40, SCREEN_W - leftW - 4, SCREEN_H - 44, RA8875_BLACK); // clear panel area

    auto players = game.getAllPlayers();
    tft.setTextSize(1);
    for (size_t i = 0; i < players.size(); ++i) {
        const auto &p = players[i];
        tft.setCursor(x0, y0 + i*yStep);
        // mark current player
        if ((int)i == game.getCurrentPlayerIndex()) {
            tft.print("> ");
        } else {
            tft.print("  ");
        }
        char buf[64];
        snprintf(buf, sizeof(buf), "%s : %d", p.name.c_str(), p.score);
        tft.print(buf);
    }

    // show current player name stronger
    tft.setCursor(x0, y0 + players.size()*yStep + 8);
    tft.setTextSize(1);
    tft.print("Current: ");
    tft.print(game.getCurrentPlayerName().c_str());
}
