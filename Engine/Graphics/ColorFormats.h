/*
A color format determines the representation of color
*/

#ifndef EAE6320_GRAPHICS_COLORFORMATS_H
#define EAE6320_GRAPHICS_COLORFORMATS_H

// Include Files
//==============

#include "Configuration.h"

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
				sColor() :_r(1.0f), _g(1.0f), _b(1.0f), _a(1.0f) {}
				cResult SetColor(const float i_r, const float i_g, const float i_b, const float i_a = 1.0f);
				float r() const { return _r; }
				float g() const { return _g; }
				float b() const { return _b; }
				float a() const { return _a; }
			private:
				float _r, _g, _b, _a;
			};
		}
	}
}

#endif	// EAE6320_GRAPHICS_COLORFORMATS_H
