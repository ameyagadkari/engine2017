/*
    This file declares the external interface for the graphics system
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Include Files
//==============
#include "ColorFormats.h"

#include <Engine/Results/Results.h>

#include <cstdint>
#include <string>

#if defined( EAE6320_PLATFORM_WINDOWS )
#include <Engine/Windows/Includes.h>
#endif

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
    namespace Gameobject
    {
        class cGameobject2D;
        class cGameobject3D;
    }
}

namespace eae6320
{
    namespace Graphics
    {
        namespace ColorFormats
        {
            struct sColor;
        }
    }
}

// Interface
//==========

namespace eae6320
{
    namespace Graphics
    {
        // Submission
        //-----------

        // These functions should be called from the application (on the application loop thread)

        // As the class progresses you will add your own functions for submitting data,
        // but the following is an example (that gets called automatically)
        // of how the application submits the total elapsed times
        // for the frame currently being submitted
        void SubmitElapsedTime(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime);
        void SubmitClearColor(const ColorFormats::sColor i_clearColor = ColorFormats::sColor());
        void SubmitClearDepth(const float i_clearDepth = 1.0f);
        void SubmitCamera(Camera::cbCamera*const& i_camera);
        void SubmitGameobject2D(Gameobject::cGameobject2D*const& i_gameObject2D);
        void SubmitGameobject3D(Gameobject::cGameobject3D*const& i_gameObject3D);
        void SubmitScreenShotName(const std::string& i_filePath);

        // When the application is ready to submit data for a new frame
        // it should call this before submitting anything
        // (or, said another way, it is not safe to submit data for a new frame
        // until this function returns successfully)
        cResult WaitUntilDataForANewFrameCanBeSubmitted(const unsigned int i_timeToWait_inMilliseconds);
        // When the application has finished submitting data for a frame
        // it must call this function
        cResult SignalThatAllDataForAFrameHasBeenSubmitted();

        // Render
        //-------

        // This is called (automatically) from the main/render thread.
        // It will render a submitted frame as soon as it is ready
        // (i.e. as soon as SignalThatAllDataForAFrameHasBeenSubmitted() has been called)
        void RenderFrame();

        // Initialization / Clean Up
        //--------------------------

        struct sInitializationParameters
        {
#if defined( EAE6320_PLATFORM_WINDOWS )
            HWND mainWindow = nullptr;
            uint16_t resolutionWidth = 0, resolutionHeight = 0;
    #if defined( EAE6320_PLATFORM_GL )
                HINSTANCE thisInstanceOfTheApplication = nullptr;
    #endif
#endif
        };

        cResult Initialize(const sInitializationParameters& i_initializationParameters);
        cResult CleanUp();
    }
}

#endif    // EAE6320_GRAPHICS_H
