/*
A color format determines the representation of color
*/

#ifndef EAE6320_GRAPHICS_COLORFORMATS_H
#define EAE6320_GRAPHICS_COLORFORMATS_H

// Include Files
//==============

#include <Engine/Results/Results.h>

// Color Formats
//===============

namespace eae6320
{
    namespace Graphics
    {
        namespace ColorFormats
        {
            struct sColor
            {
                sColor() :r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
                sColor(const sColor&) = default;
                sColor& operator=(const sColor&) = default;
                cResult SetColor(const float i_r, const float i_g, const float i_b, const float i_a = 1.0f);
                float R() const { return r; }
                float G() const { return g; }
                float B() const { return b; }
                float A() const { return a; }
            private:
                float r, g, b, a;
            };
        }
    }
}

#endif    // EAE6320_GRAPHICS_COLORFORMATS_H
