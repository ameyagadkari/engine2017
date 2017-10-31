/*
This file keeps track of the postion and orientation of 3D game entities for where they should be rendered
*/

#ifndef EAE6320_SPREDICTIONTRANSFORM_H
#define EAE6320_SPREDICTIONTRANSFORM_H


// Include Files
//==============

#include <Engine/Math/sVector.h>
#include <Engine/Math/cQuaternion.h>

namespace eae6320
{
	namespace Transform
	{
		struct sPredictionTransform
		{
			// Interface
			//==========

			// Initialization / Clean Up
			//--------------------------

			sPredictionTransform() :position(Math::sVector::zero), orientation(Math::cQuaternion()) {}
			sPredictionTransform(const Math::sVector i_position, const Math::cQuaternion i_orientation) :
				position(i_position), orientation(i_orientation) {}

			// Data
			//=====

			Math::sVector position;
			Math::cQuaternion orientation;
		};
	}
}

#endif	// EAE6320_SPREDICTIONTRANSFORM_H
