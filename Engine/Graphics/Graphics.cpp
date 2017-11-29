// Include Files
//==============

#include "Graphics.h"

#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "cSamplerState.h"
#include "sContext.h"
#include "ColorFormats.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/UserOutput/UserOutput.h>
#include <Engine/Gameobject/cGameobject2D.h>
#include <Engine/Gameobject/cGameobject3D.h>
#include <Engine/Camera/cbCamera.h>
#include <algorithm>
#include <utility>


// Static Data Initialization
//===========================

namespace
{
	// Constant buffer object
	eae6320::Graphics::cConstantBuffer s_constantBuffer_perFrame(eae6320::Graphics::ConstantBufferTypes::PerFrame);
	eae6320::Graphics::cConstantBuffer s_constantBuffer_perDrawCall(eae6320::Graphics::ConstantBufferTypes::PerDrawCall);
	// In our class we will only have a single sampler state
	eae6320::Graphics::cSamplerState s_samplerState;

	// Submission Data
	//----------------

	// This struct's data is populated at submission time;
	// it must cache whatever is necessary in order to render a frame
	struct sDataRequiredToRenderAFrame
	{
		std::vector<eae6320::Gameobject::cGameobject2D*> gameobjects2D_perFrame;
		std::vector<std::pair<eae6320::Gameobject::cGameobject3D*, eae6320::Graphics::ConstantBufferFormats::sPerDrawCall>> gameobjects3D_opaque_perFrame;
		std::vector<std::pair<eae6320::Gameobject::cGameobject3D*, eae6320::Graphics::ConstantBufferFormats::sPerDrawCall>> gameobjects3D_translucent_perFrame;
		eae6320::Graphics::ConstantBufferFormats::sPerFrame constantData_perFrame;
		eae6320::Graphics::ColorFormats::sColor clearColor_perFrame;
		float clearDepth_perFrame;

		void CleanUp()
		{
			{
				const auto length = gameobjects3D_opaque_perFrame.size();
				for (size_t i = 0; i < length; i++)
				{
					// Clean up 3d opaque gameobject
					gameobjects3D_opaque_perFrame[i].first->DecrementReferenceCount();
				}
				gameobjects3D_opaque_perFrame.clear();
			}
			{
				const auto length = gameobjects3D_translucent_perFrame.size();
				for (size_t i = 0; i < length; i++)
				{
					// Clean up 3d translucent gameobject
					gameobjects3D_translucent_perFrame[i].first->DecrementReferenceCount();
				}
				gameobjects3D_translucent_perFrame.clear();
			}
			{
				const auto length = gameobjects2D_perFrame.size();
				for (size_t i = 0; i < length; i++)
				{
					// Clean up 2d gameobject
					gameobjects2D_perFrame[i]->DecrementReferenceCount();
				}
				gameobjects2D_perFrame.clear();
			}
		}
	};
	// In our class there will be two copies of the data required to render a frame:
	//	* One of them will be getting populated by the data currently being submitted by the application loop thread
	//	* One of them will be fully populated, 
	sDataRequiredToRenderAFrame s_dataRequiredToRenderAFrame[2];
	auto* s_dataBeingSubmittedByApplicationThread = &s_dataRequiredToRenderAFrame[0];
	auto* s_dataBeingRenderedByRenderThread = &s_dataRequiredToRenderAFrame[1];
	// The following two events work together to make sure that
	// the main/render thread and the application loop thread can work in parallel but stay in sync:
	// This event is signaled by the application loop thread when it has finished submitting render data for a frame
	// (the main/render thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenAllDataHasBeenSubmittedFromApplicationThread;
	// This event is signaled by the main/render thread when it has swapped render data pointers.
	// This means that the renderer is now working with all the submitted data it needs to render the next frame,
	// and the application loop thread can start submitting data for the following frame
	// (the application loop thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenDataForANewFrameCanBeSubmittedFromApplicationThread;
}

// Interface
//==========

// Submission
//-----------

void eae6320::Graphics::SubmitElapsedTime(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_perFrame = s_dataBeingSubmittedByApplicationThread->constantData_perFrame;
	constantData_perFrame.g_elapsedSecondCount_systemTime = i_elapsedSecondCount_systemTime;
	constantData_perFrame.g_elapsedSecondCount_simulationTime = i_elapsedSecondCount_simulationTime;
}

void eae6320::Graphics::SubmitClearColor(const ColorFormats::sColor i_clearColor)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	s_dataBeingSubmittedByApplicationThread->clearColor_perFrame = i_clearColor;
}

void eae6320::Graphics::SubmitClearDepth(const float i_depth)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	s_dataBeingSubmittedByApplicationThread->clearDepth_perFrame = i_depth;
}

void eae6320::Graphics::SubmitCamera(Camera::cbCamera*const& i_camera)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);

	EAE6320_ASSERT(i_camera);

	auto& constantData_perFrame = s_dataBeingSubmittedByApplicationThread->constantData_perFrame;
	constantData_perFrame.g_transform_worldToCamera = Math::cMatrixTransformation::CreateWorldToCameraTransform(i_camera->m_predictionTransform.orientation, i_camera->m_predictionTransform.position);
	constantData_perFrame.g_transform_cameraToProjected = i_camera->m_projectedTransformPerspective;
}

void eae6320::Graphics::SubmitGameobject2D(Gameobject::cGameobject2D*const& i_gameObject2D)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);

	EAE6320_ASSERT(i_gameObject2D);

	i_gameObject2D->IncrementReferenceCount();

	s_dataBeingSubmittedByApplicationThread->gameobjects2D_perFrame.push_back(i_gameObject2D);
}

void eae6320::Graphics::SubmitGameobject3D(Gameobject::cGameobject3D*const& i_gameObject3D)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);

	EAE6320_ASSERT(i_gameObject3D);

	i_gameObject3D->IncrementReferenceCount();

	ConstantBufferFormats::sPerDrawCall constantData_perDrawCall;
	constantData_perDrawCall.g_transform_localToWorld = Math::cMatrixTransformation(i_gameObject3D->m_predictionTransform.orientation, i_gameObject3D->m_predictionTransform.position);

	auto& constantData_perFrame = s_dataBeingSubmittedByApplicationThread->constantData_perFrame;
	constantData_perDrawCall.g_transform_localToProjected = constantData_perFrame.g_transform_cameraToProjected * Math::cMatrixTransformation::ConcatenateAffine(constantData_perFrame.g_transform_worldToCamera, constantData_perDrawCall.g_transform_localToWorld);

	i_gameObject3D->IsOpaque() ?
		s_dataBeingSubmittedByApplicationThread->gameobjects3D_opaque_perFrame.push_back(std::make_pair(i_gameObject3D, constantData_perDrawCall)) :
		s_dataBeingSubmittedByApplicationThread->gameobjects3D_translucent_perFrame.push_back(std::make_pair(i_gameObject3D, constantData_perDrawCall));
}

eae6320::cResult eae6320::Graphics::WaitUntilDataForANewFrameCanBeSubmitted(const unsigned int i_timeToWait_inMilliseconds)
{
	return WaitForEvent(s_whenDataForANewFrameCanBeSubmittedFromApplicationThread, i_timeToWait_inMilliseconds);
}

eae6320::cResult eae6320::Graphics::SignalThatAllDataForAFrameHasBeenSubmitted()
{
	return s_whenAllDataHasBeenSubmittedFromApplicationThread.Signal();
}

void eae6320::Graphics::RenderFrame()
{
	// Wait for the application loop to submit data to be rendered
	{
		const auto result = WaitForEvent(s_whenAllDataHasBeenSubmittedFromApplicationThread);
		if (result)
		{
			// Switch the render data pointers so that
			// the data that the application just submitted becomes the data that will now be rendered
			std::swap(s_dataBeingSubmittedByApplicationThread, s_dataBeingRenderedByRenderThread);
			// Once the pointers have been swapped the application loop can submit new data
			const auto result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Signal();
			if (!result)
			{
				EAE6320_ASSERTF(false, "Couldn't signal that new graphics data can be submitted");
				Logging::OutputError("Failed to signal that new render data can be submitted");
				UserOutput::Print("The renderer failed to signal to the application that new graphics data can be submitted."
					" The application is probably in a bad state and should be exited");
				return;
			}
		}
		else
		{
			EAE6320_ASSERTF(false, "Waiting for the graphics data to be submitted failed");
			Logging::OutputError("Waiting for the application loop to submit data to be rendered failed");
			UserOutput::Print("The renderer failed to wait for the application to submit data to be rendered."
				" The application is probably in a bad state and should be exited");
			return;
		}
	}

	EAE6320_ASSERT(s_dataBeingRenderedByRenderThread);

	auto& g_context = sContext::g_context;

	// Every frame an entirely new image will be created.
	// Before drawing anything, then, the previous image will be erased
	// by "clearing" the image buffer (filling it with a solid color)
	// and by "clearing" the depth buffer
	g_context.ClearImageBuffer(s_dataBeingRenderedByRenderThread->clearColor_perFrame);
	g_context.ClearDepthBuffer(s_dataBeingRenderedByRenderThread->clearDepth_perFrame);

	// Update the per-frame constant buffer
	{
		// Copy the data from the system memory that the application owns to GPU memory
		auto& constantData_perFrame = s_dataBeingRenderedByRenderThread->constantData_perFrame;
		s_constantBuffer_perFrame.Update(&constantData_perFrame);
	}

	// Bind and draw opaque 3d gameobjects
	{
		const auto length = s_dataBeingRenderedByRenderThread->gameobjects3D_opaque_perFrame.size();
		for (size_t i = 0; i < length; i++)
		{
			auto& constantData_perDrawCall = s_dataBeingRenderedByRenderThread->gameobjects3D_opaque_perFrame[i].second;
			s_constantBuffer_perDrawCall.Update(&constantData_perDrawCall);
			s_dataBeingRenderedByRenderThread->gameobjects3D_opaque_perFrame[i].first->BindAndDraw();
		}
	}

	// Bind and draw translucent 3d gameobjects
	{
		sort(s_dataBeingRenderedByRenderThread->gameobjects3D_translucent_perFrame.begin(), s_dataBeingRenderedByRenderThread->gameobjects3D_translucent_perFrame.end(),
			[](const auto& i_this, const auto& i_other)
		{
			const auto& transform_worldToCamera = s_dataBeingRenderedByRenderThread->constantData_perFrame.g_transform_worldToCamera;

			const auto& this_translationVectorInCameraSpace = transform_worldToCamera*i_this.second.g_transform_localToWorld.GetTranslation();
			const auto& other_translationVectorInCameraSpace = transform_worldToCamera*i_other.second.g_transform_localToWorld.GetTranslation();

			return this_translationVectorInCameraSpace.z < other_translationVectorInCameraSpace.z;
		});
		const auto length = s_dataBeingRenderedByRenderThread->gameobjects3D_translucent_perFrame.size();
		for (size_t i = 0; i < length; i++)
		{
			auto& constantData_perDrawCall = s_dataBeingRenderedByRenderThread->gameobjects3D_translucent_perFrame[i].second;
			s_constantBuffer_perDrawCall.Update(&constantData_perDrawCall);
			s_dataBeingRenderedByRenderThread->gameobjects3D_translucent_perFrame[i].first->BindAndDraw();
		}
	}

	// Bind and draw 2d gameobjects
	{
		const auto length = s_dataBeingRenderedByRenderThread->gameobjects2D_perFrame.size();
		for (size_t i = 0; i < length; i++)
		{
			s_dataBeingRenderedByRenderThread->gameobjects2D_perFrame[i]->BindAndDraw();
		}
	}

	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it the contents of the back buffer must be "presented"
	// (or "swapped" with the "front buffer")
	g_context.BufferSwap();

	// Once everything has been drawn the data that was submitted for this frame
	// should be cleaned up and cleared.
	// so that the struct can be re-used (i.e. so that data for a new frame can be submitted to it)
	s_dataBeingRenderedByRenderThread->CleanUp();
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::Initialize(const sInitializationParameters& i_initializationParameters)
{
	cResult result;

	// Initialize the platform-specific context
	if (!((result = sContext::g_context.Initialize(i_initializationParameters))))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	// Initialize the asset managers
	{
		if (!((result = cShader::s_manager.Initialize())))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}

		if (!((result = cEffect::s_manager.Initialize())))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}

		if (!((result = cTexture::s_manager.Initialize())))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}

		if (!((result = cMaterial::s_manager.Initialize())))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}

		if (!((result = cMesh::s_manager.Initialize())))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	// Initialize the platform-independent graphics objects
	{
		if ((result = s_constantBuffer_perFrame.Initialize()))
		{
			// There is only a single per-frame constant buffer that is re-used
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_perFrame.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				ShaderTypes::Vertex | ShaderTypes::Fragment);
		}
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		if ((result = s_constantBuffer_perDrawCall.Initialize()))
		{
			// There is only a single per-drawcall constant buffer that is re-used
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_perDrawCall.Bind(
				// In our class only vertex shader uses per-drawcall constant data
				ShaderTypes::Vertex);
		}
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		if ((result = s_samplerState.Initialize()))
		{
			// There is only a single sampler state that is re-used
			// and so it can be bound at initialization time and never unbound
			s_samplerState.Bind();
		}
		else
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}
	// Initialize the events
	{
		if (!((result = s_whenAllDataHasBeenSubmittedFromApplicationThread.Initialize(Concurrency::EventType::RESET_AUTOMATICALLY_AFTER_BEING_SIGNALED))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		if (!((result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Initialize(Concurrency::EventType::RESET_AUTOMATICALLY_AFTER_BEING_SIGNALED,
			Concurrency::EventState::SIGNALED))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::Graphics::CleanUp()
{
	auto result = Results::success;
	s_dataBeingSubmittedByApplicationThread->CleanUp();
	s_dataBeingRenderedByRenderThread->CleanUp();

	{
		const auto localResult = s_constantBuffer_perFrame.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	{
		const auto localResult = s_constantBuffer_perDrawCall.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	{
		const auto localResult = s_samplerState.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	{
		const auto localResult = cTexture::s_manager.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	{
		const auto localResult = cEffect::s_manager.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	{
		const auto localResult = cMesh::s_manager.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	{
		const auto localResult = cShader::s_manager.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	{
		const auto localResult = cMaterial::s_manager.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}
	{
		const auto localResult = sContext::g_context.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}

	return result;
}