#pragma once
#include <iostream>
#include <cmath>
#include <array>
#include <vector>
#include <numeric>
#include <iomanip>

static int gcd(int a, int b)
{
    while (b != 0)
    {
        int temp = b;
        b = a % b;
        a = temp;
    }

    return std::abs(a);
}

static int integerPower(int base, int exp)
{
    int result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

static inline double centsFromRatio(const double& ratio)
{
    return 1200 * std::log2(ratio);
}

static inline double frequencyFromRatio(const double& ratio, const double& baseFrequency)
{
    if (ratio < 0)
        return 999999999999999;

    return ratio * baseFrequency;
}

static long double clampLongDoubleToLimits(const double& value)
{
    constexpr long double min{ std::numeric_limits<long double>::min() },
                          max{ std::numeric_limits<long double>::max() };

    if (std::isnan(value))
        return 0;

    if (value < min)
        return min;

    if (value > max)
        return max;

    return value;
}