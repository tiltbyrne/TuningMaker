#pragma once
#include "Fraction.h"
#include "Scale.h"
#include <string>
#include <map>
#include <optional>
#include <algorithm>

using ScaleSigniature = std::vector<int>;

template<typename Relation>
class PitchSpace
{
public:
    PitchSpace(const std::vector<Relation>& t)
        : table(t)
    { }

    inline size_t size() const
    {
        return table.size();
    }

    inline Relation relationOfRepitition() const
    {
        return table[size() - 1];
    }

    Relation getRelation(const int& noteTo, const int& noteFrom) const
    {
        const int intervalSize{ noteTo - noteFrom };

        if (intervalSize == 0)
            return { 1 };

        const auto returnFraction{ table[(std::abs(intervalSize) - 1) % size()] * //fraction part
                                   std::pow(relationOfRepitition(), (std::abs(intervalSize) - 1) / size()) };//repeat part

        return intervalSize > 0 ? returnFraction : returnFraction.reciporical();
    }

    std::optional<std::vector<std::vector<Relation>>> makeRangedScaleRelations(const std::string& scaleSigniature, const int& range) const
    {
        const auto signiatureNameReturnValue{ getSigniature(scaleSigniature) };
        if (range <= 1 || !signiatureNameReturnValue.has_value())
            return std::nullopt;

        return makeRangedScaleRelations(signiatureNameReturnValue.value(), range);
    }

    std::vector<std::vector<Relation>> makeRangedScaleRelations(const ScaleSigniature& signiature, const int& range) const
    {
        std::vector<std::vector<Fraction>> extendedScaleFractions;
        extendedScaleFractions.reserve(range);


        for (auto noteFrom{ 0 }; noteFrom != range - 1; ++noteFrom)
        {
            extendedScaleFractions.push_back({});
            extendedScaleFractions[noteFrom].reserve(range - noteFrom - 1);
            const auto repetitionNoteFrom{ noteInSigniature(signiature, noteFrom) };

            for (auto noteTo{ noteFrom + 1 }; noteTo != range; ++noteTo)
                extendedScaleFractions[noteFrom].push_back(getRelation(noteInSigniature(signiature, noteTo),
                                                                       repetitionNoteFrom));
        }

        return extendedScaleFractions;
    }

    std::vector<int> populateDummyIndecies(const std::string& scaleSigniature, const int& range) const
    {
        const auto signiatureNameReturnValue{ getSigniature(scaleSigniature) };
        if (range <= 1 || !signiatureNameReturnValue.has_value())
            return {};

        return populateDummyIndecies(signiatureNameReturnValue.value(), range);
    }

    std::vector<int> populateDummyIndecies(const ScaleSigniature& signiature, const int& range) const
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

    void addSigniature(const std::string& signiatureName, ScaleSigniature signiatureIntervals)
    {
        std::sort(signiatureIntervals.begin(), signiatureIntervals.end());
        scaleSigniatures.insert_or_assign(signiatureName, signiatureIntervals);
    }

    void removeSigniature(const std::string& signiatureName)
    {
        scaleSigniatures.erase(signiatureName);
    }

    std::optional<ScaleSigniature> getSigniature(const std::string& signiatureName) const
    {
        const auto iterator{ scaleSigniatures.find(signiatureName) };

        if (iterator != scaleSigniatures.end())
            return iterator->second;

        return std::nullopt;
    }

private:
    const std::vector<Relation> table;
    std::map<std::string, ScaleSigniature> scaleSigniatures;

    int noteInSigniature(const ScaleSigniature& signiature, const int& note) const
    {
        return signiature[note % signiature.size()] + size() * (note / signiature.size());
    }
};

template<>
Fraction PitchSpace<Fraction>::getRelation(const int& noteTo, const int& noteFrom) const
{
    const int intervalSize{ noteTo - noteFrom };

    if (intervalSize == 0)
        return { 1 };

    const auto returnFraction{ table[(std::abs(intervalSize) - 1) % size()] * //fraction part
                               relationOfRepitition().power((std::abs(intervalSize) - 1) / size()) };//repeat part

    return intervalSize > 0 ? returnFraction : returnFraction.reciporical();
}

namespace PitchSpaces
{
    static std::map<std::string, PitchSpace<Fraction>> fractional
    { 
        {"seven EDO",
        { { { 10, 9 },
            { 11, 9 },
            { 4, 3 },
            { 3, 2 },
            { 13, 8 },
            { 9, 5 },
            { 2, 1 }
        } } },
        {"twelve EDO",
        { { { 17, 16 },
            { 9, 8 },
            { 6, 5 },
            { 5, 4 },
            { 4, 3 },
            { 7, 5 },
            { 3, 2 },
            { 8, 5 },
            { 5, 3 },
            { 9, 5 },
            { 15, 8 },
            { 2, 1 }
        } } },
        {"seventeen EDO",
        { { { 24, 23 },
            { 12, 11 },
            { 8, 7 },
            { 7, 6 },
            { 11, 9 },
            { 9, 7 },
            { 4, 3 },
            { 11, 8 },
            { 13, 9 },
            { 3, 2 },
            { 11, 7 },
            { 13, 8 },
            { 12, 7 },
            { 7, 4 },
            { 11, 6 },
            { 23, 12 },
            { 2, 1 }
        } } },
        {"twenty-two EDO",
        { { { 32, 31 },
            { 15, 14 },
            { 8, 7 },
            { 11, 10 },
            { 11, 9 },
            { 9, 7 },
            { 4, 3 },
            { 11, 8 },
            { 13, 9 },
            { 3, 2 },
            { 11, 7 },
            { 13, 8 },
            { 12, 7 },
            { 7, 4 },
            { 11, 6 },
            { 23, 12 },
            { 2 }
        } } },
    };
}