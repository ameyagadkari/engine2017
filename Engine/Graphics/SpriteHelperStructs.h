#ifndef EAE6320_GRAPHICS_SPRITE_HELPER_STRUCTS_H
#define EAE6320_GRAPHICS_SPRITE_HELPER_STRUCTS_H

// Include Files
//==============

#include "Engine/Math/cHalf.h"

// Helper Structs for Sprites
//===========================
namespace eae6320
{
	namespace Graphics
	{
		namespace HelperStructs
		{
			// The positions and texture coordinates are stored as a quad
			// (meaning that only rectangular sprites can be made)

			struct sScreenPosition
			{
				// Left of screen = -1
				// Right of screen = 1
				// Top of screen = 1
				// Bottom of screen = -1

				float left, right, top, bottom;
			};

			struct sTextureCoordinates
			{
				// Left of texture = 0
				// Right of texture = 1
				// Top of texture = 1 (OpenGL-style)
				// Bottom of texture = 0 (OpenGL-style)

				uint16_t left, right, top, bottom;
				explicit sTextureCoordinates(const float i_left = 0.0f, const float i_right = 1.0f, const float i_top = 1.0f, const float i_bottom = 0.0f) :
					left(Math::cHalf::MakeHalfFromFloat(i_left)),
					right(Math::cHalf::MakeHalfFromFloat(i_right)),
#if defined( EAE6320_PLATFORM_D3D )
					top(Math::cHalf::MakeHalfFromFloat(1.0f - i_top)),
					bottom(Math::cHalf::MakeHalfFromFloat(1.0f - i_bottom))
#elif defined( EAE6320_PLATFORM_GL )
					top(Math::cHalf::MakeHalfFromFloat(i_top)),
					bottom(Math::cHalf::MakeHalfFromFloat(i_bottom))
#endif
				{}
			};
		}
	}
}

// Static Data Declaration
//========================
namespace eae6320
{
	namespace Graphics
	{
		namespace HelperStructs
		{
			// We can declare a global Texture Coordinates struct for UIs
			// which map the whole texture to the given quad

			extern sTextureCoordinates const g_defaultMappedUVs;
		}
	}
}

#endif	// EAE6320_GRAPHICS_SPRITE_HELPER_STRUCTS_H