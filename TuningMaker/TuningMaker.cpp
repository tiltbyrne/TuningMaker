#include "PitchSpace.h"

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

    sevenEDO.addSigniature("pentatonic A", { 0, 1, 3, 4, 6 });
    sevenEDO.addSigniature("pentatonic B", { 0, 2, 3, 5, 6 });
    sevenEDO.addSigniature("full", { 0, 1, 2, 3, 4, 5, 6 });
}

int main()
{
    initialiseBaseScales();

    const Scale scale(fractionsToIntervalsWithHarmonicWeight(
        PitchSpaces::sevenEDO.makeRangedScaleFractions("pentatonic A", 61), 0.1));

    const auto trueRootNote{ 0 };
    const auto weightLimit{ 0.001 };

    const auto tuning{ scale.tuneScale(trueRootNote, weightLimit) };

    for (auto note{ 0 }; note != tuning.size(); ++note)
    {
        //if (note)
            //std::cout << "\\\n" << centsFromRatio(tuning[note]) - centsFromRatio(tuning[note - 1]) << "\n\/\n";

        std::cout << std::fixed << std::setprecision(4) << centsFromRatio(tuning[note]) << std::endl;
    }
}