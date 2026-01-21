#include "Scale.h"

Scale::Scale(const std::vector<std::vector<Interval>>& i, const std::string& n)
    : intervalsPattern(i)
    , name(n)
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
        return { 1L / intervalsPattern[noteTo][noteFrom - noteTo - 1].size,
                 intervalsPattern[noteTo][noteFrom - noteTo - 1].weight };

    if (noteTo == noteFrom)
        return { 1, 0 };

    return intervalsPattern[noteFrom][noteTo - noteFrom - 1];
}

std::vector<long double> Scale::tuneScale(const long double& trueRootNote, const long double& weightLimit) const
{
    //calculate tunings for all root notes
    std::vector<std::vector<long double>> tunings(size(), std::vector<long double>(size()));

    long double lastLoadingPercentage{ 0 };
    const long double loadingInterval{ 0.1 };
    std::cout << "Tuning scale: " << name << std::endl;
    std::cout << std::fixed << std::setprecision(1) << "0.0% \r";

    for (auto rootNote{ 0 }; rootNote != size(); ++rootNote)
        for (auto note{ 0 }; note != size(); ++note)
        {
            tunings[rootNote][note] = rootNote == note ? 1 : makeTuning(rootNote, note, weightLimit);

            const long double nextLoadingPercentage{ (((long double)rootNote / (long double)size()) +
                                                     ((long double)note / ((long double)size() * (long double)size()))
                                                     ) * 100 };

            std::cout << nextLoadingPercentage << "% \r";
            if (nextLoadingPercentage - lastLoadingPercentage >= loadingInterval)
            {
                std::cout << nextLoadingPercentage << "% \r";
                lastLoadingPercentage = nextLoadingPercentage;
            }
        }

    //normalise tunings to the true root note
    for (auto rootNote{ 1 }; rootNote != size(); ++rootNote)
    {
        const auto adjustmentFactor{ 1L / tunings[rootNote][trueRootNote] };
        
        if (rootNote != trueRootNote)
            for (auto& tunedNote : tunings[rootNote])
                tunedNote *= adjustmentFactor;
    }

    //geometric mean of all tunings for each note gives the true tuning
    std::vector<long double> trueTuning(size(), 1);
    for (auto note{ 0 }; note != size(); ++note)
        for (auto rootNote{ 0 }; rootNote != size(); ++rootNote)
            trueTuning[note] *= std::pow(tunings[rootNote][note], 1L / (long double)size());

    std::cout << "100.0% \r\n" << std::endl;

    return trueTuning;
}

long double Scale::sumWeights(const int& noteTo, std::vector<int>& notesFrom) const
{
    long double sum{ 0 };

    for (auto& noteFrom : notesFrom)
        sum += getInterval(noteTo, noteFrom).weight;

    return sum;
}

long double Scale::makeTuning(const int& rootNote, int& note, const long double& weightLimit) const
{
    long double tunedNote{ 1 };

    long double adjustmentFactor{ 1 };
    bool adjustmentFactorWasSet{ false };

    std::vector<int> nextNotes(size());
    std::iota(nextNotes.begin(), nextNotes.end(), 0);
    nextNotes.erase(find(nextNotes.begin(), nextNotes.end(), note));

    const auto firstRollingWeight{ 1 / sumWeights(note, nextNotes) };

    return traverseScale(note, nextNotes, rootNote, firstRollingWeight, weightLimit, firstRollingWeight);
}

long double Scale::traverseScale(int& lastNote, std::vector<int>& possibleNextNotesInPath,
    const int& rootNote, const long double& rollingWeight, const long double& weightLimit, 
    const long double& possibleWeightsToNoteSum) const
{
    long double returnValue{ 1 };

    for (auto nextNoteIndex{ 0 }; nextNoteIndex != possibleNextNotesInPath.size(); ++nextNoteIndex)
    {
        const auto nextNote{ possibleNextNotesInPath[nextNoteIndex] };
        const auto nextInterval{ getInterval(lastNote, nextNote) };

        if (nextNote == rootNote || nextInterval.weight * rollingWeight <= weightLimit)
        {
            const auto intervalFromRootSize{ getInterval(lastNote, rootNote).size };

            returnValue *= std::pow(intervalFromRootSize, nextInterval.weight * possibleWeightsToNoteSum);
        }
        else
        {
            const auto initialLastNote{ lastNote };

            lastNote = nextNote;
            possibleNextNotesInPath.erase(possibleNextNotesInPath.begin() + nextNoteIndex);

            const auto sumWeightsToNextNote{ 1 / sumWeights(nextNote, possibleNextNotesInPath) };

            returnValue *= std::pow(nextInterval.size * traverseScale(lastNote, possibleNextNotesInPath, rootNote,
                                                           nextInterval.weight * rollingWeight * sumWeightsToNextNote, weightLimit, sumWeightsToNextNote),
                                    nextInterval.weight * possibleWeightsToNoteSum);

            possibleNextNotesInPath.insert(possibleNextNotesInPath.begin() + nextNoteIndex, lastNote);
            lastNote = initialLastNote;
        }
    }

    return returnValue;
}