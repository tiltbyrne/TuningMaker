#pragma once
#include "Utilities.h"
#include "Fraction.h"
#include "Scale.h"

namespace BaseScales
{
    const std::vector<std::vector<Fraction>> majorScale{
        { {9,8}, {5,4}, {4,3}, {3,2}, {5,3}, {15,8}, {2} },
        { {9,8}, {6,5}, {4,3}, {3,2}, {5,3}, {9,5} },
        { {16,15}, {6,5}, {4,3}, {3,2}, {8,5} },
        { {9,8}, {5,4}, {7,5}, {3,2} },
        { {9,8}, {5,4}, {4,3} },
        { {9,8}, {6,5} },
        { {16,15} }
        };

    inline double harmonicEntropyOfFraction(const Fraction& fraction)
    {
        return (double)1 / fraction.getNumerator() * fraction.getDenominator();
    }

    Fraction getFractionInBaseFractions(const std::vector<std::vector<Fraction>>& baseFractions,
        const int& noteTo, const int& noteFrom)
    {
        if (noteFrom > noteTo)
            return baseFractions[noteTo][noteFrom - noteTo - 1].getReciporical();

        if (noteTo == noteFrom)
            return { 1, 0 };

        return baseFractions[noteFrom][noteTo - noteFrom - 1];
    }

    std::vector<std::vector<Interval>> baseFractionsToIntervalsWithHarmonicWeight(const std::vector<std::vector<Fraction>>& baseFractions)
    {
        std::vector<std::vector<Interval>> baseIntervals;
        baseIntervals.reserve(baseFractions.size());

        for (auto rowItr{ baseFractions.begin() }; rowItr != baseFractions.end(); ++rowItr)
        {
            std::vector<Interval> intervalsRow;

            intervalsRow.reserve(rowItr->size());
            for (auto fractionItr{ rowItr->begin() }; fractionItr != rowItr->end(); ++fractionItr)
                intervalsRow.push_back({ (double)*fractionItr, harmonicEntropyOfFraction(*fractionItr) });

            baseIntervals.push_back(intervalsRow);
        }

        return baseIntervals;
    }

    std::vector<std::vector<Interval>> baseFractionsToIntervalsWithUniformWeight(const std::vector<std::vector<Fraction>>& baseFractions)
    {
        std::vector<std::vector<Interval>> baseIntervals;
        baseIntervals.reserve(baseFractions.size());

        for (auto rowItr{ baseFractions.begin() }; rowItr != baseFractions.end(); ++rowItr)
        {
            std::vector<Interval> intervalsRow;

            intervalsRow.reserve(rowItr->size());
            for (auto fractionItr{ rowItr->begin() }; fractionItr != rowItr->end(); ++fractionItr)
                intervalsRow.push_back({ (double)*fractionItr, 1 });

            baseIntervals.push_back(intervalsRow);
        }

        return baseIntervals;
    }

    std::vector<std::vector<Fraction>> repeatBaseScaleIntervals(const std::vector<std::vector<Fraction>>& baseFractions, const int& times)
    {
        auto repeatedFractions(baseFractions);
        repeatedFractions.reserve(baseFractions.size() * times);

        const auto repeatFactor{ baseFractions[0][baseFractions.size() - 1] };

        for (auto repeat{ 1 }; repeat != times; ++repeat)
            for (auto interval{ 0 }; interval != baseFractions.size(); ++interval)
            {
                for (auto row{ 0 }; row != repeatedFractions.size(); ++row)
                    repeatedFractions[row].insert(repeatedFractions[row].end(),
                                                  repeatFactor.power(repeat - row / baseFractions.size()) *
                                                  getFractionInBaseFractions(baseFractions,
                                                                             interval + 1 % baseFractions.size(),
                                                                             row % baseFractions.size()));

                repeatedFractions.insert(repeatedFractions.end(),
                                         { repeatFactor.power(repeat - repeatedFractions.size() / baseFractions.size()) *
                                           getFractionInBaseFractions(baseFractions,
                                                                      interval + 1 % baseFractions.size(),
                                                                      repeatedFractions.size() % baseFractions.size()) });
            }

        return repeatedFractions;
    }
}