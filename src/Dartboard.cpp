#include "Dartboard.h"
#include <Adafruit_GFX.h>
#include <math.h>

// draw a filled annular sector between rin and rout (in mm) from ang0 to ang1 (deg),
// approximated by nSteps straight segments. ang measured with 0deg at +X axis, CCW.
// Our draw uses math coords so we convert angles accordingly when placing on screen.
static void drawAnnulusSector(Adafruit_RA8875 &tft,
                              int cx, int cy, float px_per_mm,
                              float rin_mm, float rout_mm,
                              float ang0_deg, float ang1_deg,
                              uint16_t color, int nSteps = 8)
{
    if (ang1_deg <= ang0_deg) return;
    float a0 = ang0_deg * (M_PI / 180.0f);
    float a1 = ang1_deg * (M_PI / 180.0f);
    if (nSteps < 3) nSteps = 3;
    float step = (a1 - a0) / nSteps;

    for (int i = 0; i < nSteps; ++i) {
        float ta = a0 + i * step;
        float tb = a0 + (i + 1) * step;

        // points in mm (math coords: +x right, +y up)
        float x1 = rin_mm * cosf(ta), y1 = rin_mm * sinf(ta);
        float x2 = rout_mm * cosf(ta), y2 = rout_mm * sinf(ta);
        float x3 = rout_mm * cosf(tb), y3 = rout_mm * sinf(tb);
        float x4 = rin_mm * cosf(tb), y4 = rin_mm * sinf(tb);

        int sx1 = mmToPxX(x1, cx, px_per_mm), sy1 = mmToPxY(y1, cy, px_per_mm);
        int sx2 = mmToPxX(x2, cx, px_per_mm), sy2 = mmToPxY(y2, cy, px_per_mm);
        int sx3 = mmToPxX(x3, cx, px_per_mm), sy3 = mmToPxY(y3, cy, px_per_mm);
        int sx4 = mmToPxX(x4, cx, px_per_mm), sy4 = mmToPxY(y4, cy, px_per_mm);

        tft.fillTriangle(sx1, sy1, sx2, sy2, sx3, sy3, color);
        tft.fillTriangle(sx1, sy1, sx3, sy3, sx4, sy4, color);
    }
}

void drawDartboardScaled(Adafruit_RA8875 &tft, int cx, int cy, float px_per_mm, bool drawLabels)
{
    // Scoreable radius is R_double_out
    float R = DartConfig::R_double_out;

    // background circle (scoreable area)
    int r_px = (int)roundf(R * px_per_mm);
    tft.fillCircle(cx, cy, r_px, RA8875_BLACK);

    // wedge span
    const float wedge = 360.0f / 20.0f; // 18 deg

    // draw single areas (neutral alternating)
    for (int i=0;i<20;++i) {
        float centerDeg = -90.0f + i * wedge; // -90 -> 12 o'clock
        float a0 = centerDeg - wedge/2.0f;
        float a1 = centerDeg + wedge/2.0f;
        uint16_t singleColor = (i%2==0) ? RA8875_WHITE : RA8875_BLACK;
        // inner single
        drawAnnulusSector(tft, cx, cy, px_per_mm, DartConfig::R_bull_outer, DartConfig::R_triple_in, a0, a1, singleColor, 6);
        // outer single
        drawAnnulusSector(tft, cx, cy, px_per_mm, DartConfig::R_triple_out, DartConfig::R_double_in, a0, a1, singleColor, 6);
    }

    // triple ring colored segments (red/green alternating)
    for (int i=0;i<20;++i) {
        float centerDeg = -90.0f + i * wedge;
        float a0 = centerDeg - wedge/2.0f;
        float a1 = centerDeg + wedge/2.0f;
        uint16_t c = (i%2==0) ? RA8875_RED : RA8875_GREEN;
        drawAnnulusSector(tft, cx, cy, px_per_mm, DartConfig::R_triple_in, DartConfig::R_triple_out, a0, a1, c, 6);
    }

    // double ring colored segments (red/green alternating, inverted pattern)
    for (int i=0;i<20;++i) {
        float centerDeg = -90.0f + i * wedge;
        float a0 = centerDeg - wedge/2.0f;
        float a1 = centerDeg + wedge/2.0f;
        uint16_t c = (i%2==0) ? RA8875_GREEN : RA8875_RED;
        drawAnnulusSector(tft, cx, cy, px_per_mm, DartConfig::R_double_in, DartConfig::R_double_out, a0, a1, c, 6);
    }

    // draw bull
    tft.fillCircle(cx, cy, (int)roundf(DartConfig::R_bull_inner * px_per_mm), RA8875_RED);
    tft.drawCircle(cx, cy, (int)roundf(DartConfig::R_bull_outer * px_per_mm), RA8875_GREEN);

    // thin outlines for rings
    tft.drawCircle(cx, cy, (int)roundf(DartConfig::R_triple_in * px_per_mm), RA8875_WHITE);
    tft.drawCircle(cx, cy, (int)roundf(DartConfig::R_triple_out * px_per_mm), RA8875_WHITE);
    tft.drawCircle(cx, cy, (int)roundf(DartConfig::R_double_in * px_per_mm), RA8875_WHITE);
    tft.drawCircle(cx, cy, (int)roundf(DartConfig::R_double_out * px_per_mm), RA8875_WHITE);

    // radial separators
    for (int i=0;i<20;++i) {
        float ang = (-90.0f + i*wedge) * (M_PI/180.0f);
        float xr = DartConfig::R_double_out * cosf(ang);
        float yr = DartConfig::R_double_out * sinf(ang);
        int x_end = mmToPxX(xr, cx, px_per_mm);
        int y_end = mmToPxY(yr, cy, px_per_mm);
        tft.drawLine(cx, cy, x_end, y_end, RA8875_WHITE);
    }

    // draw numbers slightly inside double ring (so they fit in half-screen)
    if (drawLabels) {
        float numR = (DartConfig::R_double_out + DartConfig::R_double_in)/2.0f + 6.0f;
        tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
        tft.setTextSize(1);
        for (int i=0;i<20;++i) {
            float ang = (-90.0f + i*wedge) * (M_PI/180.0f);
            float nx = numR * cosf(ang);
            float ny = numR * sinf(ang);
            int tx = mmToPxX(nx, cx, px_per_mm);
            int ty = mmToPxY(ny, cy, px_per_mm);
            char buf[4];
            snprintf(buf, sizeof(buf), "%d", SECTOR_ORDER[i]);
            int offx = (SECTOR_ORDER[i] >= 10) ? -10 : -6;
            int offy = -6;
            tft.setCursor(tx + offx, ty + offy);
            tft.print(buf);
        }
    }
}

void drawHitMarker(Adafruit_RA8875 &tft, int px, int py, uint16_t color) {
    tft.fillCircle(px, py, 4, color);
    tft.drawCircle(px, py, 6, RA8875_WHITE);
}
