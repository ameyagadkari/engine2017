#ifndef EAE6320_CBCONTROLLER_H
#define EAE6320_CBCONTROLLER_H

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

namespace eae6320
{
	namespace Gameplay
	{
		// Class Declaration
		//==================

		class cbController
		{
		public:
			// Initialization / Clean Up
			//--------------------------

			virtual ~cbController() = default;

			// Interface
			//==========

			virtual void UpdatePosition(const float i_elapsedSecondCount_sinceLastUpdate, Transform::sTransform& io_transform) = 0;
			virtual void UpdateOrientation(const float i_elapsedSecondCount_sinceLastUpdate, Transform::sTransform& io_transform) = 0;
			virtual void UpdatePosition(const Transform::sTransform& i_transform) = 0;
			virtual void UpdateOrientation(const Transform::sTransform& i_transform) = 0;
			virtual void UpdatePosition(const float i_elapsedSecondCount_sinceLastSimulationUpdate, Transform::sPredictionTransform& io_predictionTransform) = 0;
			virtual void UpdateOrientation(const float i_elapsedSecondCount_sinceLastSimulationUpdate, Transform::sPredictionTransform& io_predictionTransform) = 0;
		};
	}
}

#endif // !EAE6320_CBCONTROLLER_H