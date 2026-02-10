#include "Scale.h"
#include <algorithm>

Interval::Interval()
    : size{ 1 }
    , weight{ 1 }
{
}

Interval::Interval(const long double& s, const long double& w)
    : size{ clampLongDoubleToLimits(s) }
    , weight{ clampLongDoubleToLimits(w) }
{
    manageZeroWeight();
}

inline long double Interval::getSize() const
{
    return size;
}

inline long double Interval::getWeight() const
{
    return weight;
}

void Interval::setSize(const long double& newSize)
{
    size = clampLongDoubleToLimits(newSize);
}

void Interval::setWeight(const long double& newWeight)
{
    weight = clampLongDoubleToLimits(newWeight);
    manageZeroWeight();
}

void Interval::setInterval(const long double& newSize, const long double& newWeight)
{
    setSize(newSize);
    setWeight(newWeight);
}

void Interval::manageZeroWeight()
{
    if (weight <= 0)
        weight = std::numeric_limits<long double>::lowest();
}

Scale::Scale(const long double& c)
    : weightCutoff(c < 0 ? 0 : c > 1 ? 1 : c)
{
}

Scale::Scale(const std::string& n, const long double& c)
    : name(n)
    , weightCutoff(c < 0 ? 0 : c > 1 ? 1 : c)
{
}

Scale::Scale(const IntervalsPattern& i, const long double& c)
    : intervalsPattern(patternHasTriangularDimensions(i) ? i : IntervalsPattern{})
    , weightCutoff(c < 0 ? 0 : c > 1 ? 1 : c)
{
    normaliseWeights();
}

Scale::Scale(const IntervalsPattern& i, const std::string& n, const long double& c)
    : intervalsPattern(patternHasTriangularDimensions(i) ? i : IntervalsPattern{})
    , name(n)
    , weightCutoff(c < 0 ? 0 : c > 1 ? 1 : c)
{
    normaliseWeights();
}

inline size_t Scale::size() const
{
    return intervalsPattern.size() + 1;
}

void Scale::setIntervalsPattern(const IntervalsPattern& newIntervalsPattern)
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


long double Scale::getWeightCutoff() const
{
    return weightCutoff;
}

void Scale::setWeightCutoff(const long double& newWeightCutoff)
{
    weightCutoff = newWeightCutoff < 0 ? 0
                                       : newWeightCutoff > 1 ? 1
                                                             : newWeightCutoff;
}

Interval Scale::getInterval(const int& noteTo, const int& noteFrom) const
{
    if (noteFrom > noteTo)
        return { 1L / intervalsPattern[noteTo][noteFrom - noteTo - 1].getSize(),
                 intervalsPattern[noteTo][noteFrom - noteTo - 1].getWeight() };

    if (noteTo == noteFrom)
        return { 1, 1 };

    return intervalsPattern[noteFrom][noteTo - noteFrom - 1];
}

long double Scale::getMinWeight() const
{
    auto minWeight{ intervalsPattern[0][0].getWeight() };
    for (const auto& row : intervalsPattern)
        for (const auto& interval : row)
            if (interval.getWeight() < minWeight)
                minWeight = interval.getWeight();

    return minWeight;
}

long double Scale::getMaxWeight() const
{
    auto maxWeight{ intervalsPattern[0][0].getWeight() };
    for (const auto& row : intervalsPattern)
        for (const auto& interval : row)
            if (interval.getWeight() > maxWeight)
                maxWeight = interval.getWeight();

    return maxWeight;
}

std::vector<float> Scale::tuneScale(const int& rootNote) const
{
    auto tuning{ makePopulatedTuning() };

    adjustTuningRootNote(tuning, (rootNote < 0 ? 0 : rootNote >= size() ? size() - 1 : rootNote));

    return insertDummyNotes(tuning);
}

long double Scale::prodWeights(const int& noteFrom, const std::vector<int>& notesTo) const
{
    if (notesTo.empty())
        return 0;

    long double prod{ 1 };

    for (const auto& noteTo : notesTo)
        prod *= getInterval(noteFrom, noteTo).getWeight();

    return clampLongDoubleToLimits(prod);
}

long double Scale::sumProdWeights(const std::vector<int>& notesFrom) const
{
    long double sum{ 0 };

    for (const auto& noteFrom : notesFrom)
        sum += prodWeights(noteFrom, notesFrom);

    return sum;
}

long double Scale::tuneNote(int& note) const
{
    if (note == 0)
        return 1;

    std::vector<int> nextNotes(size());
    std::iota(nextNotes.begin(), nextNotes.end(), 0);

    const auto reciporicalSumOfProdWeights{ 1 / sumProdWeights(nextNotes) };

    return traversePath(note, nextNotes, 1, reciporicalSumOfProdWeights);
}

long double Scale::traversePath(const int& currentNoteIndex, const std::vector<int>& possibleNextNotes,
    const long double& rollingWeight, const long double& reciporicalSumOfProdWeights) const
{
    const auto currentNote{ possibleNextNotes[currentNoteIndex] };

    auto nextPossibleNextNotes{ possibleNextNotes };
    nextPossibleNextNotes.erase(nextPossibleNextNotes.begin() + currentNoteIndex);

    const auto nextReciporicalSumProdWeights{ 1 / sumProdWeights(nextPossibleNextNotes) };

    long double returnValue{ 1 };

    for (auto nextNoteIndex{ 0 }; nextNoteIndex != possibleNextNotes.size(); ++nextNoteIndex)
    {
        const auto nextNote{ possibleNextNotes[nextNoteIndex] };
        const auto exponent{ clampLongDoubleToLimits(prodWeights(nextNote, possibleNextNotes) * reciporicalSumOfProdWeights) };

        const auto intervalSize{ (nextNote == 0 || nextNote == currentNote || exponent * rollingWeight <= weightCutoff)
            ? getInterval(currentNote, 0).getSize()
            : getInterval(currentNote, nextNote).getSize() *
                  traversePath(nextNoteIndex - (currentNoteIndex < nextNoteIndex ? 1 : 0),
                               nextPossibleNextNotes,
                               clampLongDoubleToLimits(exponent * rollingWeight),
                               nextReciporicalSumProdWeights)
        };

        returnValue *= std::pow(intervalSize, exponent);
    }

    return returnValue;
}

std::vector<long double> Scale::makePopulatedTuning() const
{
    auto percentTuned{ [this](const int& note) -> long double
        {
            return 100 * (long double)note / (long double)size();
        }
    };

    std::vector<long double> tuning;
    tuning.reserve(size());

    long double lastPercentage{ 0 };
    const long double loadingInterval{ 0.1 };

    std::cout << "Tuning " << name << std::endl << std::endl;
    std::cout << std::fixed << std::setprecision(1) << "Progress: 0.0% \r";

    for (auto note{ 0 }; note != size(); ++note)
    {
        tuning.push_back(tuneNote(note));

        const auto percentage{ percentTuned(note) };

        if (percentage - lastPercentage >= loadingInterval)
        {
            std::cout << "Progress: " << percentage << "% \r";
            lastPercentage = percentage;
        }
    }

    std::cout << "Progress: 100.0% \r\n" << std::endl;

    return tuning;
}

void Scale::adjustTuningRootNote(std::vector<long double>& tuning,
                                 const int& rootNoteIndex) const
{
    const auto rootTuning{ tuning[rootNoteIndex] };
    for (auto& note : tuning)
        note /= rootTuning;
}

std::vector<float> Scale::insertDummyNotes(const std::vector<long double>& tuning) const
{
    auto insertionAdjustment{ 0 };

    std::vector<float> tuningWithDummies(tuning.begin(), tuning.end());
    tuningWithDummies.reserve(tuningWithDummies.size() + dummyIndecies.size());

    for (const auto& index : dummyIndecies)
        if (index + insertionAdjustment >= 0 && index < tuningWithDummies.size())
        {
            tuningWithDummies.insert(tuningWithDummies.begin() + index + insertionAdjustment, std::numeric_limits<double>::quiet_NaN());

            ++insertionAdjustment;
        }

    return tuningWithDummies;
}

void Scale::normaliseWeights()
{
    long double maxWeight{ getMaxWeight() };

    for (auto& row : intervalsPattern)
        for (auto& Interval : row)
            Interval.setWeight(Interval.getWeight() / maxWeight);
}