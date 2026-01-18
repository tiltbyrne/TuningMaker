#include "BaseScales.h"

int main()
{
    const auto majorScale_2{ BaseScales::repeatBaseScaleIntervals(BaseScales::majorScale, 2) };

    const Scale scale{ BaseScales::baseFractionsToIntervalsWithUniformWeight(majorScale_2) };

    auto tuning{ scale.tuneScale(0.0001, 0) };

    for (auto& note : tuning)
		std::cout << centsFromRatio(note) << std::endl;
}