#include "BaseScales.h"

int main()
{
    const Scale scale{ BaseScales::baseFractionsToIntervalsWithHarmonicWeight(
        BaseScales::repeatBaseScaleIntervals(BaseScales::sevenEDO, 5), 5) };

    auto tuning{ scale.tuneScale(0, 0.0001) };

    for (auto note{ 0 }; note != tuning.size(); ++note)
		std::cout << "Note " << note << ") " << centsFromRatio(tuning[note]) << std::endl;
}