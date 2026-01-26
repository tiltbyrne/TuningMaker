#include "PitchSpace.h"
#include <fstream>

static constexpr size_t maxMidiNotes{ 128 };

template<typename Relation>
static void addCustomScaleToPitchSpace(PitchSpace<Relation>& pitchSpace, const std::string& scaleName)
{
    std::vector<int> scaleSigniature;
    std::string inputLine;

    while (true)
    {
        std::cin >> inputLine;

        if (inputLine == "end")
            break;

        const int relation{ std::stoi(inputLine) };
        scaleSigniature.push_back(relation);
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    pitchSpace.addSigniature(scaleName, scaleSigniature);
}


static void addCustomDecimalPitchSpace(const std::string& pitchSpaceName)
{
    std::vector<long double> relationsTable;
    std::string inputLine;

    while (true)
    {
        std::cin >> inputLine;

        if (inputLine == "end")
            break;

        const long double decimalValue{ std::stold(inputLine) };
        relationsTable.push_back(decimalValue);
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    PitchSpaces::decimal.insert({ pitchSpaceName, relationsTable });
}

static void addCustomFractionalPitchSpace(const std::string& pitchSpaceName)
{
    std::vector<Fraction> relationsTable;
    std::string inputLine;

    while (true)
    {
        std::cin >> inputLine;

        if (inputLine == "end")
            break;
        const auto separatorPosition{ inputLine.find('/') };
        if (separatorPosition != std::string::npos)
        {
            const auto numeratorString{ inputLine.substr(0, separatorPosition) };
            const auto denominatorString{ inputLine.substr(separatorPosition + 1) };
            const int numerator{ std::stoi(numeratorString) };
            const int denominator{ std::stoi(denominatorString) };
            relationsTable.push_back({ numerator, denominator });
        }
        else
        {
            const int numerator{ std::stoi(inputLine) };
            relationsTable.push_back({ numerator, 1 });
        }
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    PitchSpaces::fractional.insert({ pitchSpaceName, relationsTable });
}

static void printTuning(const std::vector<double>& tuning)
{
    std::cout << std::endl << "As linear factors: " << std::endl;

    for (auto note{ 0 }; note != tuning.size(); ++note)
    {
        const auto& factor{ tuning[note] };

        if (std::isnan(factor))
            std::cout << '\n' << "0.0001";
        else
            std::cout << '\n' << std::setprecision(4) << factor;
    }

	std::cout << std::endl << std::endl << "As cents: " << std::endl << std::endl;

    for (auto note{ 0 }; note != tuning.size(); ++note)
    {
		std::cout << std::fixed << note << ") ";

        const auto& factor{ tuning[note] };

        if (!std::isnan(factor))
            std::cout << std::setprecision(4) << centsFromRatio(tuning[note]);

        std::cout << '\n';
    }
}

int main()
{
    PitchSpaces::initialisePitchSpaceScales();

    std::cout << "Welcome to Tuning Maker. To make a tuning of a scale you must first choose the pitch space it occupies. "
        << "Do you want to use a decimal or fractional pitch space? ";

    char pitchSpaceType;

    while (true)
    {
        std::cout << "Enter 'd' for decimal or 'f' for fractional: ";
        std::cin >> pitchSpaceType;

        switch (pitchSpaceType)
        {
        case 'd':
        {
            std::cout << std::endl << "Available pitch spaces: " << std::endl << std::endl;
            PitchSpaces::printPitchSpaces(PitchSpaces::decimal);
            break;
        }
        case 'f':
        {
            std::cout << std::endl << "Available pitch spaces: " << std::endl << std::endl;
            PitchSpaces::printPitchSpaces(PitchSpaces::fractional);
            break;
        }
        default:
            std::cout << std::endl << "Invalid pitch space type. ";
            break;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (pitchSpaceType == 'd' || pitchSpaceType == 'f')
			break;
    }

    std::cout << "Enter the name of the pitch space you want to use or enter a new name to create a custom pitch space: ";

    std::string pitchSpaceName;
    std::cin >> pitchSpaceName;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << std::endl;

    switch (pitchSpaceType)
    {
    case 'd':
        if (PitchSpaces::decimal.find(pitchSpaceName) == PitchSpaces::decimal.end())
        {
            std::cout << "Enter the intervals in [" << pitchSpaceName << "] as decimals separated by spaces. Enter 'end' when finished." << std::endl << std::endl;

            addCustomDecimalPitchSpace(pitchSpaceName);

            std::cout << std::endl;
        }
        break;
	case 'f':
        if (PitchSpaces::fractional.find(pitchSpaceName) == PitchSpaces::fractional.end())
        {
            std::cout << "Enter the intervals in [" << pitchSpaceName << "] as fractions separated by spaces. Enter 'end' when finished." << std::endl << std::endl;

            addCustomFractionalPitchSpace(pitchSpaceName);

            std::cout << std::endl;
        }
        break;
    default:
        break;
    }

    std::cout << "Available scales in [" << pitchSpaceName << "]:" << std::endl << std::endl;

    if (pitchSpaceType == 'd')
    {
        PitchSpaces::decimal.at(pitchSpaceName).printSigniatures();
    }
    else if (pitchSpaceType == 'f')
    {
        PitchSpaces::fractional.at(pitchSpaceName).printSigniatures();
	}

    std::cout << "Enter the name of the scale in [" << pitchSpaceName << "] you want to use or enter a new name to create a custom scale: ";

    std::string scaleName;
    std::cin >> scaleName;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    switch (pitchSpaceType)
    {
    case 'd':
        if (!PitchSpaces::decimal.at(pitchSpaceName).getSigniature(scaleName).has_value())
        {
            std::cout << std::endl << "Enter the indecies of the intervals from the [" << scaleName << "] scale as integers separated by spaces. Enter 'end' when finished:" << std::endl << std::endl;

            addCustomScaleToPitchSpace(PitchSpaces::decimal.at(pitchSpaceName), scaleName);
        }
        break;
    case 'f':
        if (!PitchSpaces::fractional.at(pitchSpaceName).getSigniature(scaleName).has_value())
        {
            std::cout << std::endl << "Enter the indecies of the intervals from the [" << scaleName << "] scale as integers separated by spaces. Enter 'end' when finished:" << std::endl << std::endl;

            addCustomScaleToPitchSpace(PitchSpaces::fractional.at(pitchSpaceName), scaleName);
        }
        break;
    default:
        break;
    }   

	std::cout << std::endl << "Your chosen scale is: " << std::endl << std::endl
        << "[" << pitchSpaceName << "]-";

    int scaleLength;

    switch (pitchSpaceType)
    {
    case 'd':
        PitchSpaces::decimal.at(pitchSpaceName).printSigniature(scaleName);
        scaleLength = PitchSpaces::decimal.at(pitchSpaceName).getSigniature(scaleName).value().size();
        break;
    case 'f':
        PitchSpaces::fractional.at(pitchSpaceName).printSigniature(scaleName);
        scaleLength = PitchSpaces::fractional.at(pitchSpaceName).getSigniature(scaleName).value().size();
        break;
    default:
        break;
    }

    std::cout << std::endl << std::endl << "Enter the range of the final tuning of this scale "
        << "(hint: this scale contains " << scaleLength << " notes): ";

    int range;
    std::cin >> range;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (range < 1)
        range = 1;

    std::cout << std::endl << "Enter the index of the root note of the final tuning of this scale [0, " << range << "): ";

    int trueRootNote;
    std::cin >> trueRootNote;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (trueRootNote < 0)
        trueRootNote = 0;

    if (trueRootNote >= range)
        trueRootNote = range - 1;

    std::cout << std::endl << "Would you like to fill notes in [" << pitchSpaceName << "] not contained in [" << scaleName << "] with 'dummy notes'? ";

	char wantsDummyNotes;

    Scale scale;

    while (true)
    {
        std::cout << "Enter 'y' for yes or 'n' for no: ";
        std::cin >> wantsDummyNotes;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (wantsDummyNotes == 'y' || wantsDummyNotes == 'n')
        {
            break;
        }
        else
        {
            std::cout << std::endl << "Invalid answer. ";
        }
    }

	const std::string scaleNameFull{ "[" + pitchSpaceName + "]-[" + scaleName + "]" };

    if (pitchSpaceType == 'd')
    {
        const auto relationsTable{ PitchSpaces::decimal.at(pitchSpaceName).makeRangedScaleRelations(scaleName, range) };
        if (relationsTable.has_value())
        {
            scale = Scale(IntervalPatternMakers::rangedScaleLongDoubleToIntervalsWithUniformWeight(relationsTable.value()),
                          scaleNameFull);

            if (wantsDummyNotes == 'y')
				scale.setDummyIndecies(PitchSpaces::decimal.at(pitchSpaceName).getDummyIndecies(scaleName, range));
        }
    }
    else if (pitchSpaceType == 'f')
    {
        const auto relationsTable{ PitchSpaces::fractional.at(pitchSpaceName).makeRangedScaleRelations(scaleName, range) };
        if (relationsTable.has_value())
        {
            std::cout << std::endl << "Enter the exponent of the Tenney height used to calculate each interval's weight "
                << "(hint: larger values can produce tunings with more relatively accurate approximations of 'simple' intervals and 0 treats all intervals equally): ";

            long double enropyCurve;
            std::cin >> enropyCurve;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            scale = Scale(IntervalPatternMakers::rangedScaleFractionsToIntervalsWithTenneyWeight(relationsTable.value(), enropyCurve),
                          scaleNameFull);

            if (wantsDummyNotes == 'y')
                scale.setDummyIndecies(PitchSpaces::fractional.at(pitchSpaceName).getDummyIndecies(scaleName, range));
        }
    }

	std::cout << std::endl << "Enter the weight limit (between 0 and 1) for tuning calculations (hint: smaller values produce more accurate tunings but take longer to compute): ";

    long double weightLimit;
	std::cin >> weightLimit;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (weightLimit < 0)
		weightLimit = 0;
	if (weightLimit > 1)
		weightLimit = 1;

    std::cout << std::endl;

    const auto tuning{ scale.tuneScale(trueRootNote, weightLimit) };

    std::cout << "Final tuning for " << scaleNameFull << ": " << std::endl;

    printTuning(tuning);
}