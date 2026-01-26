#include "Scale.h"
#include <algorithm>

Scale::Scale()
{
}

Scale::Scale(const std::string& n)
    : name(n)
{
}

Scale::Scale(const std::vector<std::vector<Interval>>& i)
    : intervalsPattern(patternHasTriangularDimensions(i) ? i : std::vector<std::vector<Interval>>{})
{
    normaliseWeights();
}

Scale::Scale(const std::vector<std::vector<Interval>>& i, const std::string& n)
    : intervalsPattern(patternHasTriangularDimensions(i) ? i : std::vector<std::vector<Interval>>{})
    , name(n)
{
    normaliseWeights();
}

inline size_t Scale::size() const
{
    return intervalsPattern.size() + 1;
}

void Scale::setIntervalsPattern(const std::vector<std::vector<Interval>>& newIntervalsPattern)
{
    if (patternHasTriangularDimensions(newIntervalsPattern))
    {
        intervalsPattern = newIntervalsPattern;

        normaliseWeights();
        setDummyIndecies({});
    }
}

void Scale::setDummyIndecies(const std::vector<int>& newDummyIndecies)
{
    if (!newDummyIndecies.empty() && *std::min_element(newDummyIndecies.begin(), newDummyIndecies.end()) < 0)
        return;

    dummyIndecies = newDummyIndecies;
    std::sort(dummyIndecies.begin(), dummyIndecies.end());
}

void Scale::setName(const std::string& newName)
{
    name = newName;
}

inline std::string Scale::getName() const
{
    return name;
}

Interval Scale::getInterval(const int& noteTo, const int& noteFrom) const
{
    if (noteFrom > noteTo)
        return { 1L / intervalsPattern[noteTo][noteFrom - noteTo - 1].getSize(),
                 intervalsPattern[noteTo][noteFrom - noteTo - 1].getWeight() };

    if (noteTo == noteFrom)
        return { 1, 0 };

    return intervalsPattern[noteFrom][noteTo - noteFrom - 1];
}

std::vector<double> Scale::tuneScale(const int& trueRootNote, const long double& weightLimit) const
{
    auto tunings{ makePopulatedTunings(weightLimit) };

    auto tuning{ normaliseTuningsAndMakeAverageTuning(tunings, trueRootNote) };

    return insertDummyNotes(tuning);
}

long double Scale::sumWeights(const int& noteTo, std::vector<int>& notesFrom) const
{
    long double sum{ 0 };

    for (auto& noteFrom : notesFrom)
        sum += getInterval(noteTo, noteFrom).getWeight();

    return sum;
}

long double Scale::makeTuning(const int& rootNote, int& note, const long double& weightLimit) const
{
    long double tunedNote{ 1 };

    long double adjustmentFactor{ 1 };
    bool adjustmentFactorWasSet{ false };

    std::vector<int> nextNotes(size());
    std::iota(nextNotes.begin(), nextNotes.end(), 0);
    nextNotes.erase(find(nextNotes.begin(), nextNotes.end(), note));

    const auto firstRollingWeight{ 1 / sumWeights(note, nextNotes) };

    return traverseScale(note, nextNotes, rootNote, firstRollingWeight, weightLimit, firstRollingWeight);
}

long double Scale::traverseScale(int& lastNote, std::vector<int>& possibleNextNotesInPath,
    const int& rootNote, const long double& rollingWeight, const long double& weightLimit, 
    const long double& possibleWeightsToNoteSum) const
{
    long double returnValue{ 1 };

    for (auto nextNoteIndex{ 0 }; nextNoteIndex != possibleNextNotesInPath.size(); ++nextNoteIndex)
    {
        const auto nextNote{ possibleNextNotesInPath[nextNoteIndex] };
        const auto nextInterval{ getInterval(lastNote, nextNote) };

        if (nextNote == rootNote || nextInterval.getWeight() * rollingWeight <= weightLimit)
            returnValue *= std::pow(getInterval(lastNote, rootNote).getSize(), nextInterval.getWeight() * possibleWeightsToNoteSum);
        else
        {
            const auto initialLastNote{ lastNote };

            lastNote = nextNote;
            possibleNextNotesInPath.erase(possibleNextNotesInPath.begin() + nextNoteIndex);

            const auto sumWeightsToNextNote{ 1 / sumWeights(nextNote, possibleNextNotesInPath) };

            returnValue *= std::pow(nextInterval.getSize() * traverseScale(lastNote, possibleNextNotesInPath, rootNote,
                                                           nextInterval.getWeight() * rollingWeight * sumWeightsToNextNote, weightLimit, sumWeightsToNextNote),
                                    nextInterval.getWeight() * possibleWeightsToNoteSum);

            possibleNextNotesInPath.insert(possibleNextNotesInPath.begin() + nextNoteIndex, lastNote);
            lastNote = initialLastNote;
        }
    }

    return returnValue;
}

std::vector<std::vector<long double>> Scale::makePopulatedTunings(const long double& weightLimit) const
{
    auto percentTuned{ [this](int& rootNote, int& note) -> long double
        {
            return (((long double)rootNote / (long double)size())
                + ((long double)note / ((long double)size() * (long double)size()))) * 100;
        }
    };

    std::vector<std::vector<long double>> tunings(size(), std::vector<long double>(size()));

    long double lastPercentage{ 0 };
    const long double loadingInterval{ 0.1 };

    std::cout << "Tuning " << name << std::endl << std::endl;
    std::cout << std::fixed << std::setprecision(1) << "Progress: 0.0% \r";

    for (auto rootNote{ 0 }; rootNote != size(); ++rootNote)
        for (auto note{ 0 }; note != size(); ++note)
        {
            tunings[rootNote][note] = rootNote == note ? 1 : makeTuning(rootNote, note, weightLimit);

            const auto percentage{ percentTuned(rootNote, note) };

            if (percentage - lastPercentage >= loadingInterval)
            {
                std::cout << "Progress: " << percentage << "% \r";
                lastPercentage = percentage;
            }
        }

    std::cout << "Progress: 100.0% \r\n" << std::endl;

    return tunings;
}

std::vector<double> Scale::normaliseTuningsAndMakeAverageTuning(std::vector<std::vector<long double>>& tunings, const int& trueRootNote) const
{
    //normalise
    for (auto rootNote{ 1 }; rootNote != size(); ++rootNote)
        if (rootNote != trueRootNote)
        {
            const auto adjustmentFactor{ 1L / tunings[rootNote][trueRootNote] };

            for (auto& tunedNote : tunings[rootNote])
                tunedNote *= adjustmentFactor;
        }

    //average
    std::vector<double> averageTuning(size(), 1);

    for (auto note{ 0 }; note != size(); ++note)
        for (auto rootNote{ 0 }; rootNote != size(); ++rootNote)
            averageTuning[note] *= std::pow(tunings[rootNote][note], 1L / (long double)size());

    return averageTuning;
}

std::vector<double> Scale::insertDummyNotes(std::vector<double>& tuning) const
{
    tuning.reserve(tuning.size() + dummyIndecies.size());

    auto insertionAdjustment{ 0 };

    for (const auto& index : dummyIndecies)
        if (index + insertionAdjustment >= 0 && index < tuning.size())
        {
            tuning.insert(tuning.begin() + index + insertionAdjustment, std::numeric_limits<double>::quiet_NaN());

            ++insertionAdjustment;
        }

    return tuning;
}

/*
  Normalises the weights of all intervals in intervalsPattern to a range of (0, 1].
*/
void Scale::normaliseWeights()
{
    long double maxWeight{ 0 };
    for (const auto& row : intervalsPattern)
        for (const auto& Interval : row)
            if (Interval.getWeight() > maxWeight)
                maxWeight = Interval.getWeight();

    for (auto& row : intervalsPattern)
        for (auto& Interval : row)
            Interval.setWeight(Interval.getWeight() / maxWeight);
}