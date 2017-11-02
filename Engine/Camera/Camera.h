/*
This file's functions provide access to the current camera
They also provide methods to circulate through all the cameras and change the current camera
*/

#ifndef EAE6320_APPLICATION_CAMERA_H
#define EAE6320_APPLICATION_CAMERA_H

// Include Files
//==============

#include <Engine/Results/Results.h>

// Forward Declarations
//=====================

namespace eae6320
{
	namespace Camera
	{
		class cbCamera;
	}
}

namespace eae6320
{
	namespace Camera
	{

		// Interface
		//==========

		void ChangeCurrentCamera();
		void AddNewCamera(cbCamera*const& i_camera);
		cbCamera*const& GetCurrentCamera();
		cResult CleanUp();
	}
}

#endif // EAE6320_APPLICATION_CAMERA_H
