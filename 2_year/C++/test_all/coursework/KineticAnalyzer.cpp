#include "KineticAnalyzer.h"
#include "regression_analysis.h"
#include <stdexcept>
#include <cmath>
#include <numeric>

// --- KineticAnalyzer implementation ---
KineticAnalyzer::KineticAnalyzer(double cb0, double cc0, double cd0, double ce0)
    : cb0_(cb0), cc0_(cc0), cd0_(cd0), ce0_(ce0) {}

void KineticAnalyzer::setExperimentalData(const std::vector<double>& times, const std::vector<double>& concA) {
    if (times.size() != concA.size() || times.size() < 2) {
        throw std::invalid_argument("Invalid experimental data");
    }
    times_ = times;
    concA_exp_ = concA;
}

void KineticAnalyzer::analyze() {
    // 1. Вычисляем приближённые скорости w_i = |ΔC_A| / Δt
    size_t N = times_.size() - 1;
    std::vector<double> x(N); // ln(C_A,i)
    std::vector<double> y(N); // ln(w_i)

    for (size_t i = 0; i < N; ++i) {
        double dt = times_[i + 1] - times_[i];
        double dC = std::abs(concA_exp_[i + 1] - concA_exp_[i]);

        if (dt <= 0 || concA_exp_[i] <= 0 || dC <= 0) {
            throw std::runtime_error("Invalid concentration or time data");
        }

        double w = dC / dt;
        x[i] = std::log(concA_exp_[i]);
        y[i] = std::log(w);
    }

    // 2. Регрессия: y = ln k + n * x
    RegressionAnalysis reg;
    reg.setData(x, y);
    auto [lnk, n] = reg.linearRegression(); // b0 = lnk, b1 = n

    n_ = n;
    k_ = std::exp(lnk);
    r_ = reg.correlationCoefficient();

    // 3. Моделирование и дисперсия
    dispersion_ = 0.0;
    double ca = concA_exp_[0];
    for (size_t i = 0; i < times_.size(); ++i) {
        if (i > 0) {
            double dt = times_[i] - times_[i - 1];
            double dca = k_ * std::pow(ca, n_) * dt;
            ca -= dca;
            if (ca < 0) ca = 0;
        }
        dispersion_ += std::pow(concA_exp_[i] - ca, 2);
    }
}

std::vector<double> KineticAnalyzer::simulateConcentrations() const {
    std::vector<double> result;
    double ca = concA_exp_[0];
    double cb = cb0_, cc = cc0_, cd = cd0_, ce = ce0_;

    result.push_back(ca); // [A]
    result.push_back(cb); // [B]
    result.push_back(cc); // [C]
    result.push_back(cd); // [D]
    result.push_back(ce); // [E]

    for (size_t i = 1; i < times_.size(); ++i) {
        double dt = times_[i] - times_[i - 1];
        double dca = k_ * std::pow(ca, n_) * dt;
        ca = std::max(0.0, ca - dca);
        cb = std::max(0.0, cb - 2 * dca); // стехиометрия: 2B на 1A
        cc += dca;
        cd += dca;
        ce += dca;

        result.push_back(ca);
        result.push_back(cb);
        result.push_back(cc);
        result.push_back(cd);
        result.push_back(ce);
    }
    return result;
}
