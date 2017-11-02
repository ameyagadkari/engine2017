/*
This is the base class for a camera
It exposes virtual platform-independent functions that actual camera classes can implement for specific functionality
It also provides an interface to store multiple cameras
*/

#ifndef EAE6320_APPLICATION_CBCAMERA_H
#define EAE6320_APPLICATION_CBCAMERA_H

// Include Files
//==============

#include <Engine/Transform/sTransform.h>
#include <Engine/Transform/sPredictionTransform.h>
#include <Engine/Math/cMatrix_transformation.h>

namespace eae6320
{
	namespace Camera
	{
		// Class Declaration
		//==================

		class cbCamera
		{
		public:
			// Data
			//=====

			Math::cMatrixTransformation m_projectedTransformPerspective;
			Transform::sPredictionTransform m_predictionTransform;

			// Initialization / Clean Up
			//--------------------------

			virtual ~cbCamera() = default;

			// Interface
			//==========

			virtual void UpdatePosition(const float i_elapsedSecondCount_sinceLastUpdate) = 0;
			virtual void UpdateOrientation(const float i_elapsedSecondCount_sinceLastUpdate) = 0;
			virtual void UpdatePosition() = 0;
			virtual void UpdateOrientation() = 0;
			virtual void PredictPosition(const float i_elapsedSecondCount_sinceLastSimulationUpdate) = 0;
			virtual void PredictOrientation(const float i_elapsedSecondCount_sinceLastSimulationUpdate) = 0;
		protected:
			// Initialization / Clean Up
			//--------------------------

			explicit cbCamera(
				const Math::sVector i_position = Math::sVector::zero,
				const Math::sVector i_orientationEular = Math::sVector::zero,
				const float i_fieldOfView = 45.0f,
				const float i_nearPlaneDistance = 0.1f,
				const float i_farPlaneDistance = 100.0f);

			// Data
			//=====

			Transform::sTransform m_transform;
			float m_fieldOfView;
			float m_nearPlaneDistance;
			float m_farPlaneDistance;
			float m_aspectRatio;
		};
	}
}

#endif	// EAE6320_APPLICATION_CBCAMERA_H
