#include "BaseScales.h"

static void initialiseBaseScales()
{
    using namespace BaseScales;

    chromatic.addSigniature("major pentatonic", { 0, 2, 4, 7, 9 });
    chromatic.addSigniature("minor pentatonic", { 0, 3, 4, 7, 10 });
    chromatic.addSigniature("ionion", { 0, 2, 4, 5, 7, 9, 11 });
    chromatic.addSigniature("myxolydian", { 0, 2, 4, 5, 7, 9, 10 });

    sevenEDO.addSigniature("pentatonic A", { 0, 1, 3, 4, 6 });
    sevenEDO.addSigniature("pentatonic B", { 0, 2, 3, 5, 6 });
    sevenEDO.addSigniature("full", { 0, 1, 2, 3, 4, 5, 6 });
}

int main()
{
    initialiseBaseScales();

    const Scale scale(fractionsToIntervalsWithHarmonicWeight(BaseScales::sevenEDO.makeRangedScaleFractions("pentatonic A", 11)));

    const auto trueRootNote{ 0 };
    const auto weightLimit{ 0.00001 };

    const auto tuning{ scale.tuneScale(trueRootNote, weightLimit) };

    for (auto note{ 0 }; note != tuning.size(); ++note)
    {
        //if (note)
            //std::cout << "\\\n" << centsFromRatio(tuning[note]) - centsFromRatio(tuning[note - 1]) << "\n\/\n";

        std::cout << "Note " << note << ") " << centsFromRatio(tuning[note]) << std::endl;
    }
}