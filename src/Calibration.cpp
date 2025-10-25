#include "Calibration.h"
#include <iostream>
#include <stdexcept>
#include <iomanip>

void Calibration::setLocations(const std::vector<std::pair<float, float>>& truePoints,
                               const std::vector<std::pair<float, float>>& estimatedPoints) {
    if (truePoints.size() != estimatedPoints.size())
        throw std::runtime_error("Number of true and estimated points must match.");

    trueLocations = truePoints;
    estimatedLocations = estimatedPoints;
}

std::vector<std::vector<float>> invert3x3(const std::vector<std::vector<float>>& m) {
    float det = 
        m[0][0]*(m[1][1]*m[2][2]-m[1][2]*m[2][1]) -
        m[0][1]*(m[1][0]*m[2][2]-m[1][2]*m[2][0]) +
        m[0][2]*(m[1][0]*m[2][1]-m[1][1]*m[2][0]);

    if(det == 0) throw std::runtime_error("Matrix not invertible");

    float invDet = 1.0f / det;
    std::vector<std::vector<float>> inv(3, std::vector<float>(3));

    inv[0][0] =  (m[1][1]*m[2][2]-m[1][2]*m[2][1])*invDet;
    inv[0][1] = -(m[0][1]*m[2][2]-m[0][2]*m[2][1])*invDet;
    inv[0][2] =  (m[0][1]*m[1][2]-m[0][2]*m[1][1])*invDet;
    inv[1][0] = -(m[1][0]*m[2][2]-m[1][2]*m[2][0])*invDet;
    inv[1][1] =  (m[0][0]*m[2][2]-m[0][2]*m[2][0])*invDet;
    inv[1][2] = -(m[0][0]*m[1][2]-m[0][2]*m[1][0])*invDet;
    inv[2][0] =  (m[1][0]*m[2][1]-m[1][1]*m[2][0])*invDet;
    inv[2][1] = -(m[0][0]*m[2][1]-m[0][1]*m[2][0])*invDet;
    inv[2][2] =  (m[0][0]*m[1][1]-m[0][1]*m[1][0])*invDet;

    return inv;
}

void Calibration::computeAffineCoefficients() {
    if(trueLocations.size() != 3 || estimatedLocations.size() != 3)
        throw std::runtime_error("Demo requires exactly 3 points");

    // Build H and x, y vectors
    std::vector<std::vector<float>> H(3, std::vector<float>(3));
    std::vector<float> xVec(3), yVec(3);

    for(int i=0; i<3; ++i){
        H[i][0] = estimatedLocations[i].first;
        H[i][1] = estimatedLocations[i].second;
        H[i][2] = 1.0f;
        xVec[i] = trueLocations[i].first;
        yVec[i] = trueLocations[i].second;
    }

    // Invert H
    auto Hinv = invert3x3(H);

    // Compute coefficients: alpha = H^-1 * x, beta = H^-1 * y
    a = Hinv[0][0]*xVec[0] + Hinv[0][1]*xVec[1] + Hinv[0][2]*xVec[2];
    b = Hinv[1][0]*xVec[0] + Hinv[1][1]*xVec[1] + Hinv[1][2]*xVec[2];
    e = Hinv[2][0]*xVec[0] + Hinv[2][1]*xVec[1] + Hinv[2][2]*xVec[2];

    c = Hinv[0][0]*yVec[0] + Hinv[0][1]*yVec[1] + Hinv[0][2]*yVec[2];
    d = Hinv[1][0]*yVec[0] + Hinv[1][1]*yVec[1] + Hinv[1][2]*yVec[2];
    f = Hinv[2][0]*yVec[0] + Hinv[2][1]*yVec[1] + Hinv[2][2]*yVec[2];
}

std::pair<float, float> Calibration::applyCalibration(const std::pair<float, float>& estPoint) const {
    float xCal = a * estPoint.first + b * estPoint.second + e;
    float yCal = c * estPoint.first + d * estPoint.second + f;
    return {xCal, yCal};
}

void Calibration::runDemo() {
    // Demo points
    std::vector<std::pair<float, float>> truePts = {{0,0}, {1,0}, {0,1}};
    std::vector<std::pair<float, float>> estPts  = {{0.1,0.2}, {1.05,-0.1}, {-0.05,1.1}};

    setLocations(truePts, estPts);

    try {
        computeAffineCoefficients();
    } catch (std::runtime_error& e) {
        std::cout << "Compute coefficients skipped in demo: " << e.what() << "\n";
    }

    // Print header
    std::cout << std::setw(12) << "\nTrue" 
              << std::setw(20) << "Estimated" 
              << std::setw(20) << "Calibrated" << "\n";

    // Apply calibration and print all points
    for (size_t i = 0; i < estPts.size(); ++i) {
        auto calPt = applyCalibration(estPts[i]);
        std::cout << "(" << truePts[i].first << ", " << truePts[i].second << ")"
                  << "        (" << estPts[i].first << ", " << estPts[i].second << ")"
                  << "          (" << calPt.first << ", " << calPt.second << ")\n";
    }
}
