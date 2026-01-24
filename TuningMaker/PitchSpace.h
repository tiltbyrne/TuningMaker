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
    {
        std::sort(table.begin(), table.end());

        ScaleSigniature fullSigniature(t.size());
        std::iota(fullSigniature.begin(), fullSigniature.end(), 0);

        addSigniature("full", fullSigniature);
    }

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

        const auto& relationPart{ table[(std::abs(intervalSize) - 1) % size()] };

        const auto exponent{ (std::abs(intervalSize) - 1) / size() };

        const auto intervalSizeGreaterThanZero{ intervalSize > 0 };

        if constexpr (std::is_same_v<Relation, Fraction>)
        {
            const auto returnRelation{ relationPart * relationOfRepitition().power(exponent) };

            return intervalSizeGreaterThanZero ? returnRelation : returnRelation.reciporical();
        }
        else
        {
            const auto returnRelation{ relationPart * std::pow(relationOfRepitition(), exponent) };

            return intervalSizeGreaterThanZero ? returnRelation : 1 / returnRelation;
        }
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
        std::vector<std::vector<Relation>> extendedScaleFractions;
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

    std::vector<int> getDummyIndecies(const std::string& scaleSigniature, const int& range) const
    {
        const auto signiatureNameReturnValue{ getSigniature(scaleSigniature) };
        if (range <= 1 || !signiatureNameReturnValue.has_value())
            return {};

        return populateDummyIndecies(signiatureNameReturnValue.value(), range);
    }

    void addSigniature(const std::string& signiatureName, ScaleSigniature signiatureIntervals)
    {
        std::sort(signiatureIntervals.begin(), signiatureIntervals.end());

        for (auto& interval : signiatureIntervals)
			interval = interval % size();

        auto itr{ std::unique(signiatureIntervals.begin(), signiatureIntervals.end()) };

        signiatureIntervals.erase(itr, signiatureIntervals.end());

        scaleSigniatures.insert_or_assign(signiatureName, signiatureIntervals);
    }

    void removeSigniature(const std::string& signiatureName)
    {
        scaleSigniatures.erase(signiatureName);
    }


    std::vector<Relation> getTable() const
    {
        return table;
    }

    std::optional<ScaleSigniature> getSigniature(const std::string& signiatureName) const
    {
        const auto iterator{ scaleSigniatures.find(signiatureName) };

        if (iterator != scaleSigniatures.end())
            return iterator->second;

        return std::nullopt;
    }

    void printSigniature(const std::string& scaleName) const
    {
        if (scaleSigniatures.find(scaleName) != scaleSigniatures.end())
        {
            std::cout << "[" << scaleName << "] - ";
            for (auto const& index : scaleSigniatures.at(scaleName))
            {
                std::cout << index << " (";
                if (index == 0)
                    std::cout << Relation(1) << ") ";
                else
                    std::cout << table[index - 1] << ") ";
            }

            std::cout << '\b';
        }
    }

    void printSigniatures() const
    {
        for (auto const& signiature : scaleSigniatures)
        {
            std::cout << "[" << signiature.first << "] - ";
            for (auto const& index : signiature.second)
            {
                std::cout << index << " (";
                if (index == 0)
                    std::cout << Relation(1) << ") ";
                else
                    std::cout << table[index - 1] << ") ";
            }

            std::cout << std::endl << std::endl;
        }

        std::cout << '\b';
    }

private:
    std::vector<Relation> table;
    std::map<std::string, ScaleSigniature> scaleSigniatures;

    int noteInSigniature(const ScaleSigniature& signiature, const int& note) const
    {
        return signiature[note % signiature.size()] + size() * (note / signiature.size());
    }

    std::vector<int> populateDummyIndecies(const ScaleSigniature& signiature, const int& range) const
    {
        std::vector<int> dummyIndecies;

        std::vector<int> rangedSigniature;
        rangedSigniature.reserve(range);

        for (auto note{ 0 }; note != range; ++note)
            rangedSigniature.push_back(noteInSigniature(signiature, note));

        auto dummyIndex{ 0 };

        for (auto itr{ rangedSigniature.begin() }; itr != std::prev(rangedSigniature.end()); ++itr)
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
};

namespace PitchSpaces
{
    static std::map<std::string, PitchSpace<Fraction>> fractional
    {
        {"7edo",
        { { { 10, 9 },
            { 11, 9 },
            { 4, 3 },
            { 3, 2 },
            { 13, 8 },
            { 9, 5 },
            { 2 }
        } } },
        {"12edo",
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
            { 2 }
        } } },
        {"17edo",
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
            { 2 }
        } } },
        {"22edo",
        { { { 32, 31 },
            { 16, 15 },
            { 8, 7 },
            { 11, 10 },
            { 9, 8 },
            { 7, 6 },
            { 6, 5 },
            { 5, 4 },
            { 9, 7 },
            { 4, 3 },
            { 11, 8 },
            { 7, 5 },
            { 16, 11 },
            { 3, 2 },
            { 11, 7 },
            { 8, 5 },
            { 5, 3 },
            { 12, 7 },
            { 7, 4 },
            { 9, 5 },
            { 15, 8 },
            { 31, 16 },
            { 2 }
        } } },
    };

    static std::map<std::string, PitchSpace<long double>> decimal;

    template<typename Relation>
    bool typeContainingPitchSpace(const PitchSpace<Relation>& pitchClasses, const std::string& pitchSpaceName)
    {
        if (fractional.find(pitchSpaceName) != fractional.end())
            return fractional;

        if (decimal.find(pitchSpaceName) != decimal.end())
            return decimal;

        return std::nullopt;
	}

    template<typename Map>
    static void printPitchSpaces(const Map& pitchSpaces)
    {
        for (auto const& space : pitchSpaces)
        {
            std::cout << "[" << space.first << "] - ";

            for (auto const& relation : space.second.getTable())
                std::cout << relation << "  ";

			std::cout << std::endl << std::endl;
        }
    }
}