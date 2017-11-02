// Include Files
//==============

#include "cbCamera.h"

#include <Engine/Math/Functions.h>
#include <Engine/UserSettings/UserSettings.h>

// Static Data Initialization
//===========================

namespace
{
	auto s_isInitialized = false;
	uint16_t s_width = 0;
	uint16_t s_height = 0;
}

// Implementation
//===============

// Initialization / Clean Up
//--------------------------

eae6320::Camera::cbCamera::cbCamera(const Math::sVector i_position, const Math::sVector i_orientationEular, const float i_fieldOfView, const float i_nearPlaneDistance, const float i_farPlaneDistance)
	:
	m_transform(i_position, i_orientationEular),
	m_fieldOfView(Math::ConvertDegreesToRadians(i_fieldOfView)),
	m_nearPlaneDistance(i_nearPlaneDistance),
	m_farPlaneDistance(i_farPlaneDistance)
{
	m_predictionTransform.position = m_transform.position;
	m_predictionTransform.orientation = m_transform.orientationQuaternion;

	if (!s_isInitialized)
	{
		UserSettings::GetDesiredInitialResolutionWidth(s_width);
		UserSettings::GetDesiredInitialResolutionHeight(s_height);
		EAE6320_ASSERT(s_width && s_height);
		s_isInitialized = true;
	}
	m_aspectRatio = static_cast<float>(s_width) / s_height;

	m_projectedTransformPerspective = Math::cMatrixTransformation::CreateCameraToProjectedTransformPerspective(m_fieldOfView, m_aspectRatio, m_nearPlaneDistance, m_farPlaneDistance);
}
