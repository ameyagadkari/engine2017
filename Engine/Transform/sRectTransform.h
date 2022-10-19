/*
This file manages the postion and orientation of 2D game entities such as sprites
*/

#ifndef EAE6320_SRECTTRANSFORM_H
#define EAE6320_SRECTTRANSFORM_H

// Include Files
//==============

#include <cstdint>

// Forward Declarations
//=====================

namespace eae6320
{
    namespace Graphics
    {
        namespace HelperStructs
        {
            struct sScreenPosition;
        }
    }
}

// Anchor Types
//=============

namespace eae6320
{
    namespace Transform
    {
        // We define nine anchor positions on the screen
        enum eAnchor : uint8_t
        {
            UNKNOWN,
            TOP_LEFT,
            TOP_CENTER,
            TOP_RIGHT,
            MID_LEFT,
            MID_CENTER,
            MID_RIGHT,
            BOTTOM_LEFT,
            BOTTOM_CENTER,
            BOTTOM_RIGHT,
        };
    }
}

namespace eae6320
{
    namespace Transform
    {
        struct sRectTransform
        {
            // Interface
            //==========

            // Initialization / Clean Up
            //--------------------------

            explicit sRectTransform();
            explicit sRectTransform(
                const int16_t i_x,
                const int16_t i_y,
                const uint16_t i_width,
                const uint16_t i_height,
                const eAnchor i_anchor);

            // Access
            //-------

            void GetScreenPosition(Graphics::HelperStructs::sScreenPosition& o_screenPosition) const;

            // Data
            //=====

        private:

            struct
            {
                int16_t x, y;
            }pixelCoordinates;
            uint16_t width, height;
            eAnchor anchor;
        };
    }
}

#endif    // EAE6320_SRECTTRANSFORM_H
