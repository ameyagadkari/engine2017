/*
An effect is an encapsulation for the shaders and the render state.
*/
#ifndef EAE6320_GRAPHICS_CEFFECT_H
#define EAE6320_GRAPHICS_CEFFECT_H

// Include Files
//==============

#include "cShader.h"
#include "cRenderState.h"

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cEffect
		{
			// Interface
			//==========

		public:
			// Assets
			//-------
			using Handle = Assets::cHandle<cEffect>;
			static Assets::cManager<cEffect> s_manager;

			// Initialization / Clean Up
			//--------------------------

			static cResult Load(const char* const i_path, cEffect*& o_effect);

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cEffect);

			// Reference Counting
			//-------------------

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

			// Render
			//-------

			void Bind() const;

			// Data
			//=====

		private:		

#if defined( EAE6320_PLATFORM_GL )
			GLuint m_programId = 0;
#endif
			cShader::Handle m_vertexShader;
			cShader::Handle m_fragmentShader;
			EAE6320_ASSETS_DECLAREREFERENCECOUNT();
			cRenderState m_renderState;

			// Implementation
			//===============

		private:

			// Initialization / Clean Up Platform Independent
			//-----------------------------------------------

			cEffect() = default;
			~cEffect();

			cResult Initialize(char const*const i_vertexShaderName, char const*const i_fragmentShaderName, const uint8_t i_renderState);
			cResult CleanUp();

			// Initialization / Clean Up Platform Dependent
			//---------------------------------------------

			cResult InitializePlatformSpecific();
			cResult CleanUpPlatformSpecific();

			// Render
			//-------

			void BindPlatformSpecific() const;

		};
	}
}

#endif	// EAE6320_GRAPHICS_CEFFECT_H
