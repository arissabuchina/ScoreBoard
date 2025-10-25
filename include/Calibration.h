#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <vector>
#include <utility>

class Calibration {
public:
    Calibration() = default;

    void setLocations(const std::vector<std::pair<float, float>>& truePoints,
                      const std::vector<std::pair<float, float>>& estimatedPoints);

    // Solve for affine coefficients
    void computeAffineCoefficients();

    // Apply affine transformation to a point
    std::pair<float, float> applyCalibration(const std::pair<float, float>& estPoint) const;

    // Demo run
    void runDemo();

private:
    std::vector<std::pair<float, float>> trueLocations;
    std::vector<std::pair<float, float>> estimatedLocations;

    // Affine coefficients
    float a, b, c, d, e, f;

    // Matrix utilities
    std::vector<std::vector<float>> transpose(const std::vector<std::vector<float>>& mat);
    std::vector<std::vector<float>> multiply(const std::vector<std::vector<float>>& a,
                                             const std::vector<std::vector<float>>& b);
    std::vector<std::vector<float>> inverse2x2(const std::vector<std::vector<float>>& mat);
    std::vector<float> multiplyMatVec(const std::vector<std::vector<float>>& mat,
                                      const std::vector<float>& vec);
};

#endif
