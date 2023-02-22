#include <iostream>
#include <bitset>
#include <cassert>
#include "functions.h"
#include <iomanip>
#include <vector>

void checkSimpleSums();

using bs32 = std::bitset<32>;

void print(float f) {
    bs32 bs = bs32(*reinterpret_cast<uint32_t*>(&f));
    std::cout << bs << '\n';
}

void printFraction(float f) {
    std::bitset<32> bits(*reinterpret_cast<uint32_t*>(&f));
    std::cout << "scale=20\n";
    std::cout << "(2^(0";
    for (int i = 0; i <= 7; ++i) {
        if (bits[23 + i]) {
            std::cout << " + " << bits[23 + i] * (1 << i);
        }
    }
    std::cout << " - 127)) * (1";
    for (int i = 1; i <= 23; ++i) {
        if (bits[23 - i]) {
            std::cout << " + " << bits[23 - i] << '/' << (1 << (i));
        }
    }
    std::cout << ")";
    std::cout << '\n';
}

void checkPolynomial() {
    float a[] = {1.0, 2.0, 3.0};
    assert(polynomial(3.0, a, 3) == 34.0);
}

void checkKahanSum() {
    std::cout << "Kahan test start" << std::endl;
    std::vector<float> d;
    for (int i = 0; i < 1000; i++) {
        if (i % 2 == 0) {
            d.push_back(1000000.0 + 1.f / 3.f);
        } else {
            d.push_back(1000000.0 + 2.f / 3.f);
        }
    }
    float expected = 1000000000.0 + 500.0;
    float kahan = kahan_sum(d.data(), d.size());
    float dummy = dummy_sum(d.data(), d.size());
    std::cout << std::fixed << std::setprecision(60) << expected - kahan << std::endl << expected - dummy << std::endl;
    print(expected - kahan);
    print(expected - dummy);
    std::cout << "Kahan test end" << std::endl;
}

void checkPairwiseSum() {
    std::cout << "Pairwise test start" << std::endl;
    std::vector<float> d;
    for (int i = 0; i < 1000; i++) {
        if (i % 2 == 0) {
            d.push_back(100000.0 + 1.f / 3.f);
        } else {
            d.push_back(100000.0 + 2.f / 3.f);
        }
    }
    float expected = 100000000.0 + 500.0;
    float dummy = dummy_sum(d.data(), d.size());
    float pairwise = pairwise_sum_simd(d.data(), d.size());
    std::cout << std::fixed << std::setprecision(60) <<
              "diff pairwise " << expected - pairwise << std::endl <<
              "diff dummy " << expected - dummy
              << std::endl;
    print(expected - pairwise);
    print(expected - dummy);
    std::cout << "Pairwise test end" << std::endl << std::endl;
}

void checkStats() {
    std::cout << std::endl;
    Statistics statistics;
    for (auto v: {2.f, 4.f, 6.f, 8.f, 10.f}) {
        statistics.update(v);
    }
    assert(statistics.count() == 5);
    assert(statistics.sum() == 30.f);
    assert(statistics.mean() == 6.f);
    assert(statistics.variance() == 8.f);
    assert(statistics.min() == 2.f);
    assert(statistics.max() == 10.f);
    std::cout << "STATISTICS TEST CORRECT" << std::endl;
}

void checkLength() {
    std::vector<float> d = {2.0, 3.0, 6.0};
    assert(std::fabs(length(d.data(), d.size()) - 7.0) < 1e-5);
}

int main() {
    checkPolynomial();
    checkSimpleSums();
    checkKahanSum();
    checkPairwiseSum();
    checkStats();
    checkLength();
}

void checkSimpleSums() {
    std::vector<float> d = {1.f + 1.f / 3.f, 2.f + 2.f / 3.f, 1.f + 2.f / 3.f, 1.f + 1.f / 3.f};

    float dummy = dummy_sum(d.data(), d.size());
    float kahan = kahan_sum(d.data(), d.size());
    float simd = pairwise_sum_simd(d.data(), d.size());
    std::cout << std::fixed << std::setprecision(16) << simd << std::endl << dummy << std::endl << kahan << std::endl;
    assert(simd == 7.0);
}
