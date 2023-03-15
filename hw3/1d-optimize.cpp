#include <cmath>
#include <iostream>
#include <random>

using namespace std;


const double EPS_F_DIFF = 1e-3;
const double EPS_X_DIFF = 1e-3;
const double MIN_X = -5;
const double MAX_X = 5;
const int MAX_STEPS = 10000;

class RastriginFunction {
public:
    static bool differentSign(double a, double b) {
        return of(a) * of(b) < 0.0;
    }

    static double of(double x) {
        return 10.0 + x * x - 10 * cos(2 * M_PI * x);
    }

    static double absOf(double x) {
        return fabs(of(x));
    }

    static double getDeriv(double x) {
        return 2.0 * x + 20 * M_PI * sin(2 * M_PI * x);
    }


    static double deriv_deriv(double x) {
        return 2 + 40 * M_PI * M_PI * cos(2 * M_PI * x);
    }


    static bool differentSignDeriv(double a, double b) {
        return getDeriv(a) * getDeriv(b) < 0.0;
    }
};

struct Result {
    double point;
    int steps;
};

Result bisect1d(double a, double b, int step) {
    double mid = a + (b - a) / 2;

    if (step > MAX_STEPS) return {mid, step};

    if (RastriginFunction::absOf(mid) < EPS_F_DIFF) return {mid, step};

    if (RastriginFunction::differentSignDeriv(a, mid)) {
        return bisect1d(a, mid, step + 1);
    } else {
        return bisect1d(mid, b, step + 1);
    }
}

Result newton1d(double x, int step) {
    if (step > MAX_STEPS) return {x, step};
    double rastr = RastriginFunction::getDeriv(x);
    double der = RastriginFunction::deriv_deriv(x);

    if (abs(rastr) < EPS_F_DIFF) return {x, step};
    if (abs(der) < EPS_F_DIFF) return {x, step};
    double xNew = x - rastr / der;
    if (abs(xNew - x) < EPS_X_DIFF) return {x, step};
    return newton1d(xNew, step + 1);
}

class Randomer {
    std::mt19937 gen;
    std::uniform_real_distribution<double> dis;
public:
    explicit Randomer() : gen(0), dis(MIN_X, MAX_X) {
    }

    double get() {
        return dis(gen);
    }
};

void printBisectStat(Randomer &randomer, int tries) {
    int bisectSteps = 0;
    for (int i = 0; i < tries; ++i) {
        auto x1 = randomer.get();
        auto x2 = randomer.get();
        if (x2 > x1)
            swap(x1, x2);
        auto [m, step] = bisect1d(x1, x2, 0);
        bisectSteps += step;
    }

    cout << "Bisect method: "
         << (double) bisectSteps / tries
         << " avg. steps" << endl;
}

void printNewtonStat(Randomer &randomer, int tries) {
    int newtonSteps = 0;
    for (int i = 0; i < tries; ++i) {
        double x = randomer.get();
        auto [m, step] = newton1d(x, 0);
        newtonSteps += step;
    }
    cout << "Newton method: "
         << (double) newtonSteps / tries
         << " avg. steps" << endl;
}

int main() {
    Randomer randomer;
    int n_tries = 30;
    printBisectStat(randomer, n_tries);
    printNewtonStat(randomer, n_tries);
}