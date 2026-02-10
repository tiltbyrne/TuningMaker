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
    Scale(const long double& c = 0);

    /*
      Constructs a named scale with a default intervals pattern.
    */
    Scale(const std::string& n, const long double& c = 0);

    /*
      Constructs a nameless scale with intervals pattern i. If i has non-triangular dimensions
      (defined by patternHasTriangularDimensions() function in Utilities.h) then the pattern is
      default
    */
    Scale(const IntervalsPattern& i, const long double& c = 0);

    /*
       Constructs a named scale with intervals pattern i. If i has non-triangular dimensions
       (defined by patternHasTriangularDimensions() function in Utilities.h) then the pattern is
       default.
    */
    Scale(const IntervalsPattern& i, const std::string& n, const long double& c = 0);

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
    void setDummyIndecies(const std::vector<int>& newDummyIndecies);

    /*
      Resets the name of the scale.
    */
    void setName(const std::string& newName);

    /*
      Returns the name of the scale if it has one.
    */
    inline std::string getName() const;

    /*
      Returns the weight at which scale traversal will be cut off.
    */
    long double getWeightCutoff() const;

    /*
      Sets the weight at which scale traversal will be cut off.
    */
    void setWeightCutoff(const long double& newWeightCutoff);

    /*
      Returns the smallest weight of all intervals in the scale. 
    */
    long double getMinWeight() const;

    /*
      Returns the greatest weight of all intervals in the scale.
    */
    long double getMaxWeight() const;

    /*
      Produces a tuning of the scale. The tuning of the note at index = rootNote will always equal 1f
      Depending on the size of the scale and the weights of it's intervals weightCutoff can have a large
      influence on the time it takes for this function to return.
    */
    std::vector<float> tuneScale(const int& rootNote) const;

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
      The total weight of an interval tuning a note at which scale traversal will be halted and that interval
      will be replaced by an interval from note 0 to the note at the index of that interval.
    */
    long double weightCutoff;

    /*
      Accesses or calculates the value of the interval from noteFrom to noteTo depending on whether or not
      it is contained in intervalsPattern.
    */
    Interval getInterval(const int& noteTo, const int& noteFrom) const;

    /*
      Returns the product of all notes in notesFrom to noteTo.
    */
    long double prodWeights(const int& noteFrom, const std::vector<int>& notesTo) const;

    /*
      Returns the sum of the product of all notes in notesFrom to eachother.
    */
    long double sumProdWeights(const std::vector<int>& notesFrom) const;

    /*
      Calculates the tuning of a single note for a scale, assuming a single rootNote.
    */
    long double tuneNote(int& note) const;

    /*
      Iteratively traverses across the scale as if it were a graph. Iteration is halted by either finding a
      path which originates at 0, or arriving at a path whose rollingWeight < weightCutoff. Being that
      this function is called many times, it could be a sensible place to begin optimisation.
    */
    long double traversePath(const int& currentNoteIndex, const std::vector<int>& possibleNextNotes,
                             const long double& rollingWeight, const long double& reciporicalSumOfProdWeights) const;
    
    /*
      Manages calls to makeTuning() for all notes and tracks progress of this calculation.
    */
    std::vector<long double> makePopulatedTuning() const;

    /*
      Adjusts the relationships in the tuning such that the tuning of the note in tuning at
      rootNoteIndex equals 1.
    */
    void adjustTuningRootNote(std::vector<long double>& tuning,
                              const int& rootNoteIndex) const;

    /*
      Inserts NaN at the indecies contained in dummyIndecies if those intervals are valid.
    */
    std::vector<float> insertDummyNotes(const std::vector<long double>& tuning) const;

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