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
	constexpr auto s_velocityMagnitude = 1.0f;
}

// Interface
//==========

void eae6320::Camera::cFirstPersonCamera::UpdatePosition(const float i_elapsedSecondCount_sinceLastUpdate)
{
	m_rigidBodyState.UpdatePosition(i_elapsedSecondCount_sinceLastUpdate, m_transform);
}

void eae6320::Camera::cFirstPersonCamera::UpdateOrientation(const float i_elapsedSecondCount_sinceLastUpdate)
{
	m_rigidBodyState.UpdateOrientation(i_elapsedSecondCount_sinceLastUpdate, m_transform);
}

void eae6320::Camera::cFirstPersonCamera::UpdatePosition()
{
	const auto isWPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::W);
	const auto isSPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::S);
	const auto isAPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::A);
	const auto isDPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::D);
	const auto isQPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::Q);
	const auto isEPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::E);

	auto isThereInput = false;

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
	if (!(isQPressed && isEPressed))
	{
		if (isQPressed)
		{
			isThereInput = true;
			m_rigidBodyState.velocity += m_transform.localAxes.up;
		}
		if (isEPressed)
		{
			isThereInput = true;
			m_rigidBodyState.velocity -= m_transform.localAxes.up;
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

void eae6320::Camera::cFirstPersonCamera::UpdateOrientation()
{
	const auto isTPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::T);
	const auto isGPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::G);
	const auto isFPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::F);
	const auto isHPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::H);
	const auto isRPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::R);
	const auto isYPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::Y);

	auto isThereInput = false;

	if (!(isTPressed && isGPressed))
	{
		if (isTPressed)
		{
			isThereInput = true;
			m_rigidBodyState.angularVelocity_axis_local += m_transform.localAxes.forward;
		}
		if (isGPressed)
		{
			isThereInput = true;
			m_rigidBodyState.angularVelocity_axis_local -= m_transform.localAxes.forward;
		}
	}
	if (!(isHPressed && isFPressed))
	{
		if (isHPressed)
		{
			isThereInput = true;
			m_rigidBodyState.angularVelocity_axis_local += m_transform.localAxes.right;
		}
		if (isFPressed)
		{
			isThereInput = true;
			m_rigidBodyState.angularVelocity_axis_local -= m_transform.localAxes.right;
		}
	}
	if (!(isRPressed && isYPressed))
	{
		if (isRPressed)
		{
			isThereInput = true;
			m_rigidBodyState.angularVelocity_axis_local += m_transform.localAxes.up;
		}
		if (isYPressed)
		{
			isThereInput = true;
			m_rigidBodyState.angularVelocity_axis_local -= m_transform.localAxes.up;
		}
	}
	if (isThereInput)
	{
		m_rigidBodyState.angularVelocity_axis_local.Normalize();
	}
	else
	{
		m_rigidBodyState.angularVelocity_axis_local = Math::sVector::zero;
	}
}

void eae6320::Camera::cFirstPersonCamera::PredictPosition(const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	m_predictionTransform.position = m_transform.position;
	m_predictionTransform.position = m_rigidBodyState.PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate, m_predictionTransform);
}

void eae6320::Camera::cFirstPersonCamera::PredictOrientation(const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	m_predictionTransform.orientation = m_transform.orientation;
	m_predictionTransform.orientation = m_rigidBodyState.PredictFutureOrientation(i_elapsedSecondCount_sinceLastSimulationUpdate, m_predictionTransform);
}