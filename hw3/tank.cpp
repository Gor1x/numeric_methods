#include <cmath>
#include <iostream>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

class Point {
public:
    Point() : x(0), y(0), z(0) {}

    Point(double x, double y, double z) : x(x), y(y), z(z) {}

    double x, y, z;

    void read(std::istringstream &iss) {
        iss >> x;
        iss >> y;
        iss >> z;
    }

    bool below(double level) const {
        return z <= level;
    }

    Point operator*(const Point &other) const {
        return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
    }

    double dot(const Point &other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Point operator-(const Point &other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    void print() const {
        cout << fixed << setprecision(3) << "Point: " << "x = " << x << ", " << "y = " << y << ", z = " << z << endl;
    }

    bool above(double level) const {
        return z > level;
    }
};

class Segment {
public:
    Point a, b;

    Segment(Point a, Point b) : a(a), b(b) {}

    Point splitByLevel(double level) const {
        double multiplier = (a.z - level) / (a.z - b.z);
        return Point{a.x - (a.x - b.x) * multiplier, a.y - (a.y - b.y) * multiplier, level};
    }
};

class Triangle {
public:
    Point a, b, c;

    double highestPoint() const {
        return max(max(a.y, b.y), c.y);
    }

    double lowestPoint() const {
        return min(min(a.y, b.y), c.y);
    }

    Triangle(Point a, Point b, Point c) : a(a), b(b), c(c) {}

    explicit Triangle(vector<Point> &data) {
        if (data.size() != 3) {
            throw std::exception();
        }
        a = data[0];
        b = data[1];
        c = data[2];
    }

    bool isOneVertexBelowLevel(double level) const {
        return a.below(level) || b.below(level) || c.below(level);
    }

    bool allBelowLevel(double level) const {
        return a.below(level) && b.below(level) && c.below(level);
    }

    Point &get(int i) {
        if (i == 0)
            return a;
        if (i == 1)
            return b;
        if (i == 2)
            return c;
        throw std::exception();
    }


    static bool diffSign(Point &a, Point &b, double level) {
        return a.below(level) && b.above(level) || b.below(level) && a.above(level);
    }

    vector<Triangle> splitOnTrianglesByLevel(double level) {
        for (int i = 0; i < 3; i++) {
            auto aa = get(i);
            auto bb = get((i + 1) % 3);
            auto cc = get((i + 2) % 3);
            if (diffSign(aa, bb, level)) {
                auto dd = Segment(aa, bb).splitByLevel(level);
                auto ee = diffSign(bb, cc, level) ?
                          Segment(bb, cc).splitByLevel(level) :
                          Segment(aa, cc).splitByLevel(level);
                return {
                        Triangle(aa, dd, cc),
                        Triangle(ee, dd, cc),
                        Triangle(bb, dd, ee)
                };
            }
        }
        throw std::exception();
    }
};

double tetrahedronVolume(Point d, Triangle tr) {
    return (tr.a - d).dot((tr.b - d) * (tr.c - d)) / 6.0;
}

class Tank {
public:
    vector<Triangle> data;

    bool inited = false;
    double lowestPoint;
    double highestPoint;

    void read(const string &filename) {
        std::ifstream infile(filename);

        std::string line;
        std::vector<Point> pts;
        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            string word;
            iss >> word;
            if (word == "vertex") {
                Point pt;
                pt.read(iss);
                if (!inited) {
                    inited = true;
                    lowestPoint = pt.z;
                    highestPoint = pt.z;
                }
                lowestPoint = min(lowestPoint, pt.z);
                highestPoint = max(highestPoint, pt.z);
                pts.push_back(pt);
            }
            if (word == "endloop") {
                data.emplace_back(pts);
                pts.clear();
            }
        }
    }

    double getVolumeByLevelNoSplit(double level) {
        Point d(0, 0, level);
        double sum = 0;
        for (auto tr: data) {
            if (tr.isOneVertexBelowLevel(level)) {
                sum += tetrahedronVolume(d, tr);
            }
        }
        return std::fabs(sum);
    }

    double getVolumeByLevelWithSplit(double level) {
        Point d(0, 0, level);
        double sum = 0;
        for (auto tr: data) {
            if (tr.allBelowLevel(level)) {
                sum += tetrahedronVolume(d, tr);
            } else if (tr.isOneVertexBelowLevel(level)) {
                auto smalles = tr.splitOnTrianglesByLevel(level);
                for (auto small: smalles) {
                    if (small.allBelowLevel(level))
                        sum += tetrahedronVolume(d, small);
                }
            }
        }
        return std::fabs(sum);
    }

    double getLevelByVolume(double volume, bool needSplit) {
        double lower = lowestPoint;
        double higher = highestPoint;
        while (higher - lower > 1e-6) {
            double mid = (lower + higher) / 2;
            double volumeWithMid = needSplit ?
                                   getVolumeByLevelWithSplit(mid) :
                                   getVolumeByLevelNoSplit(mid);
            if (volumeWithMid > volume) {
                higher = mid;
            } else {
                lower = mid;
            }
        }
        return lower;
    }

private:
};

void tankGetLevelByVolumeWithDebug(Tank &tank) {
    {
        double levelNoSplit = tank.getLevelByVolume(100000, false);
        double noSplit = tank.getVolumeByLevelNoSplit(levelNoSplit);
        double withSplit = tank.getVolumeByLevelWithSplit(levelNoSplit);
        cout << fixed << "Got level with no split " << levelNoSplit << ". volume by this level no split " << noSplit
             << " with split " << withSplit << endl;
    }

    {
        double levelWithSplit = tank.getLevelByVolume(100000, true);
        double noSplit = tank.getVolumeByLevelNoSplit(levelWithSplit);
        double withSplit = tank.getVolumeByLevelWithSplit(levelWithSplit);
        cout << fixed << "Got level with split " << levelWithSplit << ". volume by this level no split " << noSplit
             << " with split " << withSplit << endl;
    }
}

int main() {
    cout.precision(10);
    Tank tank;
    tank.read("tank.stl");

    tankGetLevelByVolumeWithDebug(tank);

    cout << endl << endl;
}