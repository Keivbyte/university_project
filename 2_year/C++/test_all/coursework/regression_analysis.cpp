#include "regression_analysis.h"
#include <stdexcept>
#include <cmath>
#include <numeric>

bool RegressionAnalysis::isValid() const {
    return x_.size() >= 2 && x_.size() == y_.size();
}

void RegressionAnalysis::addPoint(double x, double y){
    x_.push_back(x);
    y_.push_back(y);
}

void RegressionAnalysis::setData(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size()) {
        throw std::invalid_argument("Vectors x and y must have the same size.");
    }
    x_ = x;
    y_ = y;
}

std::pair<double, double> RegressionAnalysis::linearRegression() const {
    if (!isValid()) {
        throw std::runtime_error("Not enough data for regression.");
    }

    size_t n = x_.size();
    double sum_x = std::accumulate(x_.begin(), x_.end(), 0.0);
    double sum_y = std::accumulate(y_.begin(), y_.end(), 0.0);
    double sum_xy = 0.0, sum_x2 = 0.0;

    for (size_t i = 0; i < n; ++i) {
        sum_xy += x_[i] * y_[i];
        sum_x2 += x_[i] * x_[i];
    }

    double denom = n * sum_x2 - sum_x * sum_x;
    if (std::abs(denom) < 1e-12) {
        throw std::runtime_error("Degenerate regression (denominator ≈ 0).");
    }

    double b1 = (n * sum_xy - sum_x * sum_y) / denom;
    double b0 = (sum_y - b1 * sum_x) / n;

    return {b0, b1};
}

double RegressionAnalysis::correlationCoefficient() const {
    if (!isValid()) return 0.0;

    size_t n = x_.size();
    double sum_x = std::accumulate(x_.begin(), x_.end(), 0.0);
    double sum_y = std::accumulate(y_.begin(), y_.end(), 0.0);
    double sum_xy = 0.0, sum_x2 = 0.0, sum_y2 = 0.0;

    for (size_t i = 0; i < n; ++i) {
        sum_xy += x_[i] * y_[i];
        sum_x2 += x_[i] * x_[i];
        sum_y2 += y_[i] * y_[i];
    }

    double numerator = n * sum_xy - sum_x * sum_y;
    double denominator = std::sqrt((n * sum_x2 - sum_x * sum_x) * (n * sum_y2 - sum_y * sum_y));

    if (std::abs(denominator) < 1e-12) return 0.0;
    return numerator / denominator;
}

void RegressionAnalysis::clear() {
    x_.clear();
    y_.clear();
}
