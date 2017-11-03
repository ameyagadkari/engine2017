/*
	This struct can be used to represent the state of a
	rigid body in 3D space
*/

#ifndef EAE6320_PHYSICS_SRIGIDBODYSTATE_H
#define EAE6320_PHYSICS_SRIGIDBODYSTATE_H

// Include Files
//==============

#include <Engine/Math/cQuaternion.h>
#include <Engine/Math/sVector.h>

// Forward Declarations
//=====================

namespace eae6320
{
	namespace Transform
	{
		struct sTransform;
		struct sPredictionTransform;
	}
}

// Struct Declaration
//===================

namespace eae6320
{
	namespace Physics
	{
		struct sRigidBodyState
		{
			// Data
			//=====

			//Math::sVector position;	// In arbitrary units determined by the applicaton's convention
			Math::sVector velocity;	// Distance per-second
			//Math::sVector acceleration;	// Distance per-second^2
			//Math::cQuaternion orientation;
			Math::sVector angularVelocity_axis_local = Math::sVector(0.0f, 0.0f, 0.0f);	// In local space (not world space)
			float angularSpeed = 0.5f;	// Radians per-second (positive values rotate right-handed, negative rotate left-handed)

			// Interface
			//==========

			void UpdatePosition(const float i_secondCountToIntegrate, Transform::sTransform& io_transform) const;
			void UpdateOrientation(const float i_secondCountToIntegrate, Transform::sTransform& io_transform) const;
			Math::sVector PredictFuturePosition(const float i_secondCountToExtrapolate, Transform::sPredictionTransform& io_predictionTransform) const;
			Math::cQuaternion PredictFutureOrientation(const float i_secondCountToExtrapolate, Transform::sPredictionTransform& io_predictionTransform) const;
		};
	}
}

#endif	// EAE6320_PHYSICS_SRIGIDBODYSTATE_H
