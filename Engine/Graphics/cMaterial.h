/*
A material is an encapsulation for a texture, a constant buffer and an effect.
*/
#ifndef EAE6320_GRAPHICS_CMATERIAL_H
#define EAE6320_GRAPHICS_CMATERIAL_H

// Include Files
//==============

#include "cEffect.h"
#include "cTexture.h"
#include "cConstantBuffer.h"
#include "TextureTypes.h"

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cMaterial
		{
			// Interface
			//==========

		public:
			// Assets
			//-------
			using Handle = Assets::cHandle<cMaterial>;
			static Assets::cManager<cMaterial> s_manager;

			// Initialization / Clean Up
			//--------------------------

			static cResult Load(const char* const i_path, cMaterial*& o_material);

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cMaterial);

			// Reference Counting
			//-------------------

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

			// Render
			//-------

			void Bind() const;

			// Opaque Check
			//-------------

			bool IsOpaque() const { return m_pEffect->IsOpaque(); }

			// Data
			//=====

		private:

			cConstantBuffer m_constantBuffer_perMaterial;
			struct
			{
				cTexture* m_pTexture = nullptr;
				cTexture::Handle m_texture;
				TextureUnit::eUnitNumber m_unitNumber = TextureUnit::UNKNOWN;
			}m_maps[TextureUnit::COUNT];
			cEffect::Handle m_effect;
			cEffect* m_pEffect = nullptr;
			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

			// Implementation
			//===============

		private:

			// Initialization / Clean Up Platform Independent
			//-----------------------------------------------

			cMaterial() :m_constantBuffer_perMaterial(ConstantBufferTypes::PerMaterial) {}
			~cMaterial();

			cResult Initialize(char const*const i_effectPath, char const*const*const i_texturepaths, void const*const i_data);
			cResult CleanUp();

		};
	}
}

#endif	// EAE6320_GRAPHICS_CMATERIAL_H
