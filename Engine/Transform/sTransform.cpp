// Include Files
//==============

#include "sTransform.h"
#include <Engine/Math/cMatrix_transformation.h>

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::Transform::sTransform::sTransform() :
    position(Math::sVector::zero),
    orientation(Math::cQuaternion())
{
    UpdateLocalAxes();
}

eae6320::Transform::sTransform::sTransform(const Math::sVector i_position, const Math::cQuaternion i_orientation) :
    position(i_position),
    orientation(i_orientation)
{
    UpdateLocalAxes();
}

// Helper Functions Definition
//----------------------------

void eae6320::Transform::sTransform::UpdateLocalAxes()
{
    auto localToWorld = Math::cMatrixTransformation(orientation, position);
    localAxes.forward = -localToWorld.GetBackDirection();
    localAxes.right = localToWorld.GetRightDirection();
    localAxes.up = localToWorld.GetUpDirection();
}
