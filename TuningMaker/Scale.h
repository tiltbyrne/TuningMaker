#pragma once
#include "Fraction.h"
#include "Utilities.h"
#include <limits>

struct Interval
{
    Interval(const long double& s, const long double& w)
        : size{ clampLongDoubleToLimits(s) }
        , weight{ clampLongDoubleToLimits(w) }
    {}

    long double size;
    long double weight{ 1 };
};

class Scale
{
public:
    Scale(const std::vector<std::vector<Interval>>& i);

    Scale(const std::vector<std::vector<Interval>>& i, const std::string& n);

    inline size_t size() const;

    void setIntervalsPattern(const std::vector<std::vector<Interval>>& newIntervalsPattern);

    void setDummyIndecies(const std::vector<int>& newDummyIntervals);

    void setName(const std::string& newName);

    inline std::string getName() const;

    std::vector<double> tuneScale(const int& trueRootNote, const long double& weightLimit = 0) const;

private:

    std::vector<std::vector<Interval>> intervalsPattern;
	//dummy notes, which equal -1, are inserted at these indecies. The function of dummy notes is to create spaces
	//in the scale for notes with 0Hz frequency, which can be fingering scales which are have less than 12 notes per octave
    std::vector<int> dummyIndecies;
    std::string name;

    Interval getInterval(const int& noteTo, const int& noteFrom) const;

    long double sumWeights(const int& noteTo, std::vector<int>& notesFrom) const;

    long double makeTuning(const int& rootNote, int& note, const long double& weightLimit) const;

    long double traverseScale(int& lastNote, std::vector<int>& possibleNextNotesInPath, const int& rootNote,
                              const long double& rollingWeight, const long double& weightLimit,
                              const long double& possibleWeightsToNoteSum) const;

    bool intervalsPatternHasValidDimensions(const std::vector<std::vector<Interval>>& intervals) const;

    std::vector<std::vector<long double>> makePopulatedTunings(const long double& weightLimit) const;

    std::vector<double> normaliseTuningsAndMakeAverageTuning(std::vector<std::vector<long double>>& tunings,
                                                                  const int& trueRootNote) const;

    std::vector<double> insertDummyNotes(std::vector<double>& tuning) const;
};

static std::vector<std::vector<Interval>> fractionsToIntervalsWithHarmonicWeight(const std::vector<std::vector<Fraction>>& baseFractions, const long double& entropyCurve = 1)
{
    std::vector<std::vector<Interval>> baseIntervals;
    baseIntervals.reserve(baseFractions.size());

    long double maxWeight{ 0 };
    for (auto rowItr{ baseFractions.begin() }; rowItr != baseFractions.end(); ++rowItr)
    {
        std::vector<Interval> intervalsRow;

        intervalsRow.reserve(rowItr->size());
        for (auto fractionItr{ rowItr->begin() }; fractionItr != rowItr->end(); ++fractionItr)
        {
            const auto weight{ harmonicEntropyOfFraction(*fractionItr, entropyCurve) };
            intervalsRow.push_back({ fractionItr->toLongDouble(), weight });

            if (weight > maxWeight)
                maxWeight = weight;
        }

        baseIntervals.push_back(intervalsRow);
    }

    //normalising weights
    for (auto& row : baseIntervals)
        for (auto& Interval : row)
            Interval.weight /= maxWeight;

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
            intervalsRow.push_back({ fractionItr->toLongDouble(), 1});

        baseIntervals.push_back(intervalsRow);
    }

    return baseIntervals;
}