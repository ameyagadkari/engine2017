/*
This class represents behaviour of a first person camera in the game
*/

#ifndef EAE6320_APPLICATION_CFIRST_PERSON_CAMERA_H
#define EAE6320_APPLICATION_CFIRST_PERSON_CAMERA_H


// Include Files
//==============

#include "cbCamera.h"
#include <Engine/Physics/sRigidBodyState.h>

namespace eae6320
{
	namespace Camera
	{
		// Class Declaration
		//==================

		class cFirstPersonCamera final : cbCamera
		{
			// Initialization / Clean Up
			//--------------------------
		public:
			static cFirstPersonCamera* Initialize(
				const Math::sVector i_position = Math::sVector::zero,
				const Math::cQuaternion i_orientation = Math::cQuaternion(),
				const float i_fieldOfView = 45.0f,
				const float i_nearPlaneDistance = 0.1f,
				const float i_farPlaneDistance = 100.0f) {
				return new cFirstPersonCamera(i_position, i_orientation, i_fieldOfView, i_nearPlaneDistance, i_farPlaneDistance);
			}
		private:
			//Data
			//====

			Physics::sRigidBodyState m_rigidBodyState;


			// Initialization / Clean Up
			//--------------------------
			explicit cFirstPersonCamera(
				const Math::sVector i_position,
				const Math::cQuaternion i_orientation,
				const float i_fieldOfView,
				const float i_nearPlaneDistance,
				const float i_farPlaneDistance) :
				cbCamera(i_position, i_orientation, i_fieldOfView, i_nearPlaneDistance, i_farPlaneDistance) {}

			// Interface
			//==========

			void UpdatePosition(const float i_elapsedSecondCount_sinceLastUpdate) override;
			void UpdateOrientation(const float i_elapsedSecondCount_sinceLastUpdate) override;
			void UpdatePosition() override;
			void UpdateOrientation() override;
			void PredictPosition(const float i_elapsedSecondCount_sinceLastSimulationUpdate) override;
			void PredictOrientation(const float i_elapsedSecondCount_sinceLastSimulationUpdate) override;

		};
	}
}

#endif	// EAE6320_APPLICATION_CFIRST_PERSON_CAMERA_H

