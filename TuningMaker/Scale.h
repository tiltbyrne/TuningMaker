#pragma once
#include "Fraction.h"
#include "Utilities.h"

struct Interval
{
    double size;
    double weight{ 1 };
};

class Scale
{
public:
    Scale(const std::vector<std::vector<Interval>>& i);

    inline size_t size() const;

    Interval getInterval(const int& noteTo, const int& noteFrom) const;

    std::vector<double> tuneScale(const double& trueRootNote, const double& weightLimit = 0) const;

private:
    const std::vector<std::vector<Interval>> intervalsPattern;

    double sumWeights(const int& noteTo, std::vector<int>& notesFrom) const;

    double makeTuning(const int& rootNote, int& note, const double& weightLimit) const;

    double traverseScale(int& lastNote, std::vector<int>& possibleNextNotesInPath,
        const int& rootNote, const double& rollingWeight, const double& weightLimit) const;
};

static std::vector<std::vector<Interval>> fractionsToIntervalsWithHarmonicWeight(const std::vector<std::vector<Fraction>>& baseFractions, const double& entropyCurve = 1)
{
    std::vector<std::vector<Interval>> baseIntervals;
    baseIntervals.reserve(baseFractions.size());

    for (auto rowItr{ baseFractions.begin() }; rowItr != baseFractions.end(); ++rowItr)
    {
        std::vector<Interval> intervalsRow;

        intervalsRow.reserve(rowItr->size());
        for (auto fractionItr{ rowItr->begin() }; fractionItr != rowItr->end(); ++fractionItr)
            intervalsRow.push_back({ (double)*fractionItr, harmonicEntropyOfFraction(*fractionItr, entropyCurve) });

        baseIntervals.push_back(intervalsRow);
    }

    return baseIntervals;
}

static std::vector<std::vector<Interval>> baseFractionsToIntervalsWithUniformWeight(const std::vector<std::vector<Fraction>>& baseFractions)
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