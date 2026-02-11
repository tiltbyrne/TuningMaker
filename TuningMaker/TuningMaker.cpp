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

static void writeTuningFile(const std::vector<float>& tuning, std::ofstream& file)
{
    file << std::setprecision(8);

    file << "Tuning as cents:" << std::endl;

    for (auto note{ 0 }; note != tuning.size(); ++note)
    {
        const auto& factor{ tuning[note] };

        if (std::isnan(factor))
            file << '\n' << 0.f;
        else
            file << '\n' << centsFromRatio(factor);
    }

    file << std::endl << std::endl << "Tuning as linear factors:" << std::endl;

    for (auto note{ 0 }; note != tuning.size(); ++note)
    {
        const auto& factor{ tuning[note] };

        if (std::isnan(factor))
            file << '\n' << 1.f;
        else
            file << '\n' << factor;
    }

    file.close();
}

int main()
{
    PitchSpaces::initialisePitchSpaceScales();

    std::cout << "Welcome to Tuning Maker. This program will ask for information about a scale, then compute a "
        << "tuning of it. This tuning will be written to a text file inside the folder in which TuningMaker.exe is saved.\n\n"
        << "First, choose the pitch space the scale occupies. Do you want to use a decimal or fractional pitch space? ";

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
            std::cout << "Enter the intervals in [" << pitchSpaceName << "] as decimals. Remember to include the period ('octave') of the pitch space. "
                << "Enter 'end' when finished." << std::endl << std::endl;

            addCustomDecimalPitchSpace(pitchSpaceName);

            std::cout << std::endl;
        }
        break;
	case 'f':
        if (PitchSpaces::fractional.find(pitchSpaceName) == PitchSpaces::fractional.end())
        {
            std::cout << "Enter the intervals in [" << pitchSpaceName << "] as fractions. Remember to include the period ('octave') of the pitch space. "
                << "Enter 'end' when finished." << std::endl << std::endl;

            addCustomFractionalPitchSpace(pitchSpaceName);

            std::cout << std::endl;
        }
        break;
    default:
        break;
    }

    std::cout << "Available scales in [" << pitchSpaceName << "] (period omitted):" << std::endl << std::endl;

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
            std::cout << std::endl << "Enter the indecies of the intervals from the [" << scaleName << "] scale as integers. Enter 'end' when finished:" << std::endl << std::endl;

            addCustomScaleToPitchSpace(PitchSpaces::decimal.at(pitchSpaceName), scaleName);
        }
        break;
    case 'f':
        if (!PitchSpaces::fractional.at(pitchSpaceName).getSigniature(scaleName).has_value())
        {
            std::cout << std::endl << "Enter the indecies of the intervals from the [" << scaleName << "] scale as integers. Enter 'end' when finished:" << std::endl << std::endl;

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

    std::string inputLine;
    std::cin >> inputLine;

    int range;
    try
    {
        range = std::stoi(inputLine);

        if (range < 1)
        {
            range = 1;
            std::cout << "Invalid value: range set to " << range << std::endl;
        }
    }
    catch (std::invalid_argument e)
    {
        range = scaleLength + 1;
        std::cout << "Invalid value: range set to " << range << std::endl;
    }

    inputLine.clear();

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << std::endl << "Enter the index of the root note of the final tuning of this scale [0, " << range << "): ";

    std::cin >> inputLine;

    int rootNote;
    try
    {
        rootNote = std::stoi(inputLine);

        if (rootNote < 0)
        {
            rootNote = 0;
            std::cout << "Invalid index: root note index set to " << rootNote << std::endl;
        }

        if (rootNote >= range)
        {
            rootNote = range - 1;
            std::cout << "Invalid index: root note index set to " << rootNote << std::endl;
        }
    }
    catch (std::invalid_argument e)
    {
        rootNote = 0;
        std::cout << "Invalid index: root note index set to " << rootNote << std::endl;
    }

    inputLine.clear();

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << std::endl << "Would you like to fill notes in [" << pitchSpaceName << "] not contained in ["
         << pitchSpaceName << "]-[" << scaleName << "] with dummy values, which are tuned to 0 cents? ";

	char wantsDummyNotes;

    Scale scale;

    while (true)
    {
        std::cout << "Enter 'y' for yes or 'n' for no: ";
        std::cin >> wantsDummyNotes;

        if (wantsDummyNotes == 'y' || wantsDummyNotes == 'n')
        {
            break;
        }
        else
        {
            std::cout << std::endl << "Invalid answer. ";
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
                << "(hint: larger values can produce tunings with more accurate approximations of simple intervals at "
                << "the expense of the accuratcy of more complex ones. An exponent of zero makes all weights equal to one): ";

            std::cin >> inputLine;

            long double enropyCurve;

            try
            {
                enropyCurve = std::stold(inputLine);

                clampLongDoubleToLimits(enropyCurve);
            }
            catch (std::invalid_argument e)
            {
                enropyCurve = 0;
                std::cout << "Invalid value: exponent set to " << enropyCurve << std::endl;
            }
            inputLine.clear();

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            scale = Scale(IntervalPatternMakers::rangedScaleFractionsToIntervalsWithTenneyWeight(relationsTable.value(), enropyCurve),
                          scaleNameFull);

            if (wantsDummyNotes == 'y')
                scale.setDummyIndecies(PitchSpaces::fractional.at(pitchSpaceName).getDummyIndecies(scaleName, range));
        }
    }

    const long double recomendedCuttoff{ 0.001 };
	std::cout << std::endl << "Enter the cutoff weight [0, 1] for tuning calculations (hint: smaller values produce more accurate tunings "
        << "but take longer to compute, " << recomendedCuttoff << " tends to work well): ";

    std::cin >> inputLine;

    long double cutoffWeight;
    try
    {
        cutoffWeight = std::stold(inputLine);

        if (cutoffWeight < 0)
        {
            cutoffWeight = recomendedCuttoff;
            std::cout << "Invalid value: cutoff weight set to " << cutoffWeight << std::endl;
        }
        if (cutoffWeight > 1)
        {
            cutoffWeight = recomendedCuttoff;
            std::cout << "Invalid value: cutoff weight set to " << cutoffWeight << std::endl;
        }
    }
    catch (std::invalid_argument e)
    {
        cutoffWeight = recomendedCuttoff;
        std::cout << "Invalid value: cutoff weight set to " << cutoffWeight << std::endl;
    }
    inputLine.clear();

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << std::endl;

    scale.setWeightCutoff(cutoffWeight);

    const auto tuning{ scale.tuneScale(rootNote) };

    std::cout << "A tuning of " << scaleNameFull << " has been computed. Enter the name of the text file "
        << "to which the tuning will be written: ";

    std::string fileName;
    std::getline(std::cin, fileName);

    //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Write to the file
    std::cout << std::endl << "The text file will be written after the program is exited. Copy it's contents into "
        << "Scala, Excel, or Scale Workshop to further manipulate it, analyse it, "
        << "or produce a tuning file of it. Thank you for using Tuning Maker. Press any key to exit.\n";

    std::cin.get();

    // Create and open a text file
    std::ofstream file(fileName + ".txt");

    writeTuningFile(tuning, file);
}