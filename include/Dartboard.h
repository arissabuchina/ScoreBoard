#include <Arduino.h>
#include <Adafruit_RA8875.h>

// Dartboard geometry (mm) - from your provided values
namespace DartConfig {
    constexpr float R_outer       = 225.5f;   // full board radius (not used visually)
    constexpr float R_double_out  = 170.0f;   // scoreable outer radius (mm)
    constexpr float R_double_in   = 162.0f;
    constexpr float R_triple_out  = 107.0f;
    constexpr float R_triple_in   = 99.0f;
    constexpr float R_bull_outer  = 16.0f;
    constexpr float R_bull_inner  = 6.35f;
}

// Standard sector order (12 o'clock = 20) clockwise
static const int SECTOR_ORDER[20] = {
    20, 1, 18, 4, 13, 6, 10, 15, 2, 17, 
    3, 19, 7, 16, 8, 11, 14, 9, 12, 5
};

// draw the dartboard centered at (cx,cy) in pixels, using px_per_mm scale.
// if fullDraw==true we draw all labels; optional coloring parameters kept simple.
void drawDartboardScaled(Adafruit_RA8875 &tft, int cx, int cy, float px_per_mm, bool drawLabels = true);

// helper that converts mm (math coords: +X right, +Y up) -> screen px for an arbitrary center and scale
inline int mmToPxX(float mm, int cx, float px_per_mm) { return cx + (int)roundf(mm * px_per_mm); }
inline int mmToPxY(float mm, int cy, float px_per_mm) { return cy - (int)roundf(mm * px_per_mm); }

// small utility to draw a hit marker on the board (pixel coords)
void drawHitMarker(Adafruit_RA8875 &tft, int px, int py, uint16_t color = RA8875_YELLOW);
