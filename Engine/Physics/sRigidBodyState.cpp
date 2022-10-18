// Include Files
//==============

#include "sRigidBodyState.h"

#include <Engine/Transform/sTransform.h>
#include <Engine/Transform/sPredictionTransform.h>

// Interface
//==========

void eae6320::Physics::sRigidBodyState::UpdatePosition(const float i_secondCountToIntegrate, Transform::sTransform& io_transform) const
{
    // Update position
    {
        io_transform.position += velocity * i_secondCountToIntegrate;
    }
    // Update velocity
    {
        //velocity += acceleration * i_secondCountToIntegrate;
    }
}

void eae6320::Physics::sRigidBodyState::UpdateOrientation(const float i_secondCountToIntegrate, Transform::sTransform& io_transform) const
{
    // Update orientation
    {
        const auto rotation = Math::cQuaternion(angularSpeed * i_secondCountToIntegrate, angularVelocity_axis_local);
        io_transform.orientation = io_transform.orientation * rotation;
        io_transform.orientation.Normalize();
        io_transform.UpdateLocalAxes();
    }
}

eae6320::Math::sVector eae6320::Physics::sRigidBodyState::PredictFuturePosition(const float i_secondCountToExtrapolate, Transform::sPredictionTransform& io_predictionTransform) const
{
    return io_predictionTransform.position + (velocity * i_secondCountToExtrapolate);
}

eae6320::Math::cQuaternion eae6320::Physics::sRigidBodyState::PredictFutureOrientation(const float i_secondCountToExtrapolate, Transform::sPredictionTransform& io_predictionTransform) const
{
    const auto rotation = Math::cQuaternion(angularSpeed * i_secondCountToExtrapolate, angularVelocity_axis_local);
    return Math::cQuaternion(io_predictionTransform.orientation * rotation).GetNormalized();
}
