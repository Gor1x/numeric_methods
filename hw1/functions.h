#ifndef HW1_FUNCTIONS_H
#define HW1_FUNCTIONS_H

#include <cmath>
#include <algorithm>

float polynomial(float x, const float* a, int n);

float kahan_sum(const float* x, int n);

float dummy_sum(const float* x, int n);

float pairwise_sum_simd(float* x, int n);

class Statistics {
private:
    float savedSum = 0;
    int savedCount = 0;
    float savedMean = 0;
    float helperM = 0;
    float savedMin;
    float savedMax;

public:
    void update(float x);            // добавить новый элемент
    int count() const noexcept;

    float min() const noexcept;

    float max() const noexcept;

    float sum() const noexcept;

    float mean() const noexcept;     // среднее
    float variance() const noexcept; // дисперсия
};


float length(const float* x, int n);

#endif //HW1_FUNCTIONS_H
