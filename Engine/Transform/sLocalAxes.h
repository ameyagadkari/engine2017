#ifndef EAE6320_SLOCALAXES_H
#define EAE6320_SLOCALAXES_H

// Include Files
//==============

#include <Engine/Math/sVector.h>

namespace eae6320
{
    namespace Transform
    {
        struct sLocalAxes
        {
            Math::sVector forward;
            Math::sVector up;
            Math::sVector right;
            sLocalAxes() :forward(Math::sVector::forward), up(Math::sVector::up), right(Math::sVector::right) {}
        };
    }
}

#endif // !EAE6320_SLOCALAXES_H