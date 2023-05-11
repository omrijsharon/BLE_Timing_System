#ifndef SOLVER_H
#define SOLVER_H

#include <vector>

float matrixDet3x3(const std::vector<std::vector<float>>& mat);

float quadraticFit(const std::vector<float>& x, const std::vector<float>& y, float& a, float& b, float& c);

void movingAverageFilter(const std::vector<float>& input, std::vector<float>& output, int windowSize);

unsigned long measureQuadraticFitExecutionTime(const std::vector<float> &x, const std::vector<float> &y, float &a, float &b, float &c);

#endif // SOLVER_H