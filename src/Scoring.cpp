#include "Scoring.h"
#include "Dartboard.h"
#include <cmath>

// We assume math coords: +X right, +Y up
static inline float angleDegFromTop(float x_mm, float y_mm) {
    float theta = atan2f(y_mm, x_mm) * 180.0f / M_PI; // -180..180, 0 at +x
    float deg_from_top = 90.0f - theta; // 0 at top, clockwise positive
    while (deg_from_top < 0.0f) deg_from_top += 360.0f;
    while (deg_from_top >= 360.0f) deg_from_top -= 360.0f;
    return deg_from_top;
}

static inline int sectorFromAngleDeg(float deg_from_top) {
    float shifted = deg_from_top + 9.0f; // center rounding
    int idx = (int)floorf(shifted / 18.0f);
    idx = idx % 20;
    if (idx < 0) idx += 20;
    return SECTOR_ORDER[idx];
}

ScoreResult computeScoreFromMM(float x_mm, float y_mm) {
    ScoreResult r;
    float rr = sqrtf(x_mm*x_mm + y_mm*y_mm);
    r.r_mm = rr;
    float deg = angleDegFromTop(x_mm, y_mm);
    r.deg = deg;

    // Miss outside double out
    if (rr > DartConfig::R_double_out) {
        r.baseValue = 0;
        r.multiplier = 0;
        r.total = 0;
        r.reason = "miss";
        return r;
    }

    // Bulls
    if (rr <= DartConfig::R_bull_inner) {
        r.baseValue = 50;
        r.multiplier = 1;
        r.total = 50;
        r.reason = "inner_bull";
        return r;
    }
    if (rr <= DartConfig::R_bull_outer) {
        r.baseValue = 25;
        r.multiplier = 1;
        r.total = 25;
        r.reason = "outer_bull";
        return r;
    }

    int base = sectorFromAngleDeg(deg);
    r.baseValue = base;

    if (rr >= DartConfig::R_double_in && rr <= DartConfig::R_double_out) {
        r.multiplier = 2;
        r.total = base * 2;
        r.reason = "double";
        return r;
    }
    if (rr >= DartConfig::R_triple_in && rr <= DartConfig::R_triple_out) {
        r.multiplier = 3;
        r.total = base * 3;
        r.reason = "triple";
        return r;
    }

    // else single
    r.multiplier = 1;
    r.total = base;
    r.reason = "single";

    

    return r;
}
