/*
	A sprite is a 2d quad on screen
	(usually a used for UI elements of a game, unless its a 2d game)
 */

#ifndef EAE6320_GRAPHICS_CSPRITE_H
#define EAE6320_GRAPHICS_CSPRITE_H

 // Include Files
 //==============

#include <Engine/Assets/ReferenceCountedAssets.h>
#include <Engine/Results/Results.h>
#include <Engine/Math/cHalf.h>

#ifdef EAE6320_PLATFORM_GL
#include "OpenGL/Includes.h"
#endif

// Forward Declarations
//=====================

#if defined( EAE6320_PLATFORM_D3D )
struct ID3D11Buffer;
struct ID3D11InputLayout;
#endif

namespace eae6320
{
	namespace Transform
	{
		enum eAnchor : uint8_t;
		struct sRectTransform;
	}
}

namespace eae6320
{
	namespace Graphics
	{
		namespace VertexFormats
		{
			struct sSprite;
		}
	}
}

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cSprite
		{
			// Interface
			//==========

		public:

			// Initialization / Clean Up
			//--------------------------

			static cResult Load(cSprite*& o_sprite, const Transform::sRectTransform& i_rectTransform);

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cSprite);

			// Reference Counting
			//-------------------

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

			// Render
			//-------

			void Draw() const;

			// Data
			//=====

		private:
#if defined( EAE6320_PLATFORM_D3D ) 
			// A vertex buffer holds the data for each vertex
			ID3D11Buffer* m_vertexBuffer = nullptr;
			// D3D has an "input layout" object that associates the layout of the vertex format struct
			// with the input from a vertex shader
			ID3D11InputLayout* m_vertexInputLayout = nullptr;
#elif defined( EAE6320_PLATFORM_GL )
			// A vertex buffer holds the data for each vertex
			GLuint m_vertexBufferId = 0;
			// A vertex array encapsulates the vertex data as well as the vertex input layout
			GLuint m_vertexArrayId = 0;
#endif
			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

			// Implementation
			//===============

			// Initialization / Clean Up
			//--------------------------

			cSprite() = default;
			~cSprite();

			cResult Initialize(const Transform::sRectTransform& i_rectTransform);
			cResult CleanUp();

			// Helper Functions Declaration
			//-----------------------------

			static void GenerateVertexData(const Transform::sRectTransform& i_rectTransform, VertexFormats::sSprite*const&  o_vertexData);
		};
	}
}

#endif	// EAE6320_GRAPHICS_CSPRITE_H
