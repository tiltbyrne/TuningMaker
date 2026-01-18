#pragma once
#include "Utilities.h"
#include "Fraction.h"

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
    const std::vector<std::vector<Interval>> intervals;

    double sumWeights(const int& noteTo, std::vector<int>& notesFrom) const;

    double makeTuning(const int& rootNote, int& note, const double& weightLimit) const;

    double traverseScale(int& lastNote, std::vector<int>& possibleNextNotesInPath,
        const int& rootNote, const double& rollingWeight, const double& weightLimit) const;
};