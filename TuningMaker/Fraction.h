#pragma once
#include "Utilities.h"

class Fraction
{
public:
    Fraction(const int& n, const int& d = 1);

    int getNumerator() const;

    int getDenominator() const;

    Fraction getReciporical() const;

    void setValue(const int& newNumerator, const int& newDenominator = 1);

    operator double() const;

    Fraction operator*(const Fraction& otherFraction) const;

    Fraction power(const int& exponent) const;

private:
    int numerator;
    int denominator{ 1 };

    void cancel();
};