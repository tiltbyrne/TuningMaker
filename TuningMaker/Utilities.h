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

static inline double centsFromRatio(const long double& ratio)
{
    return 1200 * std::log2(ratio);
}

static long double clampLongDoubleToLimits(const long double& value)
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