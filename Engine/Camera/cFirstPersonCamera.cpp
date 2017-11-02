// Include Files
//==============

#include "cFirstPersonCamera.h"

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

void eae6320::Camera::cFirstPersonCamera::UpdatePosition(const float i_elapsedSecondCount_sinceLastUpdate)
{
	m_rigidBodyState.UpdatePosition(i_elapsedSecondCount_sinceLastUpdate, m_transform);
}

void eae6320::Camera::cFirstPersonCamera::UpdateOrientation(const float i_elapsedSecondCount_sinceLastUpdate)
{

}

void eae6320::Camera::cFirstPersonCamera::UpdatePosition()
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
			m_rigidBodyState.velocity += m_transform.localAxes.forward;
		}
		if (isSPressed)
		{
			isThereInput = true;
			m_rigidBodyState.velocity -= m_transform.localAxes.forward;
		}
	}
	if (!(isDPressed && isAPressed))
	{
		if (isDPressed)
		{
			isThereInput = true;
			m_rigidBodyState.velocity += m_transform.localAxes.right;
		}
		if (isAPressed)
		{
			isThereInput = true;
			m_rigidBodyState.velocity -= m_transform.localAxes.right;
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
void eae6320::Camera::cFirstPersonCamera::UpdateOrientation()
{

}

void eae6320::Camera::cFirstPersonCamera::PredictPosition(const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	m_predictionTransform.position = m_transform.position;
	m_predictionTransform.position = m_rigidBodyState.PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate, m_predictionTransform);
}
void eae6320::Camera::cFirstPersonCamera::PredictOrientation(const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	
}