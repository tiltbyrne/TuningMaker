#pragma once
#include "Utilities.h"

class Fraction
{
public:
    Fraction(const int& n, const int& d = 1);

    int getNumerator() const;

    int getDenominator() const;

    Fraction reciporical() const;

    void setValue(const int& newNumerator, const int& newDenominator = 1);

    long double toLongDouble() const;

    Fraction operator*(const Fraction& otherFraction) const;

    Fraction power(const int& exponent) const;

private:
    int numerator;
    int denominator{ 1 };

    void cancel();
};

static long double harmonicEntropyOfFraction(const Fraction& fraction, const long double& entropyCurve = 1)
{
    auto returnValue{ std::pow(1L / (long double)(fraction.getNumerator() * fraction.getDenominator()), entropyCurve) };

    if (std::isnan(returnValue))
        returnValue = 0;

    return returnValue;
}