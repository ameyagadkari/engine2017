/*
This file manages the postion and orientation of 3D game entities
*/

#ifndef EAE6320_STRANSFORM_H
#define EAE6320_STRANSFORM_H


// Include Files
//==============

#include "sLocalAxes.h"

#include <Engine/Math/sVector.h>
#include <Engine/Math/cQuaternion.h>

namespace eae6320
{
    namespace Transform
    {
        struct sTransform
        {
            // Interface
            //==========

            // Initialization / Clean Up
            //--------------------------

            sTransform();
            sTransform(const Math::sVector i_position, const Math::cQuaternion i_orientation);

            // Data
            //=====

            sLocalAxes localAxes;
            Math::sVector position;
            Math::cQuaternion orientation;

            // Helper Functions Declaration
            //-----------------------------

            void UpdateLocalAxes();
        };
    }
}

#endif    // EAE6320_STRANSFORM_H
