// Include Files
//==============

#include "cExampleGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Transform/sRectTransform.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Graphics/Graphics.h>

namespace
{
	bool isPaused = false;
}

// Inherited Implementation
//=========================

// Run
//----

void eae6320::cExampleGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Escape))
	{
		// Exit the application
		const auto result = Exit(EXIT_SUCCESS);
		EAE6320_ASSERT(result);
	}

	// Is the user pressing the P key?
	if (UserInput::IsKeyPressedOnce(UserInput::KeyCodes::P))
	{
		if (!isPaused)
		{
			// Pause the application
			SetSimulationRate(0.0f);
			isPaused = true;
		}
		else
		{
			// UnPause the application
			SetSimulationRate(1.0f);
			isPaused = false;
		}
	}

	// Game is not paused
	if (!isPaused)
	{
		if (UserInput::IsKeyPressed(UserInput::KeyCodes::S))
		{
			SetSimulationRate(0.5f);
		}
		else
		{
			SetSimulationRate(1.0f);
		}
	}
}

void eae6320::cExampleGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	// Submit Clear Color to Graphics
	{
		Graphics::ColorFormats::sColor clearColor;
		if (!(clearColor.SetColor(0.0f, 0.0f, 1.0f)))
		{
			EAE6320_ASSERT(false);
			Logging::OutputError("All the SetColor parameters must be [0.0, 1.0]");
		}
		Graphics::SubmitClearColor(clearColor);
	}
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cExampleGame::Initialize()
{
	uint16_t currentResolutionWidth, currentResolutionHeight;
	const auto result = GetCurrentResolution(currentResolutionWidth, currentResolutionHeight);
	if (result)
	{
		eae6320::Transform::SetCurrentResolution(currentResolutionWidth, currentResolutionHeight);
	}
	return result;
}

eae6320::cResult eae6320::cExampleGame::CleanUp()
{
	return Results::Success;
}
