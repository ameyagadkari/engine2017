#ifndef EAE6320_DEFAULT_GAMEOBJECT_CONTROLLER_H
#define EAE6320_DEFAULT_GAMEOBJECT_CONTROLLER_H

// Include Files
//==============

#include <Engine/Gameobject/cbController.h>
#include <Engine/Physics/sRigidBodyState.h>

namespace eae6320
{
	namespace Gameplay
	{
		// Class Declaration
		//==================

		class DefaultController final : cbController
		{
			// Initialization / Clean Up
			//--------------------------
		public:
			static DefaultController* Initialize() { return new DefaultController(); }
		private:
			DefaultController() = default;

			//Data
			//====

			Physics::sRigidBodyState m_rigidBodyState;

			// Interface
			//==========

			void UpdatePosition(const float i_elapsedSecondCount_sinceLastUpdate, Transform::sTransform& io_transform) override;
			void UpdateOrientation(const float i_elapsedSecondCount_sinceLastUpdate, Transform::sTransform& io_transform) override;
			void UpdatePosition(const Transform::sTransform& i_transform) override;
			void UpdateOrientation(const Transform::sTransform& i_transform) override;
			void UpdatePosition(const float i_elapsedSecondCount_sinceLastSimulationUpdate, Transform::sPredictionTransform& io_predictionTransform) override;
			void UpdateOrientation(const float i_elapsedSecondCount_sinceLastSimulationUpdate, Transform::sPredictionTransform& io_predictionTransform) override;
		};
	}
}

#endif // !EAE6320_DEFAULT_GAMEOBJECT_CONTROLLER_H
