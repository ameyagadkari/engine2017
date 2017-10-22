/*
	A vertex format determines the layout of the geometric data
	that the CPU will send to the GPU
*/

#ifndef EAE6320_GRAPHICS_VERTEX_FORMATS_H
#define EAE6320_GRAPHICS_VERTEX_FORMATS_H

// Vertex Formats
//===============

namespace eae6320
{
	namespace Graphics
	{
		namespace VertexFormats
		{
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
				// 2 floats == 8 bytes
				// Offset = 0
				float x, y;

				// COLOR
				// 4 uint8_t == 4 bytes
				// Offset = 8
				uint8_t r, g, b, a;
			};
		}
	}
}

#endif	// EAE6320_GRAPHICS_VERTEX_FORMATS_H
