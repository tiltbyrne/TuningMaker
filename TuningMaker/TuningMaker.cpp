#include "PitchSpace.h"

static std::vector<long double> insertDummyNotesInTuning(const std::vector<long double>& tuning, const std::vector<int>& dummyNoteIndecies)
{
    auto returnTuning{ tuning };
    returnTuning.reserve(tuning.size() + dummyNoteIndecies.size());

    
}

static void initialiseBaseScales()
{
    using namespace PitchSpaces;

    chromatic.addSigniature("major pentatonic", { 0, 2, 4, 7, 9 });
    chromatic.addSigniature("minor pentatonic", { 0, 3, 4, 7, 10 });

    chromatic.addSigniature("ionion", { 0, 2, 4, 5, 7, 9, 11 });
    chromatic.addSigniature("dorian", { 0, 2, 3, 5, 7, 9, 10 });
    chromatic.addSigniature("phrygian", { 0, 1, 3, 5, 7, 8, 10 });
    chromatic.addSigniature("lydian", { 0, 2, 4, 6, 7, 9, 11 });
    chromatic.addSigniature("myxolydian", { 0, 2, 4, 5, 7, 9, 10 });
    chromatic.addSigniature("aolian", { 0, 2, 3, 5, 7, 8, 10 });
    chromatic.addSigniature("locrian", { 0, 1, 3, 5, 6, 8, 10 });

    neutralSevenEDO.addSigniature("neutral pentatonic A", { 0, 1, 3, 4, 6 });
    neutralSevenEDO.addSigniature("neutral pentatonic B", { 0, 2, 3, 5, 6 });

    neutralSevenEDO.addSigniature("full", { 0, 1, 2, 3, 4, 5, 6 });
}

int main()
{
    initialiseBaseScales();

    const std::string name{ "full" };

    const auto& pitchSpace{ PitchSpaces::neutralSevenEDO };

    const int range{ 61 };

    const auto fractionsTable{ pitchSpace.makeRangedScaleFractions(name, range) };

    if (fractionsTable.has_value())
    {
        Scale scale(fractionsToIntervalsWithHarmonicWeight(fractionsTable.value(), 20), name);

        scale.setDummyIndecies(pitchSpace.populateDummyIndecies(name, range));

        const auto tuning{ scale.tuneScale(0, 0.00001) };

        for (auto note{ 0 }; note != tuning.size(); ++note)
            std::cout << /*note << ") " <<*/ std::fixed << std::setprecision(4) << FrequencyFromRatio(tuning[note], 20) << " ";

        for (auto note{ 0 }; note != tuning.size(); ++note)
            std::cout << /*note << ") " <<*/ std::fixed << std::setprecision(4) << FrequencyFromRatio(tuning[note], 20) << std::endl;
    }
}