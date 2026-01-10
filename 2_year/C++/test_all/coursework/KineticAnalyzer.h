// KineticAnalyzer.h
#pragma once
#include <vector>

class KineticAnalyzer {
private:
    std::vector<double> times_;
    std::vector<double> concA_exp_;
    double cb0_, cc0_, cd0_, ce0_;

    double n_ = 0.0;
    double k_ = 0.0;
    double r_ = 0.0;
    double dispersion_ = 0.0;

public:
    KineticAnalyzer(double cb0 = 0.0, double cc0 = 0.0, double cd0 = 0.0, double ce0 = 0.0);

    void setExperimentalData(const std::vector<double>& times, const std::vector<double>& concA);
    void analyze();

    double getOrder() const { return n_; }
    double getRateConstant() const { return k_; }
    double getCorrelation() const { return r_; }
    double getDispersion() const { return dispersion_; }

    std::vector<double> simulateConcentrations() const;
};
