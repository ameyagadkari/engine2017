/*
A gameobject 3d class is a representation for a 3D gameobject on screen
*/

#ifndef EAE6320_GAMEOBJECT_CGAMEOBJECT3D_H
#define EAE6320_GAMEOBJECT_CGAMEOBJECT3D_H

// Include Files
//==============

#include <Engine/Transform/sTransform.h>
#include <Engine/Transform/sPredictionTransform.h>
#include <Engine/Graphics/cMaterial.h>
#include <Engine/Graphics/cMesh.h>
#include <ExampleGame_/Gameplay/cDefaultGameobjectController.h>

namespace eae6320
{
	namespace Gameplay
	{
		class cbController;
		enum eControllerType : uint8_t
		{
			NO_CONTROLLER,
			DEFAULT_GAMEOBJECT_CONTROLLER
		};
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

			static cResult Load(const char* const i_path, cGameobject3D*& o_gameobject3D, const Math::sVector& i_position, char const * const i_meshPath, char const * const i_materialPath, const Gameplay::eControllerType i_controllerType);

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cGameobject3D);

			// Reference Counting
			//-------------------

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

			cResult CleanUp();

			//Update
			//------

			void UpdateBasedOnSimulationInput() const;
			void UpdateBasedOnSimulationTime(const float i_elapsedSecondCount_sinceLastUpdate);
			void PredictSimulationBasedOnElapsedTime(const float i_elapsedSecondCount_sinceLastSimulationUpdate);

			// Render
			//-------

			void BindAndDraw() const;

		private:

			// Initialization / Clean Up
			//--------------------------
			explicit cGameobject3D(const Math::sVector& i_position, const Gameplay::eControllerType i_controllerType);
			~cGameobject3D() { CleanUp(); }

			// Data
			//=====

		public:
			Transform::sPredictionTransform m_predictionTransform;
		private:
			Transform::sTransform m_transform;
			Gameplay::cbController* m_controller;
			Graphics::cMesh::Handle m_mesh;
			Graphics::cMaterial::Handle m_material;
			EAE6320_ASSETS_DECLAREREFERENCECOUNT();
		};
	}
}

#endif // EAE6320_GAMEOBJECT_CGAMEOBJECT3D_H