// Include Files
//==============

#include "Camera.h"
#include "cbCamera.h"

#include <Engine/UserInput/UserInput.h>

#include <vector>

// Static Data Initialization
//===========================

namespace
{
	size_t s_currentCameraNumber = 0;
	size_t s_maxCameraNumber = 0;
	std::vector<eae6320::Camera::cbCamera*> s_cameras;
	eae6320::Camera::cbCamera* s_currentCamera = nullptr;
}

// Interface
//==========

void eae6320::Camera::ChangeCurrentCamera()
{
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::C))
	{
		s_currentCameraNumber = (s_currentCameraNumber == s_maxCameraNumber - 1) ? 0 : ++s_currentCameraNumber;
		s_currentCamera = s_cameras[s_currentCameraNumber];
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::V))
	{
		s_currentCameraNumber = (!s_currentCameraNumber) ? s_maxCameraNumber - 1 : --s_currentCameraNumber;
		s_currentCamera = s_cameras[s_currentCameraNumber];
	}
}

void eae6320::Camera::AddNewCamera(cbCamera*const& i_camera)
{
	s_cameras.push_back(i_camera);
	s_maxCameraNumber = s_cameras.size();
	if (s_maxCameraNumber > 0 && !s_currentCamera)
	{
		s_currentCamera = s_cameras[0];
	}
}

eae6320::Camera::cbCamera*const& eae6320::Camera::GetCurrentCamera()
{
	return s_currentCamera;
}

eae6320::cResult eae6320::Camera::CleanUp()
{
	const auto result = Results::success;
	for (size_t i = 0; i < s_maxCameraNumber; i++)
	{
		if (s_cameras[i])
		{
			delete s_cameras[i];
			s_cameras[i] = nullptr;
		}
	}
	return result;
}