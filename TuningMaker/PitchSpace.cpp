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

std::optional<std::vector<std::vector<Fraction>>> PitchSpace::makeRangedScaleFractions(const std::string& scaleSigniature, const int& range) const
{
    const auto signiatureNameReturnValue{ getSigniature(scaleSigniature) };
    if (range <= 1 || !signiatureNameReturnValue.has_value())
        return std::nullopt;

    return makeRangedScaleFractions(signiatureNameReturnValue.value(), range);
}

std::vector<std::vector<Fraction>> PitchSpace::makeRangedScaleFractions(const ScaleSigniature& signiature, const int& range) const
{
    std::vector<std::vector<Fraction>> extendedScaleFractions;
    extendedScaleFractions.reserve(range);


    for (auto noteFrom{ 0 }; noteFrom != range - 1; ++noteFrom)
    {
        extendedScaleFractions.push_back({});
        extendedScaleFractions[noteFrom].reserve(range - noteFrom - 1);
        const auto repetitionNoteFrom{ noteInSigniature(signiature, noteFrom) };

        for (auto noteTo{ noteFrom + 1 }; noteTo != range; ++noteTo)
            extendedScaleFractions[noteFrom].push_back(getFraction(noteInSigniature(signiature, noteTo),
                                                                   repetitionNoteFrom));
    }

    return extendedScaleFractions;
}

std::vector<int> PitchSpace::populateDummyIndecies(const std::string& scaleSigniature, const int& range) const
{
    const auto signiatureNameReturnValue{ getSigniature(scaleSigniature) };
    if (range <= 1 || !signiatureNameReturnValue.has_value())
        return {};

    return populateDummyIndecies(signiatureNameReturnValue.value(), range);
}

std::vector<int> PitchSpace::populateDummyIndecies(const ScaleSigniature& signiature, const int& range) const
{
    std::vector<int> dummyIndecies;
    
    std::vector<int> rangedSigniature;
    rangedSigniature.reserve(range);

    for (auto note{ 0 }; note != range; ++note)
        rangedSigniature.push_back(noteInSigniature(signiature, note));

    auto dummyIndex{ 0 };

    for (auto itr{ rangedSigniature.begin() }; itr != std::prev(rangedSigniature.end(), 2); ++itr)
    {
        auto note{ 1 };
        while (note != *std::next(itr) - *itr)
        {
            dummyIndecies.push_back(1 + dummyIndex);
            ++note;
        }

        ++dummyIndex;
    }
    
    return dummyIndecies;
}

void PitchSpace::addSigniature(const std::string& signiatureName, ScaleSigniature signiatureIntervals)
{
    std::sort(signiatureIntervals.begin(), signiatureIntervals.end());
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

int PitchSpace::noteInSigniature(const ScaleSigniature& signiature, const int& note) const
{
    return signiature[note % signiature.size()] + size() * (note / signiature.size());
}