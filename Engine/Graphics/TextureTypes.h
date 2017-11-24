/*
A texture type determines what sort of information is stored in the texture.
*/

#ifndef EAE6320_GRAPHICS_TEXTURETYPES_H
#define EAE6320_GRAPHICS_TEXTURETYPES_H

// Include Files
//==============

#include <cstdint>

// Texture Types
//==============

namespace eae6320
{
	namespace Graphics
	{
		namespace TextureUnit
		{
			// This is the location where [TextureType] will be bound in the shader

			enum eUnitNumber : uint8_t
			{
				COLOR,
				SPECULAR,
				NORMAL,
				BUMP,
				EMISSIVE,
				DISPLACEMENT,
				DISTORTION,

				COUNT
			};
		}
		namespace TextureTypes
		{
			// There are several types of textures which are used to store information about a mesh/sprite
			// They are generally called maps and are used for different purposes mostly in the fragment(pixel) shader

			enum eType : uint8_t
			{
				COLOR = 1u << TextureUnit::COLOR,
				SPECULAR = 1u << TextureUnit::SPECULAR,
				NORMAL = 1u << TextureUnit::NORMAL,
				BUMP = 1u << TextureUnit::BUMP,
				EMISSIVE = 1u << TextureUnit::EMISSIVE,
				DISPLACEMENT = 1u << TextureUnit::DISPLACEMENT,
				DISTORTION = 1u << TextureUnit::DISTORTION,
			};

			// Color Map

			inline bool IsColorMapPresent(const uint8_t i_textureTypeMask)
			{
				return (i_textureTypeMask & COLOR) != 0;
			}
			inline void AddColorMap(uint8_t& io_textureTypeMask)
			{
				io_textureTypeMask |= COLOR;
			}
			inline void RemoveColorMap(uint8_t& io_textureTypeMask)
			{
				io_textureTypeMask &= ~COLOR;
			}

			// Specular Map

			inline bool IsSpecularMapPresent(const uint8_t i_textureTypeMask)
			{
				return (i_textureTypeMask & SPECULAR) != 0;
			}
			inline void AddSpecularMap(uint8_t& io_textureTypeMask)
			{
				io_textureTypeMask |= SPECULAR;
			}
			inline void RemoveSpecularMap(uint8_t& io_textureTypeMask)
			{
				io_textureTypeMask &= ~SPECULAR;
			}

			// Normal Map

			inline bool IsNormalMapPresent(const uint8_t i_textureTypeMask)
			{
				return (i_textureTypeMask & NORMAL) != 0;
			}
			inline void AddNormalMap(uint8_t& io_textureTypeMask)
			{
				io_textureTypeMask |= NORMAL;
			}
			inline void RemoveNormalMap(uint8_t& io_textureTypeMask)
			{
				io_textureTypeMask &= ~NORMAL;
			}

			// Bump Map

			inline bool IsBumpMapPresent(const uint8_t i_textureTypeMask)
			{
				return (i_textureTypeMask & BUMP) != 0;
			}
			inline void AddBumpMap(uint8_t& io_textureTypeMask)
			{
				io_textureTypeMask |= BUMP;
			}
			inline void RemoveBumpMap(uint8_t& io_textureTypeMask)
			{
				io_textureTypeMask &= ~BUMP;
			}

			// Emissive Map

			inline bool IsEmissiveMapPresent(const uint8_t i_textureTypeMask)
			{
				return (i_textureTypeMask & EMISSIVE) != 0;
			}
			inline void AddEmissiveMap(uint8_t& io_textureTypeMask)
			{
				io_textureTypeMask |= EMISSIVE;
			}
			inline void RemoveEmissiveMap(uint8_t& io_textureTypeMask)
			{
				io_textureTypeMask &= ~EMISSIVE;
			}

			// Displacement Map

			inline bool IsDisplacementMapPresent(const uint8_t i_textureTypeMask)
			{
				return (i_textureTypeMask & DISPLACEMENT) != 0;
			}
			inline void AddDisplacementMap(uint8_t& io_textureTypeMask)
			{
				io_textureTypeMask |= DISPLACEMENT;
			}
			inline void RemoveDisplacementMap(uint8_t& io_textureTypeMask)
			{
				io_textureTypeMask &= ~DISPLACEMENT;
			}

			// Distortion Map

			inline bool IsDistortionMapPresent(const uint8_t i_textureTypeMask) 
			{
				return (i_textureTypeMask & DISTORTION) != 0;
			}
			inline void AddDistortionMap(uint8_t& io_textureTypeMask) 
			{
				io_textureTypeMask |= DISTORTION;
			}
			inline void RemoveDistortionMap(uint8_t& io_textureTypeMask)
			{
				io_textureTypeMask &= ~DISTORTION;
			}
		}
	}
}

#endif	//EAE6320_GRAPHICS_TEXTURETYPES_H
