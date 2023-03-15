#include <cmath>
#include <iostream>
#include <random>

using namespace std;


const double EPS_F_DIFF = 1e-3;
const double EPS_X_DIFF = 1e-3;
const double MIN_X = -5;
const double MAX_X = 5;
const int MAX_STEPS = 10000;

class Point2d {
public:
    double x, y;

    Point2d(double x, double y);

    static Point2d getMidPoint(Point2d a, Point2d b);

    double &operator[](int i);

};



class RastriginFunction2d {
public:
    static bool differentSign(Point2d a, Point2d b) {
        return of(a) * of(b) < 0.0;
    }

    static double of1d(double x) {
        return 10.0 + x * x - 10 * cos(2 * M_PI * x);
    }

    static double of(Point2d x) {
        return of1d(x.x) + of1d(x.y);
    }

    static double absOf(Point2d x) {
        return fabs(of(x));
    }

    static double deriv1d(double x) {
        return 2.0 * x + 20 * M_PI * sin(2 * M_PI * x);
    }

    static double deriv_deriv1d(double x) {
        return 2 + 40 * M_PI * M_PI * cos(2 * M_PI * x);
    }


    static bool differentSignDeriv1d(double a, double b) {
        return deriv1d(a) * deriv1d(b) < 0.0;
    }
};


struct Result {
    Point2d point;
    int steps;
};

Result bisect2d(Point2d a, Point2d b, int step) {
    auto mid = Point2d::getMidPoint(a, b);

    if (step > MAX_STEPS) return {mid, step};

    if (RastriginFunction2d::absOf(mid) < 2 * EPS_F_DIFF) return {mid, step};
    if (fabs(a.x - b.x) + fabs(a.y - b.y) < EPS_X_DIFF * 2) return {mid, step};

    // Choose the dimension where something can be optimized
    int dim;
    if (fabs(a.x - b.x) > EPS_X_DIFF && RastriginFunction2d::differentSignDeriv1d(a.x, b.x)) {
        dim = 0;
    } else {
        dim = 1;
    }

    // Choose which part we can drop in selected dimension
    if (RastriginFunction2d::differentSignDeriv1d(a[dim], mid[dim])) {
        b[dim] = mid[dim];
        return bisect2d(a, b, step + 1);
    } else {
        a[dim] = mid[dim];
        return bisect2d(a, b, step + 1);
    }
}

Result newton2d(Point2d pt, int step) {
    if (step > MAX_STEPS) return {pt, step};
    double valX = RastriginFunction2d::deriv1d(pt.x);
    double valY = RastriginFunction2d::deriv1d(pt.y);
    double derX = RastriginFunction2d::deriv_deriv1d(pt.x);
    double derY = RastriginFunction2d::deriv_deriv1d(pt.y);

    if (abs(derX) < EPS_F_DIFF && abs(derY) < EPS_F_DIFF) return {pt, step};

    double newX;
    double newY;
    if (abs(derX) < EPS_F_DIFF)
        newX = pt.x;
    else
        newX = pt.x - valX / derX;

    if (abs(derY) < EPS_F_DIFF)
        newY = pt.y;
    else
        newY = pt.y - valY / derY;
    if (abs(newX - pt.x) < EPS_X_DIFF && fabs(newY - pt.y) < EPS_X_DIFF) return {pt, step};
    return newton2d({newX, newY}, step + 1);
}

class Randomer {
    std::mt19937 gen;
    std::uniform_real_distribution<double> dis;
public:
    explicit Randomer() : gen(0), dis(MIN_X, MAX_X) {
    }

    Point2d get() {
        return Point2d(dis(gen), dis(gen));
    }
};

void changeCoords(Point2d &a, Point2d &b) {
    if (a.x > b.x) {
        swap(a.x, b.x);
    }
    if (a.y > b.y) {
        swap(a.y, b.y);
    }
    if (a.x > 0)
        a.x *= -1;
    if (a.y > 0)
        a.y *= -1;

    if (b.x < 0)
        b.x *= -1;
    if (b.y < 0)
        b.y *= -1;
}

void printBisectStat(Randomer &randomer, int tries) {
    int bisectSteps = 0;
    int bisectHits = 0;
    for (int i = 0; i < tries; ++i) {
        auto x1 = randomer.get();
        auto x2 = randomer.get();
        changeCoords(x1, x2);
        auto result = bisect2d(x1, x2, 0);

        if (RastriginFunction2d::absOf(result.point) < EPS_F_DIFF)
            bisectHits++;
        bisectSteps += result.steps;
    }

    // Если посмотреть на конкретные результаты, то видно, что сходится к пикам функции. Обычно не везёт.
    cout << "Bisect method: "
         << (double) bisectSteps / tries
         << " avg. steps" << endl;
}

void printNewtonStat(Randomer &randomer, int tries) {
    int newtonSteps = 0;
    int newTonHits = 0;
    for (int i = 0; i < tries; ++i) {
        auto x = randomer.get();
        auto result = newton2d(x, 0);
        if (RastriginFunction2d::absOf(result.point) < EPS_F_DIFF) newTonHits++;
        newtonSteps += result.steps;
    }
    cout << "Newton method: "
         << (double) newtonSteps / tries
         << " avg. steps" << endl;
}

int main() {
    Randomer randomer;
    int n_tries = 100;
    printBisectStat(randomer, n_tries);
    printNewtonStat(randomer, n_tries);
}


Point2d::Point2d(double x, double y) : x(x), y(y) {

}

Point2d Point2d::getMidPoint(Point2d a, Point2d b) {
    return Point2d((a.x + b.x) / 2, (a.y + b.y) / 2);
}

double& Point2d::operator[](int i) {
    if (i == 0)
        return x;
    if (i == 1)
        return y;
    throw std::exception();
}