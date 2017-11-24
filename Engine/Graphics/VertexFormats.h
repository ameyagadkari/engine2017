/*
	A vertex format determines the layout of the geometric data
	that the CPU will send to the GPU
*/

#ifndef EAE6320_GRAPHICS_VERTEX_FORMATS_H
#define EAE6320_GRAPHICS_VERTEX_FORMATS_H

// Include Files
//==============

#include <cstdint>

// Vertex Formats
//===============

namespace eae6320
{
	namespace Graphics
	{
		namespace VertexFormats
		{
			enum eLayoutType : uint8_t
			{
				UNKNOWN,
				SPRITE,
				MESH
			};

			struct sSprite
			{
				// POSITION
				// 2 floats == 8 bytes
				// Offset = 0
				float x, y;

				// TEXTURE_COORDINATES
				// 2 uint16_t == 4 bytes
				// Offset = 8
				uint16_t u, v;
			};

			struct sMesh
			{
				// POSITION
				// 3 floats == 12 bytes
				// Offset = 0
				float x, y, z;

				// NORMAL
				// 3 floats == 12 bytes
				// Offset = 12
				float nx, ny, nz;

				// TEXTURE_COORDINATES
				// 2 uint16_t == 4 bytes
				// Offset = 24
				uint16_t u, v;

				// COLOR
				// 4 uint8_t == 4 bytes
				// Offset = 28
				uint8_t r, g, b, a;
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
		namespace VertexFormats
		{
			extern eLayoutType g_layoutType;
		}
	}
}

#endif	// EAE6320_GRAPHICS_VERTEX_FORMATS_H
