// Include Files
//==============

#include "sTransform.h"
#include <Engine/Math/Functions.h>

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::Transform::sTransform::sTransform() :
	position(Math::sVector::zero),
	orientationEular(Math::sVector::zero)
{
	UpdateLocalAxes();
}

eae6320::Transform::sTransform::sTransform(const Math::sVector i_position, const Math::sVector i_orientationEular) :
	position(i_position),
	orientationEular(i_orientationEular)
{
	UpdateLocalAxes();
}

// Helper Functions Definition
//----------------------------

void eae6320::Transform::sTransform::UpdateLocalAxes()
{
	orientationQuaternion =
		Math::cQuaternion(Math::ConvertDegreesToRadians(orientationEular.x), Math::sVector::right)*
		Math::cQuaternion(Math::ConvertDegreesToRadians(orientationEular.y), Math::sVector::up)*
		Math::cQuaternion(Math::ConvertDegreesToRadians(orientationEular.z), Math::sVector::forward);

	auto newForward = orientationQuaternion.GetInverse()*Math::sVector::back;

	localAxes.forward = newForward.GetNormalized();
	localAxes.right = Cross(localAxes.forward, Math::sVector::up).GetNormalized();
	localAxes.up = Cross(localAxes.right, localAxes.forward).GetNormalized();
}
