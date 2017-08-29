// Include Files
//==============

#include "cExampleGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>

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
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
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

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cExampleGame::Initialize()
{
	return Results::Success;
}

eae6320::cResult eae6320::cExampleGame::CleanUp()
{
	return Results::Success;
}
