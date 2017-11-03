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
	bool s_isFirstMouseMove = false;
	struct
	{
		short lastX, lastY;
	}mousePosition;
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
	const auto isAltPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::CONTROL);
	auto isThereInput = false;
	if (UserInput::g_isMouseTracked && isAltPressed)
	{
		if (s_isFirstMouseMove)
		{
			mousePosition.lastX = UserInput::g_mousePoints.x;
			mousePosition.lastY = UserInput::g_mousePoints.y;
			s_isFirstMouseMove = false;
		}
		const auto xOffset = UserInput::g_mousePoints.x - mousePosition.lastX;
		const auto yOffset = UserInput::g_mousePoints.y - mousePosition.lastY;
		mousePosition.lastX = UserInput::g_mousePoints.x;
		mousePosition.lastY = UserInput::g_mousePoints.y;
		if (xOffset > 0)
		{
			m_rigidBodyState.angularVelocity_axis_local -= Math::sVector::up;//m_transform.localAxes.up;
			isThereInput = true;
		}
		else if (xOffset < 0)
		{
			m_rigidBodyState.angularVelocity_axis_local += Math::sVector::up;//m_transform.localAxes.up;
			isThereInput = true;
		}
		if (yOffset > 0)
		{
			m_rigidBodyState.angularVelocity_axis_local -= Math::sVector::right;//m_transform.localAxes.right;
			isThereInput = true;
		}
		else if (yOffset < 0)
		{
			m_rigidBodyState.angularVelocity_axis_local += Math::sVector::right;//m_transform.localAxes.right;
			isThereInput = true;
		}
	}
	else
	{
		s_isFirstMouseMove = true;
	}
	if (isThereInput)
	{
		m_rigidBodyState.angularVelocity_axis_local.Normalize();
	}
	else
	{
		m_rigidBodyState.angularVelocity_axis_local = Math::sVector::zero;
	}
	/*const auto isTPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::T);
	const auto isGPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::G);
	const auto isFPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::F);
	const auto isHPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::H);
	const auto isRPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::R);
	const auto isYPressed = UserInput::IsKeyPressed(UserInput::KeyCodes::Y);

	auto isThereInput = false;

	/*if (!(isTPressed && isGPressed))
	{
		if (isTPressed)
		{
			isThereInput = true;
			m_rigidBodyState.angularVelocity_axis_local += Math::sVector::forward;//m_transform.localAxes.forward;
		}
		if (isGPressed)
		{
			isThereInput = true;
			m_rigidBodyState.angularVelocity_axis_local -= Math::sVector::forward; //m_transform.localAxes.forward;
		}
	}
	if (!(isHPressed && isFPressed))
	{
		if (isHPressed)
		{
			isThereInput = true;
			m_rigidBodyState.angularVelocity_axis_local += Math::sVector::right;//m_transform.localAxes.right;
		}
		if (isFPressed)
		{
			isThereInput = true;
			m_rigidBodyState.angularVelocity_axis_local -= Math::sVector::right; //m_transform.localAxes.right;
		}
	}
	if (!(isRPressed && isYPressed))
	{
		if (isRPressed)
		{
			isThereInput = true;
			m_rigidBodyState.angularVelocity_axis_local += Math::sVector::up; //m_transform.localAxes.up;
		}
		if (isYPressed)
		{
			isThereInput = true;
			m_rigidBodyState.angularVelocity_axis_local -= Math::sVector::up; //m_transform.localAxes.up;
		}
	}
	if (isThereInput)
	{
		m_rigidBodyState.angularVelocity_axis_local.Normalize();
	}
	else
	{
		m_rigidBodyState.angularVelocity_axis_local = Math::sVector::zero;
	}*/
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