#include <cmath>
#include <iostream>
#include "functions.h"

float polynomial(float x, const float* a, int n) {
    float result = a[n - 1];
    for (int i = n - 2; i >= 0; i--) {
        result = std::fma(result, x, a[i]);
    }
    return result;
}

#pragma GCC push_options
#pragma GCC optimize ("O0")

float kahan_sum(const float* x, int n) {
    float s = 0;
    float c = 0;
    for (int i = 0; i < n; i++) {
        float y = x[i] - c;
        float t = s + y;
        c = (t - s) - y;
        s = t;
    }
    return s;
}

#pragma GCC pop_options

#pragma GCC push_options
//#pragma GCC optimize ("O3")

#define print(x, y) std::cout << (x) <<" " << y << std::endl;

float pairwise_sum_simd(float* x, int n) {
    int len = n;

    while (len > 1) {
        int i = 0;
        for (; i * 2 + 7 < len; i += 4) {
            x[i + 0] = x[i * 2 + 0] + x[i * 2 + 4];
            x[i + 1] = x[i * 2 + 1] + x[i * 2 + 5];
            x[i + 2] = x[i * 2 + 2] + x[i * 2 + 6];
            x[i + 3] = x[i * 2 + 3] + x[i * 2 + 7];
        }
        for (; i * 2 + 1 < len; i++) {
            x[i] = x[i * 2] + x[i * 2 + 1];
        }
        if (len % 2 == 1) {
            x[len / 2] = x[len - 1];
        }
        len = (len + 1) / 2;
    }
    return x[0];
}

#pragma GCC pop_options

float dummy_sum(const float* x, int n) {
    float sum = 0;
    for (int i = 0; i < n; i++) {
        sum += x[i];
    }
    return sum;
}

void Statistics::update(float x) {
    if (savedCount == 0) {
        savedMin = x;
        savedMax = x;
    } else {
        savedMin = std::min(savedMin, x);
        savedMax = std::max(savedMax, x);
    }
    savedCount++;
    savedSum += x;

    float newMean = savedMean + (x - savedMean) / (float) savedCount;
    helperM = helperM + (x - savedMean) * (x - newMean);

    savedMean = newMean;
}

float Statistics::min() const noexcept {
    return savedMin;
}

int Statistics::count() const noexcept {
    return savedCount;
}

float Statistics::max() const noexcept {
    return savedMax;
}

float Statistics::sum() const noexcept {
    return savedSum;
}

float Statistics::mean() const noexcept {
    return savedMean;
}

float Statistics::variance() const noexcept {
    return helperM / (float) count();
}


float length(const float* x, int n) {
    float sum = 0;
    float maxLength = 1.f;
    for (int i = 0; i < n; i++) {
        float curLen = std::fabs(x[i]);
        if (curLen > maxLength) {
            float coef = maxLength / curLen;
            sum *= coef * coef;
            maxLength = curLen;
        }
        float cur = x[i] / maxLength;
        sum += cur * cur;
    }
    return maxLength * std::sqrt(sum);
}