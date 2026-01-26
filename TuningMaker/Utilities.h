#pragma once
#include <iostream>
#include <cmath>
#include <array>
#include <vector>
#include <numeric>
#include <iomanip>

/*
  Returns the greatest common denominator of integers a and b, used by Fraction.
*/
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
/*
  Returns base raised to exponent.
*/
static int integerPower(int base, int exponent)
{
    int result = 1;
    for (;;)
    {
        if (exponent & 1)
            result *= base;
        exponent >>= 1;
        if (!exponent)
            break;
        base *= base;
    }

    return result;
}

/*
  Returns cents values (one 100th of a 12edo semitone) from a frequency ratio.
*/
static inline double centsFromRatio(const double& ratio)
{
    return 1200 * std::log2(ratio);
}

/*
  Returns the frequency (Hz) of a ratio from baseFrequency.
*/
static inline double frequencyFromRatio(const double& ratio, const double& baseFrequency)
{
    return ratio * baseFrequency;
}

/*
  Enforces value does not exceed it's numeric limits and if it is NaN returns 0.
*/
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

/*
  Returns true if pattern is triangular. A pattern is triangular if pattern[n].size() == pattern[n + 1].size() + 1
  for all vectors of T in pattern.
*/
template<typename T>
static bool patternHasTriangularDimensions(const std::vector<std::vector<T>>& pattern)
{
    for (auto rowSize{ 0 }; rowSize != pattern.size(); ++rowSize)
        if (pattern[rowSize].size() != pattern.size() - rowSize)
            return false;

    return true;
}