#include "Scale.h"

Scale::Scale(const std::vector<std::vector<Interval>>& i)
    : intervalsPattern(i)
{
    for (auto rowSize{ 0 }; rowSize != intervalsPattern.size(); ++rowSize)
    {
        if (intervalsPattern[rowSize].size() != intervalsPattern.size() - rowSize)
            throw std::invalid_argument("Invalid scale intervals");
    }
}

inline size_t Scale::size() const
{
    return intervalsPattern.size() + 1;
}

Interval Scale::getInterval(const int& noteTo, const int& noteFrom) const
{
    if (noteFrom > noteTo)
        return { (double)1 / intervalsPattern[noteTo][noteFrom - noteTo - 1].size,
                 intervalsPattern[noteTo][noteFrom - noteTo - 1].weight };

    if (noteTo == noteFrom)
        return { 1, 0 };

    return intervalsPattern[noteFrom][noteTo - noteFrom - 1];
}

std::vector<double> Scale::tuneScale(const double& trueRootNote, const double& weightLimit) const
{
    //calculate tunings for all root notes
    std::vector<std::vector<double>> tunings(size(), std::vector<double>(size()));

    double lastLoadingPercentage{ 0 };
    const double loadingInterval{ 0.1 };
    std::cout << "Tuning scale:" << std::endl;
    std::cout << std::fixed << std::setprecision(1) << 0.0 << "% \r";

    for (auto rootNote{ 0 }; rootNote != size(); ++rootNote)
        for (auto note{ 0 }; note != size(); ++note)
        {
            tunings[rootNote][note] = rootNote == note ? 1 : makeTuning(rootNote, note, weightLimit);

            const double nextLoadingPercentage{ (((double)rootNote / (double)size()) +
                                                ((double)note / ((double)size() * (double)size()))
                                                ) * 100 };

            if (nextLoadingPercentage - lastLoadingPercentage >= loadingInterval)
            {
                std::cout << nextLoadingPercentage << "% \r";
                lastLoadingPercentage = nextLoadingPercentage;
            }
        }

    //normalise tunings to the true root note
    for (auto rootNote{ 0 }; rootNote != size(); ++rootNote)
    {
        const auto adjustmentFactor{ 1 / tunings[rootNote][trueRootNote] };
        
        if (rootNote != trueRootNote)
            for (auto& tunedNote : tunings[rootNote])
                tunedNote *= adjustmentFactor;
    }

    //geometric mean of all tunings for each note gives the true tuning
    std::vector<double> trueTuning(size(), 1);
    for (auto note{ 0 }; note != size(); ++note)
    {
        for (auto rootNote{ 0 }; rootNote != size(); ++rootNote)
            trueTuning[note] *= tunings[rootNote][note];

        trueTuning[note] = std::pow(trueTuning[note], (double)1 / (double)size());
    }

    std::cout << 100.0 << "% \r\n" << std::endl;

    return trueTuning;
}

double Scale::sumWeights(const int& noteTo, std::vector<int>& notesFrom) const
{
    double sum{ 0 };

    for (auto& noteFrom : notesFrom)
        sum += getInterval(noteTo, noteFrom).weight;

    return sum;
}

double Scale::makeTuning(const int& rootNote, int& note, const double& weightLimit) const
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

double Scale::traverseScale(int& lastNote, std::vector<int>& possibleNextNotesInPath,
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