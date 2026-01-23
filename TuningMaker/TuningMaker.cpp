#include "PitchSpace.h"
#include <algorithm>

static constexpr size_t maxMidiNotes{ 128 };

static void initialiseBaseScales()
{
    auto& sevenEDO{ PitchSpaces::fractional.at("seven EDO") };

    sevenEDO.addSigniature("neutral pentatonic A", { 0, 1, 3, 4, 6 });
    sevenEDO.addSigniature("neutral pentatonic B", { 0, 2, 3, 5, 6 });

    auto& twelveEDO{ PitchSpaces::fractional.at("twelve EDO") };

    twelveEDO.addSigniature("major pentatonic", { 0, 2, 4, 7, 9 });
    twelveEDO.addSigniature("minor pentatonic", { 0, 3, 4, 7, 10 });
    twelveEDO.addSigniature("ionian", { 0, 2, 4, 5, 7, 9, 11 });
    twelveEDO.addSigniature("dorian", { 0, 2, 3, 5, 7, 9, 10 });
    twelveEDO.addSigniature("phrygian", { 0, 1, 3, 5, 7, 8, 10 });
    twelveEDO.addSigniature("lydian", { 0, 2, 4, 6, 7, 9, 11 });
    twelveEDO.addSigniature("myxolydian", { 0, 2, 4, 5, 7, 9, 10 });
    twelveEDO.addSigniature("aolian", { 0, 2, 3, 5, 7, 8, 10 });
    twelveEDO.addSigniature("locrian", { 0, 1, 3, 5, 6, 8, 10 });
}

static void printTuning(const std::vector<double>& tuning, const double& baseFrequency)
{
    std::cout << "Copy the following and paste it into the New Pitch textbox inside Scala's Edit scale window to make a tuning file: "
        << std::endl << std::endl << "z";

    for (auto note{ 0 }; note != tuning.size(); ++note)
        std::cout << std::fixed << std::setprecision(4) << frequencyFromRatio(tuning[note], baseFrequency) << " ";

    std::cout << "999999999999999" << std::endl << std::endl << "Tuning in cents:" << std::endl << std::endl;

    for (auto note{ 0 }; note != tuning.size(); ++note)
    {
        const auto& cents{ centsFromRatio(tuning[note]) };

        std::cout << "note " << note << ") ";

        if (!std::isnan(cents))
            std::cout << std::fixed << std::setprecision(4) << centsFromRatio(tuning[note]);

        std::cout << std::endl;
    }
}

int main()
{
    initialiseBaseScales();

    const std::string pitchSpaceName{ "twelve EDO" };

    const auto& pitchSpace{ PitchSpaces::fractional.at(pitchSpaceName) };

    const std::string scaleName{ "major pentatonic" };

    const auto range{ 11 };

    const auto relationsTable{ pitchSpace.makeRangedScaleRelations(scaleName, range) };

    if (relationsTable.has_value())
    {
        Scale scale(fractionsToIntervalsWithHarmonicWeight(relationsTable.value(), 1),
                    pitchSpaceName + " " + scaleName);

        const auto wantsDummyNotes{ true };

        if (wantsDummyNotes)
            scale.setDummyIndecies(pitchSpace.getDummyIndecies(scaleName, range));

        const auto trueRootNote{ 0 };

        const double weightLimit{ 0.01 };

        const auto tuning{ scale.tuneScale(trueRootNote, weightLimit) };

        const double baseFrequency{ 20 };

        printTuning(tuning, baseFrequency);
    }
}