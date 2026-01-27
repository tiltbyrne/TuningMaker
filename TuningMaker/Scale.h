#pragma once
#include "Fraction.h"
#include "Utilities.h"
#include <limits>

/*
  Musically, an interval between two notes is the factor you need to multiply one note by to a arrive
  at the other. Mathematically, this number is the interval's size, while it's weight represents how
  "important" it is (howver that is interpreted).
*/
class Interval
{
public:
    /*
      Constructs an interval of size and weight = 1;
    */
    Interval();

    /*
      Constructs an interval of size s and weight w. These values are clamped to limits if they exceed
      numeric limits.
    */
    Interval(const long double& s, const long double& w = 1);

    /*
      Returns the size of the interval.
    */
    inline long double getSize() const;

    /*
      Returns the weight of the interval.
    */
    inline long double getWeight() const;

    /*
      Sets the size of the interval and clamps it's value to numeric limits if necassary.
    */
    void setSize(const long double& newSize);
    
    /*
      Sets the size of the interval, clamps it's value to numeric limits, and manages weight = 0 if necassary.
    */
    void setWeight(const long double& newWeight);

    /*
      Sets both the size and weight of the interval.
    */
    void setInterval(const long double& newSize, const long double& newWeight);

private:
    /*
      The distance between two notes.
    */
    long double size;
    /*
      The "importance" of the interval.
    */
    long double weight;

    /*
      Weights <= 0, which are impossible accordin to the model of tuning used as they can lead to division by
      0, are set to the numeric limits lowest value for long double.
    */
    void manageZeroWeight();
};

using IntervalsPattern = std::vector<std::vector<Interval>>;

/*
  A Scale represents a collection of notes as the ideal pattern intervals between those notes.
  It also contains the logic necessary to produce a tuning of itself, output by tuneScale().
  It is beyond the scope the documentation of this class to discuss exactly how this tuning
  is calculated, but it involves treating scales as complete weighted graphs. It may also
  contain some 'dummy' notes, which are not contained in the scale but are contained in the
  pitch space containing it (think black notes in the C major scale).
*/
class Scale
{
public:
    /*
      Constructs a nameless scale with a default intervals pattern.
    */
    Scale();

    /*
      Constructs a named scale with a default intervals pattern.
    */
    Scale(const std::string& n);

    /*
      Constructs a nameless scale with intervals pattern i. If i has non-triangular dimensions
      (defined by patternHasTriangularDimensions() function in Utilities.h) then the pattern is
      default
    */
    Scale(const IntervalsPattern& i);

    /*
       Constructs a named scale with intervals pattern i. If i has non-triangular dimensions
       (defined by patternHasTriangularDimensions() function in Utilities.h) then the pattern is
       default.
    */
    Scale(const IntervalsPattern& i, const std::string& n);

    /*
      Returns the number of notes in the scale.
    */
    inline size_t size() const;

    /*
      Sets intervalsPattern to newIntervalsPattern if it has triangular dimensions, (defined by
      patternHasTriangularDimensions() function in Utilities.h).
    */
    void setIntervalsPattern(const IntervalsPattern& newIntervalsPattern);

    /*
      Sets the dummy notes of the scale. In tuning of the scale produced by tuneScale, dummy note
      tunings will be inserted at these indecies, which are represented by NaN values. Note that
      the indecies of dummy notes are independent of intervalsPattern and invalid dummy indecies
      will not be inserted into the final tuning.
    */
    void setDummyIndecies(const std::vector<int>& newDummyIntervals);

    /*
      Resets the name of the scale.
    */
    void setName(const std::string& newName);

    /*
      Returns the name of the scale if it has one.
    */
    inline std::string getName() const;

    /*
      Returns the smallest weight of all intervals in the scale. 
    */
    long double getMinWeight() const;

    /*
      Returns the greatest weight of all intervals in the scale.
    */
    long double getMaxWeight() const;

    /*
      Produces a tuning of the scale. The tuning of the note at index = trueRootNote will always equal 1f
      Depending on the size of the scale and the weights of it's intervals weightCutoff can have a large
      influence on the time it takes for this function to return.
    */
    std::vector<double> tuneScale(const int& trueRootNote, const long double& weightCutoff = 0) const;

private:
    /*
      The ideal intervals between all notes in the scale. The interval between notes A and B is equal to
      the reciporical of the interval between B and A. The weight of both intervals is the same. So,
      we only need to store the interval between A and B for any value of A or B.
    */
    IntervalsPattern intervalsPattern;
    /*
      The indecies of intervals which will always be tuned NaN in tuneScale().
    */
    std::vector<int> dummyIndecies;
    /*
      The name of the scale;
    */
    std::string name;

    /*
      Accesses or calculates the value of the interval from noteFrom to noteTo depending on whether or not
      it is contained in intervalsPattern.
    */
    Interval getInterval(const int& noteTo, const int& noteFrom) const;

    /*
      Returns the sum of all notes in notesFrom to noteTo. This is a useful value for tuning calculation.
    */
    long double sumWeights(const int& noteTo, std::vector<int>& notesFrom) const;

    /*
      Calculates the tuning of a single note for a scale, assuming a single rootNote.
    */
    long double makeTuning(const int& rootNote, int& note, const long double& weightCutoff) const;

    /*
      Iteratively traverses across the scale as if it were a graph. Iteration is broken by either finding a
      path which originates at rootNote, or arriving at a path whose rollingWeight <= weightCutoff. Being that
      this function is called many times, it could be a sensible place to begin optimisation.
    */
    long double traverseScale(int& lastNote, std::vector<int>& possibleNextNotesInPath, const int& rootNote,
                              const long double& rollingWeight, const long double& weightCutoff,
                              const long double& possibleWeightsToNoteSum) const;
    
    /*
      Manages calls to makeTuning() for all possible notes and rootNotes, populates size() number of tunings
      for each note in the scale, and tracks progress of this calculation.
    */
    std::vector<std::vector<long double>> makePopulatedTunings(const long double& weightCutoff) const;

    /*
      Produces a tuning of the scale from the tunings produced by makePopulatedTunings(), normalised and averaged
      such that the tuning of the note at index trueRootNote equals 1f
    */
    std::vector<double> normaliseTuningsAndMakeAverageTuning(std::vector<std::vector<long double>>& tunings,
                                                                  const int& trueRootNote) const;

    /*
      Inserts NaN at the indecies contained in dummyIndecies if those intervals are valid.
    */
    std::vector<double> insertDummyNotes(std::vector<double>& tuning) const;

    /*
      Normalises the weights of all intervals in intervalsPattern to a range of (0, 1].
    */
    void normaliseWeights();
};


//All functions in this namespace should return an IntervalPattern which can be used by Scale objects.
namespace IntervalPatternMakers
{
    /*
      Produces an IntervalsPattern where all intervals have a weight equal to one over the product of the
      numerator and denominator (their Tenney Weight), raised to the power of entropyCurve.
    */
    static IntervalsPattern
        rangedScaleFractionsToIntervalsWithTenneyWeight(const std::vector<std::vector<Fraction>>& rangedScale,
            const long double& entropyCurve = 0)
    {
        IntervalsPattern pattern;
        pattern.reserve(rangedScale.size());

        long double maxWeight{ 0 };
        for (auto rowItr{ rangedScale.begin() }; rowItr != rangedScale.end(); ++rowItr)
        {
            std::vector<Interval> intervalsRow;

            intervalsRow.reserve(rowItr->size());
            for (auto fractionItr{ rowItr->begin() }; fractionItr != rowItr->end(); ++fractionItr)
            {
                const auto weight{ tenneyHeightOfFraction(*fractionItr, entropyCurve) };
                intervalsRow.push_back({ fractionItr->toLongDouble(), weight });

                if (weight > maxWeight)
                    maxWeight = weight;
            }

            pattern.push_back(intervalsRow);
        }

        return pattern;
    }

    /*
      Produces an IntervalsPattern where all intervals have equal weight.
    */
    static IntervalsPattern
        rangedScaleLongDoubleToIntervalsWithUniformWeight(const std::vector<std::vector<long double>>& rangedScale)
    {
        IntervalsPattern pattern;
        pattern.reserve(rangedScale.size());

        for (auto rowItr{ rangedScale.begin() }; rowItr != rangedScale.end(); ++rowItr)
        {
            std::vector<Interval> intervalsRow;

            intervalsRow.reserve(rowItr->size());
            for (auto fractionItr{ rowItr->begin() }; fractionItr != rowItr->end(); ++fractionItr)
                intervalsRow.push_back(*fractionItr);

            pattern.push_back(intervalsRow);
        }

        return pattern;
    }
}