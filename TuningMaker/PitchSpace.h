#pragma once
#include "Fraction.h"
#include "Scale.h"
#include <string>
#include <map>
#include <optional>

using ScaleSigniature = std::vector<int>;

class PitchSpace
{
public:
    PitchSpace(const std::vector<Fraction>& t);

    inline size_t size() const;

    inline Fraction fractionOfRepitition() const;

    Fraction getFraction(const int& noteTo, const int& noteFrom) const;

    std::vector<std::vector<Fraction>> makeRangedScaleFractions(const std::string& scaleSigniature, const int& range) const;

    std::vector<std::vector<Fraction>> makeRangedScaleFractions(const ScaleSigniature& signiature, const int& range) const;

    void addSigniature(const std::string& signiatureName, const ScaleSigniature& signiatureIntervals);

    void removeSigniature(const std::string& signiatureName);

    std::optional<ScaleSigniature> getSigniature(const std::string& signiatureName) const;

private:
    const std::vector<Fraction> table;
    std::map<std::string, ScaleSigniature> scaleSigniatures;

};

namespace PitchSpaces
{
    static PitchSpace chromatic{
        {
            Fraction{ 17, 16 },
            Fraction{ 9, 8 },
            Fraction{ 6, 5 },
            Fraction{ 5, 4 },
            Fraction{ 4, 3 },
            Fraction{ 7, 5 },
            Fraction{ 3, 2 },
            Fraction{ 8, 5 },
            Fraction{ 5, 3 },
            Fraction{ 9, 5 },
            Fraction{ 15, 8 },
            Fraction{ 2 }
        } };

    static PitchSpace neutralSevenEDO{
        {
            Fraction{ 10, 9 },
            Fraction{ 11, 9 },
            Fraction{ 4, 3 },
            Fraction{ 3, 2 },
            Fraction{ 18, 11 },
            Fraction{ 9, 5 },
            Fraction{ 2 }
        } };
}