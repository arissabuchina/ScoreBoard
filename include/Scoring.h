#include <utility>

// result of scoring
struct ScoreResult {
    int baseValue;   // 1..20 or 25/50 for bulls
    int multiplier;  // 0=miss,1=single,2=double,3=triple
    int total;       // baseValue * multiplier or 25/50
    float r_mm;      // radial distance in mm
    float deg;       // degrees from top clockwise [0,360)
    const char* reason;

    int baseScore = 0;     // e.g. 20
    //int multiplier = 1;    // 1 = single, 2 = double, 3 = triple
};

// Compute the score given mm coordinates in math frame (+Y up)
// x_mm, y_mm are floats (in mm). Returns ScoreResult.
ScoreResult computeScoreFromMM(float x_mm, float y_mm);
