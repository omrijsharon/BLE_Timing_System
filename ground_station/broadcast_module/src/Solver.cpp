#include <Arduino.h>
#include <algorithm>

#include "Solver.h"

float matrixDet3x3(const std::vector<std::vector<float>>& mat) {
    return mat[0][0] * (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]) -
           mat[0][1] * (mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0]) +
           mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]);
}

std::vector<float> matrixMultiply(const std::vector<std::vector<float>>& mat, const std::vector<float>& vec) {
    std::vector<float> result(vec.size(), 0);
    for (size_t i = 0; i < mat.size(); ++i) {
        for (size_t j = 0; j < mat[0].size(); ++j) {
            result[i] += mat[i][j] * vec[j];
        }
    }
    return result;
}

float matrixDet2x2(const std::vector<std::vector<float>>& matrix) {
    return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
}

float quadraticFit(const std::vector<float>& x, const std::vector<float>& y, float& a, float& b, float& c) {
    if (x.size() != y.size() || x.size() < 3) {
        return -1;
    }

    float minX = *std::min_element(x.begin(), x.end());
    std::vector<float> x_offset(x.size());
    for (size_t i = 0; i < x.size(); ++i) {
        x_offset[i] = x[i] - minX;
    }

    float Sx = 0, Sy = 0, Sxx = 0, Sxy = 0, Sxxx = 0, Sxxy = 0, Sxxxx = 0;
    for (size_t i = 0; i < x_offset.size(); ++i) {
        float xi = x_offset[i];
        float yi = y[i];
        Sx += xi;
        Sy += yi;
        Sxx += xi * xi;
        Sxy += xi * yi;
        Sxxx += xi * xi * xi;
        Sxxy += xi * xi * yi;
        Sxxxx += xi * xi * xi * xi;
    }

    std::vector<std::vector<float>> matA = {
        {static_cast<float>(x_offset.size()), Sx, Sxx},
        {Sx, Sxx, Sxxx},
        {Sxx, Sxxx, Sxxxx}
    };

    std::vector<float> vecB = {Sy, Sxy, Sxxy};

    float detA = matrixDet3x3(matA);
    if (detA == 0) {
        return -1;
    }

    std::vector<std::vector<float>> matAi(3, std::vector<float>(3));
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            std::vector<std::vector<float>> submat(2, std::vector<float>(2));
            for (int row = 0, new_row = 0; row < 3; ++row) {
                if (row == i) {
                    continue;
                }
                for (int col = 0, new_col = 0; col < 3; ++col) {
                    if (col == j) {
                        continue;
                    }
                    submat[new_row][new_col] = matA[row][col];
                    ++new_col;
                }
                ++new_row;
            }
            matAi[j][i] = ((i + j) % 2 == 0 ? 1 : -1) * matrixDet2x2(submat);
        }
    }

    a = 0;
    b = 0;
    c = 0;
    std::vector<float> coeffs = matrixMultiply(matAi, vecB);
    a = coeffs[0] / detA;
    b = coeffs[1] / detA;
    c = coeffs[2] / detA;

    float t_min = -b / (2 * c);
    float true_t_min = t_min + minX;
    return true_t_min;
}

void movingAverageFilter(const std::vector<float>& input, std::vector<float>& output, int windowSize) {
  // Implement the moving average filter function here
}

unsigned long measureQuadraticFitExecutionTime(const std::vector<float> &x, const std::vector<float> &y, float &a, float &b, float &c) {
  unsigned long startTime = micros();
  quadraticFit(x, y, a, b, c);
  unsigned long endTime = micros();

  return endTime - startTime;
}