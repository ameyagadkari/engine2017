/*
A gameobject 3d class is a representation for a 3D gameobject on screen
*/

#ifndef EAE6320_GAMEOBJECT_CGAMEOBJECT3D_H
#define EAE6320_GAMEOBJECT_CGAMEOBJECT3D_H

// Include Files
//==============

#include <Engine/Transform/sTransform.h>
#include <Engine/Graphics/cEffect.h>

// Forward Declarations
//=====================

namespace eae6320
{
	namespace Graphics
	{
		namespace HelperStructs 
		{
			struct sMeshData;
		}
		class cMesh;
	}
}

// Class Declaration
//==================

namespace eae6320
{
	namespace Gameobject
	{
		class cGameobject3D
		{
			// Interface
			//==========

		public:

			// Initialization / Clean Up
			//--------------------------

			static cResult Load(const char* const i_path, cGameobject3D*& o_gameobject3D,const Math::sVector& i_position,const Graphics::HelperStructs::sMeshData& i_meshData, char* const i_effectPath, const std::string& i_vertexShaderName, const std::string& i_fragmentShaderName, const uint8_t i_renderState);

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cGameobject3D);

			// Reference Counting
			//-------------------

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

			cResult CleanUp();

			// Render
			//-------

			void BindAndDraw() const;

		private:

			// Initialization / Clean Up
			//--------------------------
			explicit cGameobject3D(const Math::sVector& i_position);
			~cGameobject3D() { CleanUp(); }

			// Data
			//=====

		public:
			Transform::sTransform m_transform;
		private:
			Graphics::cMesh* m_mesh;
			Graphics::cEffect::Handle m_effect;
			EAE6320_ASSETS_DECLAREREFERENCECOUNT();
		};
	}
}

#endif // EAE6320_GAMEOBJECT_CGAMEOBJECT3D_H