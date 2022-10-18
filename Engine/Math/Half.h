/*
This file contains representation for a half(half of float)
*/

#ifndef EAE6320_MATH_HALF_H
#define EAE6320_MATH_HALF_H

#include <cstdint>

// Class Declaration
//==================

namespace eae6320
{
    namespace Math
    {

        // Interface
        //==========

        uint16_t MakeHalfFromFloat(const float i_value);
        float MakeFloatFromHalf(const uint16_t i_value);
    }
}

#endif //  EAE6320_MATH_HALF_H