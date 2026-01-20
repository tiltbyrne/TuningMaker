#include "PitchSpace.h"

PitchSpace::PitchSpace(const std::vector<Fraction>& t)
    : table(t)
{
}

inline size_t PitchSpace::size() const
{
    return table.size();
}

inline Fraction PitchSpace::fractionOfRepitition() const
{
    return table[size() - 1];
}

Fraction PitchSpace::getFraction(const int& noteTo, const int& noteFrom) const
{
    const int intervalSize{ noteTo - noteFrom };

    if (intervalSize == 0)
        return { 1 };

    const auto returnFraction{ table[(std::abs(intervalSize) - 1) % size()] * //fraction part
                               fractionOfRepitition().power((std::abs(intervalSize) - 1) / size()) };//repeat part

    return intervalSize > 0 ? returnFraction : returnFraction.reciporical();
}

std::vector<std::vector<Fraction>> PitchSpace::makeRangedScaleFractions(const std::string& scaleSigniature, const int& range) const
{
    const auto signiatureNameReturnValue{ getSigniature(scaleSigniature) };
    if (range <= 1 || !signiatureNameReturnValue.has_value())
        return { { 1 } };

    return makeRangedScaleFractions(signiatureNameReturnValue.value(), range);
}

std::vector<std::vector<Fraction>> PitchSpace::makeRangedScaleFractions(const ScaleSigniature& signiature, const int& range) const
{
    std::vector<std::vector<Fraction>> extendedScaleFractions;
    extendedScaleFractions.reserve(range);

    for (auto noteFrom{ 0 }; noteFrom != range - 1; ++noteFrom)
    {
        extendedScaleFractions.push_back({});
        const auto repetitionNoteFrom{ signiature[noteFrom % signiature.size()] + size() * (noteFrom / signiature.size()) };

        for (auto noteTo{ noteFrom + 1 }; noteTo != range; ++noteTo)
            extendedScaleFractions[noteFrom].push_back(getFraction(signiature[noteTo % signiature.size()] + size() * (noteTo / signiature.size()),
                repetitionNoteFrom));
    }

    return extendedScaleFractions;
}

void PitchSpace::addSigniature(const std::string& signiatureName, const ScaleSigniature& signiatureIntervals)
{
    scaleSigniatures.insert_or_assign(signiatureName, signiatureIntervals);
}

void PitchSpace::removeSigniature(const std::string& signiatureName)
{
    scaleSigniatures.erase(signiatureName);
}

std::optional<ScaleSigniature> PitchSpace::getSigniature(const std::string& signiatureName) const
{
    const auto iterator{ scaleSigniatures.find(signiatureName) };

    if (iterator != scaleSigniatures.end())
        return iterator->second;

    return std::nullopt;
}