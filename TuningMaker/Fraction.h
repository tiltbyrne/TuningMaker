#pragma once
#include "Utilities.h"

/*
  A simple, minimal class representing a (superparticular) fraction.
*/
class Fraction
{
public:
    /*
      Constructs a fraction 1/1;
    */
    Fraction();

    /*
      Constructs a fraction n/d or or n/1. All fractions will be cancelled.
    */
    Fraction(const int& n, const int& d = 1);

    /*
      Returns the numerator of the fraction.
    */
    int getNumerator() const;

    /*
      Returns the denominator of the fraction.
    */
    int getDenominator() const;

    /*
      Returns Fraction(denominator, numerator).
    */
    Fraction reciporical() const;

    /*
      Sets the value of the fraction and cancells it.
    */
    void setValue(const int& newNumerator, const int& newDenominator = 1);

    /*
      Returns the fraction converted into a long double.
    */
    long double toLongDouble() const;

    /*
      Less-than comparator operator.
    */
    bool operator<(const Fraction& otherFraction) const;

    /*
      Equal to comparator operator.
    */
    bool operator==(const Fraction& otherFraction) const;
    
    /*
      Multiplication operator.
    */
    Fraction operator*(const Fraction& otherFraction) const;

    /*
      Returns the fraction raised to the power of exponent and returns it as a fraction.
    */
    Fraction power(const int& exponent) const;

    /*
      Ostream input.
    */
    friend std::ostream& operator<<(std::ostream& os, const Fraction& dt);

private:
    int numerator;
    int denominator;

    /*
      Cancels the numerator and denominator of the fraction (devides both by their greatest common divisor).
    */
    void cancel();
};

/*
  The Tenney height of a fraction is one over the product of it's numerator and denominator. This function returns the
  Tenney height of fraction raised to the power of entropyCurve.
*/
static long double tenneyHeightOfFraction(const Fraction& fraction, const long double& entropyCurve = 1)
{
    auto returnValue{ std::pow(1L / (long double)(fraction.getNumerator() * fraction.getDenominator()), entropyCurve) };

    if (std::isnan(returnValue))
        returnValue = 0;

    return returnValue;
}