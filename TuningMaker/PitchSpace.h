#pragma once
#include "Fraction.h"
#include "Scale.h"
#include <string>
#include <map>
#include <optional>
#include <algorithm>

using ScaleSigniature = std::vector<int>;

/*
  A PitchSpace represents the total pitch materials which a scale could be constructed from (think of all
  the different scales "contained" in the chromatic 12edo pitch space). Pitch space stores this information
  as an array of Relations, as well as a map of named scale signiatures, which store the indecies of the
  notes in the pitch space from which that scale is constructed.
*/
template<typename Relation>
class PitchSpace
{
public:
    /*
      Constructs a pitch space out of an array of Relations. Table then has duplicate values and
      values equal to 1 erased, and is sorted. Also, a scale named "full" is added to the space,
      whose signiature contains all notes in the pitch space.
    */
    PitchSpace(const std::vector<Relation>& t)
        : table(t)
    {
        std::sort(table.begin(), table.end());

        if (table[0] == Relation(1))
            table.erase(table.begin());

        ScaleSigniature fullSigniature(t.size());
        std::iota(fullSigniature.begin(), fullSigniature.end(), 0);

        addSigniature("full", fullSigniature);
    }

    /*
      The number of notes in the pitch space.
    */
    inline size_t size() const
    {
        return table.size();
    }

    /*
      The final value of the relations table, powers of which will are multiplied are used in the
      calculation of relations bewteeen notes outside of the range of the pitch space table
    */
    inline Relation relationOfRepetition() const
    {
        return table[size() - 1];
    }

    /*
      Calculates the Relation between noteFrom and noteTo. If the size of this relation is greater
      than the range of the pitch space table this relation is derived from the modulo base size()
      of it's size, multiplied by the pitch space repetition it occupies
    */
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
            const auto returnRelation{ relationPart * relationOfRepetition().power(exponent) };

            return intervalSizeGreaterThanZero ? returnRelation : returnRelation.reciporical();
        }
        else
        {
            const auto returnRelation{ relationPart * std::pow(relationOfRepetition(), exponent) };

            return intervalSizeGreaterThanZero ? returnRelation : 1 / returnRelation;
        }
    }

    /*
      Makes a triangular matrix of size = range of the relations betwee notes in the scale at
      scale name if that scale exists.
    */
    std::optional<std::vector<std::vector<Relation>>> makeRangedScaleRelations(const std::string& signiatureName, const int& range) const
    {
        const auto signiatureNameReturnValue{ getSigniature(signiatureName) };
        if (range <= 1 || !signiatureNameReturnValue.has_value())
            return std::nullopt;

        return makeRangedScaleRelations(signiatureNameReturnValue.value(), range);
    }
    
    /*
      Makes a triangular matrix of size = range of the relations betwee notes in signiature.
      Relations between notes that exceed the range of the pitch space table are calculated
      by getRelation();
    */
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

    /*
      Returns a vector of indecies NOT in the named signiature extended to range if that scale exists.
    */
    std::vector<int> getDummyIndecies(const std::string& signiatureName, const int& range) const
    {
        const auto signiatureNameReturnValue{ getSigniature(signiatureName) };
        if (range <= 1 || !signiatureNameReturnValue.has_value())
            return {};

        return populateDummyIndecies(signiatureNameReturnValue.value(), range);
    }

    /*
      Adds a scale named signiatureName to scaleSigniatures. It's signiature note indecies
      are wrapped around the pitch space's size, has duplicate values removed, and is sorted.
    */
    void addSigniature(const std::string& signiatureName, ScaleSigniature signiatureIntervals)
    {
        for (auto& interval : signiatureIntervals)
			interval = interval % size();

        std::sort(signiatureIntervals.begin(), signiatureIntervals.end());

        auto itr{ std::unique(signiatureIntervals.begin(), signiatureIntervals.end()) };

        signiatureIntervals.erase(itr, signiatureIntervals.end());

        scaleSigniatures.insert_or_assign(signiatureName, signiatureIntervals);
    }

    /*
      Erases the scale at name if it exists.
    */
    void removeSigniature(const std::string& signiatureName)
    {
        if (scaleSigniatures.find(signiatureName) != scaleSigniatures.end())
            scaleSigniatures.erase(signiatureName);
    }

    /*
      Returns the table of relations.
    */
    std::vector<Relation> getTable() const
    {
        return table;
    }

    /*
      Returns the scale signiature at name if it exists.
    */
    std::optional<ScaleSigniature> getSigniature(const std::string& signiatureName) const
    {
        const auto iterator{ scaleSigniatures.find(signiatureName) };

        if (iterator != scaleSigniatures.end())
            return iterator->second;

        return std::nullopt;
    }

    /*
      Prints the scale signiature at scaleName if it exists in the form:
      [name] - 0 (Relation(1)) 1 (table[0]) 2 (table[1]) etc.
    */
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

            //std::cout << "(" << size() << ") " << relationOfRepetition();
        }
    }

    /*
      Prints all scale signiatures in the form:
      [name] - 0 (Relation(1)) 1 (table[0]) 2 (table[1]) etc.
    */
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

            //std::cout << "(" << size() << ") " << relationOfRepetition();

            std::cout << std::endl << std::endl;
        }
    }

private:
    /*
      Contains all relations between notes at a given distance which defines the pitch space.
    */
    std::vector<Relation> table;

    /*
      Stores named scales which are contained in the pitch space.
    */
    std::map<std::string, ScaleSigniature> scaleSigniatures;

    /*
      Returns the index of note in the scale signiature even if that note exceeds the index of the
      signiature by imagining where it would be if the pattern of the signiature extended to include
      that note.
    */
    int noteInSigniature(const ScaleSigniature& signiature, const int& note) const
    {
        return signiature[note % signiature.size()] + size() * (note / signiature.size());
    }

    /*
      Returns the index of all intervals which are not contained in makeRangedScaleRelations(signiature, range)
      but are less than range.
    */
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

/*
  Preloaded standard pitch spaces.
*/
namespace PitchSpaces
{
    /*
      Standard pitch spaces with ideal fractional relations.
    */
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
        {"15edo",
        { { { 21, 20 },
            { 11, 10 },
            { 8, 7 },
            { 6, 5 },
            { 5, 4 },
            { 4, 3 },
            { 11, 8 },
            { 16, 11 },
            { 3, 2 },
            { 8, 5 },
            { 5, 3 },
            { 7, 4 },
            { 11, 6 },
            { 21, 11 },
            { 2 }
        } } },
    };

    /*
      Standard pitch spaces with ideal decimal (long double) relations.
    */
    static std::map<std::string, PitchSpace<long double>> decimal;

    /*
      Prints all pitch spaces in the form:
      [name] - 0 (Relation(1)) 1 (table[0]) 2 (table[1]) etc.
    */
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
    
    /*
      Adds some scales to pitch spaces.
    */
    static void initialisePitchSpaceScales()
    {
        auto& sevenEDO{ PitchSpaces::fractional.at("7edo") };

        sevenEDO.addSigniature("neutral pentatonic A", { 0, 1, 3, 4, 6 });
        sevenEDO.addSigniature("neutral pentatonic B", { 0, 2, 3, 5, 6 });

        auto& twelveEDO{ PitchSpaces::fractional.at("12edo") };

        twelveEDO.addSigniature("major pentatonic", { 0, 2, 4, 7, 9 });
        twelveEDO.addSigniature("minor pentatonic", { 0, 3, 4, 7, 10 });
        twelveEDO.addSigniature("ionian", { 0, 2, 4, 5, 7, 9, 11 });
        twelveEDO.addSigniature("dorian", { 0, 2, 3, 5, 7, 9, 10 });
        twelveEDO.addSigniature("phrygian", { 0, 1, 3, 5, 7, 8, 10 });
        twelveEDO.addSigniature("lydian", { 0, 2, 4, 6, 7, 9, 11 });
        twelveEDO.addSigniature("myxolydian", { 0, 2, 4, 5, 7, 9, 10 });
        twelveEDO.addSigniature("aolian", { 0, 2, 3, 5, 7, 8, 10 });
        twelveEDO.addSigniature("locrian", { 0, 1, 3, 5, 6, 8, 10 });

        auto& twentytwoEDO{ PitchSpaces::fractional.at("22edo") };

        twentytwoEDO.addSigniature("orwell9", { 0, 3, 5, 8, 10, 13, 15, 18, 20 });
    }
}