//regression_analysis.h
#pragma once
#include <vector>
#include <utility>

class RegressionAnalysis {
private:
    std::vector<double> x_;
    std::vector<double> y_;

public:
    RegressionAnalysis() = default;

    void addPoint(double x, double y);
    void setData(const std::vector<double>& x, const std::vector<double>& y);

    bool isValid() const;

    std::pair<double, double> linearRegression() const;
    double correlationCoefficient() const;

    void clear();
};
