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
	constexpr auto velocityMagnitude = 1.0f;
	bool isThereInput = false;
}

// Interface
//==========

void eae6320::Gameplay::DefaultController::UpdatePosition(const float i_elapsedSecondCount_sinceLastUpdate, Transform::sTransform& io_transform)
{
	m_rigidBodyState.UpdatePosition(i_elapsedSecondCount_sinceLastUpdate, io_transform);
}

void eae6320::Gameplay::DefaultController::UpdateOrientation(const float i_elapsedSecondCount_sinceLastUpdate, Transform::sTransform& io_transform)
{

}

void eae6320::Gameplay::DefaultController::UpdatePosition(const Transform::sTransform& i_transform)
{
	const auto isWPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::W);
	const auto isSPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::S);
	const auto isAPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::A);
	const auto isDPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::D);

	if (!(isWPressed && isSPressed))
	{
		if (isWPressed)
		{
			isThereInput = true;
			m_rigidBodyState.velocity += i_transform.localAxes.up;
		}
		if (isSPressed)
		{
			isThereInput = true;
			m_rigidBodyState.velocity -= i_transform.localAxes.up;
		}
	}
	if (!(isDPressed && isAPressed))
	{
		if (isDPressed)
		{
			isThereInput = true;
			m_rigidBodyState.velocity += i_transform.localAxes.right;
		}
		if (isAPressed)
		{
			isThereInput = true;
			m_rigidBodyState.velocity -= i_transform.localAxes.right;
		}
	}
	if (!isThereInput)
	{
		m_rigidBodyState.velocity = Math::sVector::zero;
	}
	else
	{
		m_rigidBodyState.velocity.Normalize();
		m_rigidBodyState.velocity *= velocityMagnitude;
		isThereInput = false;
	}
}
void eae6320::Gameplay::DefaultController::UpdateOrientation(const Transform::sTransform& i_transform)
{

}

void eae6320::Gameplay::DefaultController::UpdatePosition(const float i_elapsedSecondCount_sinceLastSimulationUpdate, Transform::sPredictionTransform& io_predictionTransform)
{
	io_predictionTransform.position = m_rigidBodyState.PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate, io_predictionTransform);
}
void eae6320::Gameplay::DefaultController::UpdateOrientation(const float i_elapsedSecondCount_sinceLastSimulationUpdate, Transform::sPredictionTransform& io_predictionTransform)
{
	
}