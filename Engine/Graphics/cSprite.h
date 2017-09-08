#ifndef EAE6320_GRAPHICS_CSPRITE_H
#define EAE6320_GRAPHICS_CSPRITE_H

// Include Files
//==============

#include <Engine/Results/Results.h>
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
		struct sRectTransform;
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

			cResult Initialize(const Transform::sRectTransform& i_rectTransform);
			cResult CleanUp();

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

		};
	}
}

#endif	// EAE6320_GRAPHICS_CSPRITE_H
