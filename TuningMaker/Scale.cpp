#include "Scale.h"

Scale::Scale(const std::vector<std::vector<Interval>>& i)
    : intervals(i)
{
    for (auto rowSize{ 0 }; rowSize != intervals.size(); ++rowSize)
    {
        if (intervals[rowSize].size() != intervals.size() - rowSize)
            throw std::invalid_argument("Invalid scale intervals");
    }
}

inline size_t Scale::size() const
{
    return intervals.size() + 1;
}

Interval Scale::getInterval(const int& noteTo, const int& noteFrom) const
{
    if (noteFrom > noteTo)
        return { (double)1 / intervals[noteTo][noteFrom - noteTo - 1].size,
                 intervals[noteTo][noteFrom - noteTo - 1].weight };

    if (noteTo == noteFrom)
        return { 1, 0 };

    return intervals[noteFrom][noteTo - noteFrom - 1];
}

std::vector<double> Scale::tuneScale(const double& weightLimit, const double& trueRootNote) const
{
    std::vector<std::vector<double>> tunings(size(), std::vector<double>(size()));

    for (auto rootNote{ 0 }; rootNote != size(); ++rootNote)
        for (auto note{ 0 }; note != size(); ++note)
            tunings[rootNote][note] = rootNote == note ? 1 : makeTuning(rootNote, note, weightLimit);

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

        trueTuning[note] = std::pow(trueTuning[note], (double)1 / (double)size());
    }

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