#include "Calibration.h"
#include "esp_dsp.h"
#include <vector>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <HardwareSerial.h>

void Calibration::setLocations(const std::vector<std::pair<float, float>>& truePoints,
                               const std::vector<std::pair<float, float>>& estimatedPoints) {
    if (truePoints.size() != estimatedPoints.size())
        throw std::runtime_error("Number of true and estimated points must match.");

    trueLocations = truePoints;
    estimatedLocations = estimatedPoints;
}


void Calibration::computeAffineCoefficients() 
{
    const int N = trueLocations.size();
    if (N < 3)
        throw std::runtime_error("Need at least 3 calibration points.");

    const int rows = 2 * N;
    const int cols = 6;

    // H matrix: 2N x 6
    std::vector<float> H(rows * cols, 0.0f);
    std::vector<float> T(rows, 0.0f);

    for (int i = 0; i < N; ++i) {
        float X = estimatedLocations[i].first;
        float Y = estimatedLocations[i].second;
        float x = trueLocations[i].first;
        float y = trueLocations[i].second;

        // Equation for x
        H[i*cols + 0] = X;
        H[i*cols + 1] = Y;
        H[i*cols + 2] = 1;
        H[i*cols + 3] = 0;
        H[i*cols + 4] = 0;
        H[i*cols + 5] = 0;
        T[i] = x;

        // Equation for y
        H[(i+N)*cols + 0] = 0;
        H[(i+N)*cols + 1] = 0;
        H[(i+N)*cols + 2] = 0;
        H[(i+N)*cols + 3] = X;
        H[(i+N)*cols + 4] = Y;
        H[(i+N)*cols + 5] = 1;
        T[i+N] = y;
    }

    // Compute HtH = H^T * H and HtT = H^T * T
    float HtH[6][6] = {0};
    float HtT[6] = {0};

    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 6; ++j) {
            float sum = 0;
            for (int k = 0; k < rows; ++k)
                sum += H[k*cols + i] * H[k*cols + j];
            HtH[i][j] = sum;
        }
        float sumT = 0;
        for (int k = 0; k < rows; ++k)
            sumT += H[k*cols + i] * T[k];
        HtT[i] = sumT;
    }

    // Solve 6x6 system using Gaussian elimination
    auto solve6x6 = [](float H[6][6], float T[6], float theta[6]) {
        int n = 6;
        // Gaussian elimination with partial pivoting
        for (int i = 0; i < n; i++) {
            // Pivot
            int maxRow = i;
            for (int k = i+1; k < n; k++)
                if (fabs(H[k][i]) > fabs(H[maxRow][i]))
                    maxRow = k;
            for (int k = 0; k < n; k++) std::swap(H[i][k], H[maxRow][k]);
            std::swap(T[i], T[maxRow]);

            // Eliminate
            for (int k = i+1; k < n; k++) {
                float f = H[k][i] / H[i][i];
                for (int j = i; j < n; j++)
                    H[k][j] -= H[i][j] * f;
                T[k] -= T[i] * f;
            }
        }

        // Back substitution
        for (int i = n-1; i >= 0; i--) {
            theta[i] = T[i];
            for (int j = i+1; j < n; j++)
                theta[i] -= H[i][j] * theta[j];
            theta[i] /= H[i][i];
        }
    };

    float theta[6];
    solve6x6(HtH, HtT, theta);

    // Assign coefficients
    a = theta[0]; b = theta[1]; e = theta[2];
    c = theta[3]; d = theta[4]; f = theta[5];

    std::cout << "\nComputed Affine Coefficients:\n"
              << "a: " << a << ", b: " << b << ", e: " << e << "\n"
              << "c: " << c << ", d: " << d << ", f: " << f << "\n";
} 


std::pair<float, float> Calibration::applyCalibration(const std::pair<float, float>& estPoint) const {
    float xCal = a * estPoint.first + b * estPoint.second + e;
    float yCal = c * estPoint.first + d * estPoint.second + f;
    return {xCal, yCal};
}

void Calibration::runDemo() {
    // Demo points

    std::cout << "\n\nUsing 3 points from last demo \n";

    std::vector<std::pair<float, float>> truePts = {{1,1}, {2,2}, {3,3}};
    std::vector<std::pair<float, float>> estPts  = {{1.1,0.9}, {2.1,1.9}, {3.05,2.95}};

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




    std::cout << "\n\nUsing 4 points\n";

    std::vector<std::pair<float, float>> truePts_4 = {{1,1}, {2,2}, {3,3}, {4,4}};
    std::vector<std::pair<float, float>> estPts_4  = {{1.1,0.9}, {2.1,1.9}, {3.05,2.95}, {4.5,6.8}};

    setLocations(truePts_4, estPts_4);

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
    for (size_t i = 0; i < estPts_4.size(); ++i) {
        auto calPt = applyCalibration(estPts_4[i]);
        std::cout << "(" << truePts_4[i].first << ", " << truePts_4[i].second << ")"
                  << "        (" << estPts_4[i].first << ", " << estPts_4[i].second << ")"
                  << "          (" << calPt.first << ", " << calPt.second << ")\n";
    }






    std::cout << "\n\nUsing 5 points\n";

    std::vector<std::pair<float, float>> truePts_5 = {{1,1}, {2,2}, {3,3}, {4,4}, {5,5}};
    std::vector<std::pair<float, float>> estPts_5  = {{1.1,0.9}, {2.1,1.9}, {3.05,2.95}, {4.5,6.8}, {5.2,4.9}};

    setLocations(truePts_5, estPts_5);

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
    for (size_t i = 0; i < estPts_5.size(); ++i) {
        auto calPt = applyCalibration(estPts_5[i]);
        std::cout << "(" << truePts_5[i].first << ", " << truePts_5[i].second << ")"
                  << "        (" << estPts_5[i].first << ", " << estPts_5[i].second << ")"
                  << "          (" << calPt.first << ", " << calPt.second << ")\n";
    }


  }




