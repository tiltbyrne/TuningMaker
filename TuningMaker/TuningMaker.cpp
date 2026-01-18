#include <iostream>
#include <array>
#include <vector>
#include <numeric>
#include <cmath>

/*
* using u_short = unsigned short;

static constexpr u_short scaleSize{ 4 };

static constexpr u_short tunedNotes{ scaleSize - 1 };

static constexpr size_t factorial(const size_t& i)
{
    return i - 1 ? i * factorial(i - 1) : 1;
}

static constexpr size_t numberOfRadicalIndecies{ (1 + factorial(tunedNotes - 1)) };



std::array<std::array<double, numberOfRadicalIndecies>, tunedNotes> pathRadicalIndecies{};

//make fraction

static inline Interval getInterval(const u_short& noteFrom, const u_short& noteTo)
{

    return scale[noteFrom > tunedNotes ? tunedNotes : noteFrom]
        [noteTo > scaleSize ? tunedNotes : noteTo];
}

    static constexpr std::array<std::array<Interval, scaleSize>, scaleSize> scale
    { {{{
        { f(1) }, { f(9,8) }, { f(5,3) }, { f(2) }
    }},{{
        { f(8,9) }, { f(1) }, { f(3,2) }, { f(9,5) }
    }},{{
        { f(3,5) }, { f(2,3) }, { f(1) }, { f(6,5) }
    }},{{
        { f(1,2) }, { f(5,9) }, { f(5,6) }, { f(1) }
    }}} };

    static constexpr u_short rootNote{ 0 };





double calculateTuning(std::vector<u_short>& path, const double& sum = 1)
{
    for (u_short note{ 0 }; note != scaleSize; ++note)
        if (std::any_of(path.begin(), path.end(), [&note](auto& noteInPath)
            { return noteInPath != note; }) )
        {
            const auto& interval{ scale[path[path.size() - 1]][note] };

            path.push_back(note);
            return sum * std::pow(interval.size * calculateTuning(path, sum), interval.weight);
            path.pop_back();
        }

    return sum;
}

void populatepathRadicalIndecies()
{
    for (auto note{ 0 }; note != scaleSize; ++note)
    {
        for ()
    }
}

struct CombSum
{
    double sum;
    uint64_t mask;
};

static std::vector<CombSum> generateCombinationSums(const double* a, size_t n)
{
    const uint64_t total = 1ULL << n;
    std::vector<CombSum> out(total);

    out[0] = { 0.0, 0 };

    for (uint64_t m = 1; m < total; ++m)
    {
        uint64_t lsb = m & (~m + 1);
        uint64_t prev = m ^ lsb;
        int idx = std::countr_zero(lsb);

        out[m].sum = out[prev].sum + a[idx];
        out[m].mask = m;
    }

    return out;
}

static inline double sumFromMask(const double* a, uint64_t mask)
{
    double sum = 0.0;
    while (mask)
    {
        uint64_t lsb = mask & (~mask + 1);
        sum += a[std::countr_zero(lsb)];
        mask ^= lsb;
    }
    return sum;
}

static inline double sumForIndices(const std::vector<CombSum>& sums, const std::vector<size_t>& indices)
{
    uint64_t mask = 0;
    for (size_t i : indices)
        mask |= 1ULL << i;

    return sums[mask].sum;
}

*/

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return std::abs(a);
}

static int integerPower(int base, int exp)
{
    int result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

class Fraction
{
public:
    Fraction(const int& n, const int& d = 1)
        : numerator(n), denominator(d == 0 ? 1 : d)
    {
        cancel();
    }

    int getNumerator() const
    {
        return numerator;
    }

    int getDenominator() const
    {
        return denominator;
    }

    Fraction getReciporical() const
    {
		return Fraction(denominator, numerator);
    }

    void setValue(const int& newNumerator, const int& newDenominator = 1)
    {
        numerator = newNumerator;
        denominator = newDenominator == 0 ? 1 : newDenominator;

        cancel();
    }

    operator double() const
    {
        return (double)numerator / (double)denominator;
    }

    Fraction operator*(const Fraction otherFraction) const
    {
        return { numerator * otherFraction.numerator,
                 denominator * otherFraction.denominator };
    }

    Fraction power(const int& exponent) const
    {
        
        return { integerPower(numerator, exponent), integerPower(denominator, exponent) };
    }

private:
    int numerator;
    int denominator{ 1 };

    void cancel()
    {
        const auto GCD{ gcd(denominator, numerator) };

        numerator /= GCD;
        denominator /= GCD;
    }
};
//static constexpr int trueRootNote{ 0 };

//static constexpr double lowerWeightLimit{ 0 };

struct Interval
{
    double size;
    double weight{ 1 };
};

static double f(const int& numerator, const int& denominator = 1)
{
    return static_cast<double>(numerator) /
           static_cast<double>(denominator == 0 ? 1 : denominator);
}

class Scale
{
public:
    Scale(const std::vector<std::vector<Interval>>& i)
		: intervals(i)
    {
        for (auto rowSize{ 0 }; rowSize != intervals.size(); ++rowSize)
        {
            if (intervals[rowSize].size() != intervals.size() - rowSize)
				throw std::invalid_argument("Invalid scale intervals");
        }
    }

    inline size_t size() const
    {
        return intervals.size() + 1;
    }

    Interval getInterval(const int& noteTo, const int& noteFrom) const
    {
        if (noteFrom > noteTo)
            return { (double)1 / intervals[noteTo][noteFrom - noteTo - 1].size,
                     intervals[noteTo][noteFrom - noteTo - 1].weight};

        if (noteTo == noteFrom)
            return { 1, 0 };

        return intervals[noteFrom][noteTo - noteFrom - 1];
    }

    std::vector<double> tuneScale(const double& weightLimit, const double& trueRootNote) const
    {
        std::vector<std::vector<double>> tunings(size(), std::vector<double>(size()));

        for (auto rootNote{ 0 }; rootNote != size(); ++rootNote)
            for (auto note{ 0 }; note != size(); ++note)
                tunings[rootNote][note] = rootNote == note ? 1
                : makeTuning(rootNote, note, weightLimit);

        for (auto rootNote{ 0 }; rootNote != size(); ++rootNote)
        {
            const auto adjustmentFactor{ 1 / tunings[rootNote][trueRootNote] };
            if (rootNote != trueRootNote)
                for (auto& tunedNote : tunings[rootNote])
                    tunedNote *= adjustmentFactor;
        }

        std::vector<double> trueTuning(size(), 1);
        for (auto note{ 0 }; note != size(); ++note)
        {
            for (auto rootNote{ 0 }; rootNote != size(); ++rootNote)
                trueTuning[note] *= tunings[rootNote][note];

            //const auto trueNote{ std::pow(trueTuning[note], (double)1 / (double)scaleSize) };
            trueTuning[note] = std::pow(trueTuning[note], (double)1 / (double)size());
        }

        return trueTuning;
    }

private:
    double sumWeights(const int& noteTo, std::vector<int>& notesFrom) const
    {
        double sum{ 0 };

        for (auto& noteFrom : notesFrom)
            sum += getInterval(noteTo, noteFrom).weight;

        return sum;
    }

    double makeTuning(const int& rootNote, int& note, const double& weightLimit) const
    {
        double tunedNote{ 1 };

        double adjustmentFactor{ 1 };
        bool adjustmentFactorWasSet{ false };

        std::vector<int> nextNotes(size());
        std::iota(nextNotes.begin(), nextNotes.end(), 0);
        nextNotes.erase(find(nextNotes.begin(), nextNotes.end(), note));

        const auto firstRollingWeight{ 1 / sumWeights(note, nextNotes) };

        return std::pow(traverseScale(note, nextNotes, rootNote, firstRollingWeight, weightLimit),
            firstRollingWeight);
    }

    double traverseScale(int& lastNote, std::vector<int>& possibleNextNotesInPath,
        const int& rootNote, const double& rollingWeight, const double& weightLimit) const
    {
        double returnValue{ 1 };

        for (auto nextNoteIndex{ 0 }; nextNoteIndex != possibleNextNotesInPath.size(); ++nextNoteIndex)
        {
            const auto nextNote{ possibleNextNotesInPath[nextNoteIndex] };
            const auto nextInterval{ getInterval(lastNote, nextNote) };

            if (nextNote == rootNote || nextInterval.weight * rollingWeight <= weightLimit)
            {
                const auto intervalFromRootSize{ getInterval(lastNote, rootNote).size };

                returnValue *= std::pow(intervalFromRootSize, nextInterval.weight);
            }
            else
            {
                const auto initialLastNote{ lastNote };

                lastNote = possibleNextNotesInPath[nextNoteIndex];
                possibleNextNotesInPath.erase(possibleNextNotesInPath.begin() + nextNoteIndex);

                const auto sumWeightsToNextNote{ 1 / sumWeights(nextNote, possibleNextNotesInPath) };

                returnValue *= std::pow(nextInterval.size *
                    std::pow(traverseScale(lastNote, possibleNextNotesInPath, rootNote,
                        nextInterval.weight * rollingWeight * sumWeightsToNextNote, weightLimit),
                        sumWeightsToNextNote),
                    nextInterval.weight);

                possibleNextNotesInPath.insert(possibleNextNotesInPath.begin() + nextNoteIndex, lastNote);
                lastNote = initialLastNote;
            }
        }

        return returnValue;
    }

	const std::vector<std::vector<Interval>> intervals;
};

static inline double harmonicEntropyOfFraction(const Fraction& fraction)
{
    return (double)1 / fraction.getNumerator() * fraction.getDenominator();
}

//make harmonic-entropy-weighted interval from fraction
static Interval HE(const Fraction& fraction)
{
    return { (double)fraction, harmonicEntropyOfFraction(fraction)};
}

static inline double centsFromRatio(const double& ratio)
{
    return 1200 * std::log2(ratio);
}

static Fraction getFractionInBaseFractions(const std::vector<std::vector<Fraction>>& baseFractions,
                                       const int& noteTo, const int& noteFrom)
{
    if (noteFrom > noteTo)
        return baseFractions[noteTo][noteFrom - noteTo - 1].getReciporical();

    if (noteTo == noteFrom)
        return { 1, 0 };

    return baseFractions[noteFrom][noteTo - noteFrom - 1];
}

static std::vector<std::vector<Fraction>> repeatBaseScaleIntervals(const std::vector<std::vector<Fraction>>& baseFractions, const int& times)
{
    auto repeatedFractions(baseFractions);
	repeatedFractions.reserve(baseFractions.size() * times);

    const auto repeatFactor{ baseFractions[0][baseFractions.size() - 1] };
    
    for (auto repeat{ 1 }; repeat != times; ++repeat)
        for (auto interval{ 0 }; interval != baseFractions.size(); ++interval)
        {
            for (auto row{ 0 }; row != repeatedFractions.size(); ++row)
            {
                const auto repeatAsFraction{ repeatFactor.power(repeat - row / baseFractions.size()) };

                repeatedFractions[row].insert(repeatedFractions[row].end(),
                                              repeatAsFraction * getFractionInBaseFractions(baseFractions,
                                                                                             interval + 1 % baseFractions.size(),
                                                                                             row % baseFractions.size()));
            }

            repeatedFractions.insert(repeatedFractions.end(),
                                     { repeatFactor.power(repeat - repeatedFractions.size() / baseFractions.size()) *
                                       getFractionInBaseFractions(baseFractions,
                                                                   interval + 1 % baseFractions.size(),
                                                                   repeatedFractions.size() % baseFractions.size()) });
        }

    return repeatedFractions;
}

static std::vector<std::vector<Interval>> baseFractionsToIntervalsWithHarmonicWeight(const std::vector<std::vector<Fraction>>& baseFractions)
{
    std::vector<std::vector<Interval>> baseIntervals;
    baseIntervals.reserve(baseFractions.size());

    for (auto rowItr{ baseFractions.begin() }; rowItr != baseFractions.end(); ++rowItr)
    {
        std::vector<Interval> intervalsRow;

		intervalsRow.reserve(rowItr->size());
        for (auto fractionItr{ rowItr->begin() }; fractionItr != rowItr->end(); ++fractionItr)
            intervalsRow.push_back( {(double)*fractionItr, harmonicEntropyOfFraction(*fractionItr)} );

		baseIntervals.push_back( intervalsRow );
    }

	return baseIntervals;
}

static std::vector<std::vector<Interval>> baseFractionsToIntervalsWithUniformWeight(const std::vector<std::vector<Fraction>>& baseFractions)
{
    std::vector<std::vector<Interval>> baseIntervals;
    baseIntervals.reserve(baseFractions.size());

    for (auto rowItr{ baseFractions.begin() }; rowItr != baseFractions.end(); ++rowItr)
    {
        std::vector<Interval> intervalsRow;

        intervalsRow.reserve(rowItr->size());
        for (auto fractionItr{ rowItr->begin() }; fractionItr != rowItr->end(); ++fractionItr)
            intervalsRow.push_back({ (double)*fractionItr, 1 });

        baseIntervals.push_back(intervalsRow);
    }

    return baseIntervals;
}

int main()
{
    std::vector<std::vector<Fraction>> baseMajorScaleIntervals{
        { {9,8}, {5,4}, {4,3}, {3,2}, {5,3}, {15,8}, {2} },
        { {9,8}, {6,5}, {4,3}, {3,2}, {5,3}, {9,5} },
        { {16,15}, {6,5}, {4,3}, {3,2}, {8,5} },
        { {9,8}, {5,4}, {7,5}, {3,2} },
        { {9,8}, {5,4}, {4,3} },
        { {9,8}, {6,5} },
        { {16,15} }
    };

    const auto baseMajorScaleIntervals_2{ repeatBaseScaleIntervals(baseMajorScaleIntervals, 2) };

    const auto baseMajorScaleIntervals_8{ repeatBaseScaleIntervals(baseMajorScaleIntervals, 8) };

    const Scale scale{ baseFractionsToIntervalsWithUniformWeight(baseMajorScaleIntervals_8) };

    auto tuning{ scale.tuneScale(0.00001, 0) };

    for (auto& note : tuning)
		std::cout << centsFromRatio(note) << std::endl;
}