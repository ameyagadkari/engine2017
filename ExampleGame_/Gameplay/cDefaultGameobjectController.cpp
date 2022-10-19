// Include Files
//==============

#include "cDefaultGameobjectController.h"

#include <Engine/UserInput/UserInput.h>
#include <Engine/Transform/sTransform.h>
#include <Engine/Transform/sPredictionTransform.h>

// Static Data Initialization
//===========================

namespace
{
    constexpr auto s_velocityMagnitude = 1.0f;
}

// Interface
//==========

void eae6320::Gameplay::DefaultController::UpdatePosition(const float i_elapsedSecondCount_sinceLastUpdate, Transform::sTransform& io_transform)
{
    m_rigidBodyState.UpdatePosition(i_elapsedSecondCount_sinceLastUpdate, io_transform);
}

void eae6320::Gameplay::DefaultController::UpdateOrientation(const float i_elapsedSecondCount_sinceLastUpdate, Transform::sTransform& io_transform)
{
    m_rigidBodyState.UpdateOrientation(i_elapsedSecondCount_sinceLastUpdate, io_transform);
}

void eae6320::Gameplay::DefaultController::UpdatePosition(const Transform::sTransform& i_transform)
{
    const auto isUpPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::UP);
    const auto isDownPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::DOWN);
    const auto isLeftPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::LEFT);
    const auto isRightPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::RIGHT);

    auto isThereInput = false;

    if (!(isUpPressed && isDownPressed))
    {
        if (isUpPressed)
        {
            isThereInput = true;
            m_rigidBodyState.velocity += Math::sVector::up;
        }
        if (isDownPressed)
        {
            isThereInput = true;
            m_rigidBodyState.velocity -= Math::sVector::up;
        }
    }
    if (!(isRightPressed && isLeftPressed))
    {
        if (isRightPressed)
        {
            isThereInput = true;
            m_rigidBodyState.velocity += Math::sVector::right;
        }
        if (isLeftPressed)
        {
            isThereInput = true;
            m_rigidBodyState.velocity -= Math::sVector::right;
        }
    }
    if (isThereInput)
    {
        m_rigidBodyState.velocity.Normalize();
        m_rigidBodyState.velocity *= s_velocityMagnitude;    
    }
    else
    {
        m_rigidBodyState.velocity = Math::sVector::zero;
    }
}
void eae6320::Gameplay::DefaultController::UpdateOrientation(const Transform::sTransform& i_transform)
{
    // No input based orientation update
}

void eae6320::Gameplay::DefaultController::UpdatePosition(const float i_elapsedSecondCount_sinceLastSimulationUpdate, Transform::sPredictionTransform& io_predictionTransform)
{
    io_predictionTransform.position = m_rigidBodyState.PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate, io_predictionTransform);
}
void eae6320::Gameplay::DefaultController::UpdateOrientation(const float i_elapsedSecondCount_sinceLastSimulationUpdate, Transform::sPredictionTransform& io_predictionTransform)
{
    io_predictionTransform.orientation = m_rigidBodyState.PredictFutureOrientation(i_elapsedSecondCount_sinceLastSimulationUpdate, io_predictionTransform);
}