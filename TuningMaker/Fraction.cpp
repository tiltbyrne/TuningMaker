#include "Fraction.h"

Fraction::Fraction(const int& n, const int& d)
    : numerator(n), denominator(d == 0 ? 1 : d)
{
    cancel();
}

int Fraction::getNumerator() const
{
    return numerator;
}

int Fraction::getDenominator() const
{
    return denominator;
}

Fraction Fraction::reciporical() const
{
    return Fraction(denominator, numerator);
}

void Fraction::setValue(const int& newNumerator, const int& newDenominator)
{
    numerator = newNumerator;
    denominator = newDenominator == 0 ? 1 : newDenominator;
    
    cancel();
}

Fraction::operator long double() const
{
    return (long double)numerator / (long double)denominator;
}

Fraction Fraction::operator*(const Fraction& otherFraction) const
{
    return { numerator * otherFraction.numerator, denominator * otherFraction.denominator };
}

Fraction Fraction::power(const int& exponent) const
{
    return { integerPower(numerator, exponent), integerPower(denominator, exponent) };
}

void Fraction::cancel()
{
    const auto GCD{ gcd(denominator, numerator) };

    numerator /= GCD;
    denominator /= GCD;
    }