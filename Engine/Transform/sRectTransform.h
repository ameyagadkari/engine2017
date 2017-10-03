/*
This file manages the postion and orientation of 2D game entities such as sprites
*/

#ifndef EAE6320_SRECTTRANSFORM_H
#define EAE6320_SRECTTRANSFORM_H

// Include Files
//==============

#include <cstdint>

// Anchor Types
//=============

namespace eae6320
{
	namespace Transform
	{
		// We define nine anchor positions on the screen
		enum eAnchor : uint8_t
		{
			TOP_LEFT = 0,
			TOP_CENTER = 1 << 0,
			TOP_RIGHT = 1 << 1,
			MID_LEFT = 1 << 2,
			MID_CENTER = 1 << 3,
			MID_RIGHT = 1 << 4,
			BOTTOM_LEFT = 1 << 5,
			BOTTOM_CENTER = 1 << 6,
			BOTTOM_RIGHT = 1 << 7,
		};
		// Helper struct to store screen position
		struct sScreenPosition
		{
			float left, right, top, bottom;
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

			void GetScreenPosition(sScreenPosition& o_screenPosition) const { o_screenPosition = screenPosition; }

			// Data
			//=====

		private:
			sScreenPosition screenPosition;
			struct
			{
				int16_t x, y;
			}pixelCoordinates;
			uint16_t width, height;
			eAnchor anchor;

			// Initialization / Clean Up
			//--------------------------

			void GenerateNewScreenCoordinates();
		};
	}
}

#endif	// EAE6320_SRECTTRANSFORM_H
