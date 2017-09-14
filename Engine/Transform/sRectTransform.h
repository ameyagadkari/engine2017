/*
This file manages the postion and orientation of 2D game entities such as sprites
*/

#ifndef EAE6320_SRECTTRANSFORM_H
#define EAE6320_SRECTTRANSFORM_H

// Include Files
//==============

#include <cstdint>
//#include <Engine/Results/Results.h>

// Anchor Types
//=============

namespace eae6320
{
	namespace Transform
	{
		// We define nine anchor positions on the screen
		enum eAnchor : uint8_t
		{
			TopLeft = 0,
			TopCentre = 1 << 0,
			TopRight = 1 << 1,
			MidLeft = 1 << 2,
			MidCentre = 1 << 3,
			MidRight = 1 << 4,
			BottomLeft = 1 << 5,
			BottomCentre = 1 << 6,
			BottomRight = 1 << 7,
		};
		// Helper struct to store screen position
		struct sScreenPosition
		{
			float left, right, top, bottom;
		};
	}
}

// Interface
//==========

namespace eae6320
{
	namespace Transform
	{
		struct sRectTransform
		{
			explicit sRectTransform();
			explicit sRectTransform(
				const int16_t i_x,
				const int16_t i_y,
				const uint16_t i_width,
				const uint16_t i_height,
				const eAnchor i_anchor);
			void GetScreenPosition(sScreenPosition& o_screenPosition) const { o_screenPosition = screenPosition; }
		private:
			sScreenPosition screenPosition;
			struct
			{
				int16_t x, y;
			}pixelCoordinates;
			uint16_t width, height;
			eAnchor anchor;
			void GenerateNewScreenCoordinates();
		};
	}
}

#endif	// EAE6320_SRECTTRANSFORM_H
