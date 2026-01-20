#pragma once
#include "PitchSpace.h"

namespace BaseScales
{
    PitchSpace chromatic{
        {
            Fraction{ 17, 16 },
            Fraction{ 9, 8 },
            Fraction{ 6, 5 },
            Fraction{ 5, 4 },
            Fraction{ 4, 3 },
            Fraction{ 7, 5 },
            Fraction{ 3, 2 },
            Fraction{ 8, 5 },
            Fraction{ 5, 3 },
            Fraction{ 9, 5 },
            Fraction{ 15, 8 },
            Fraction{ 2 }
        } };

    PitchSpace sevenEDO{
        {
            Fraction{ 10, 9 },
            Fraction{ 11, 9 },
            Fraction{ 4, 3 },
            Fraction{ 3, 2 },
            Fraction{ 18, 11 },
            Fraction{ 9, 5 },
            Fraction{ 2 }
        } };
}