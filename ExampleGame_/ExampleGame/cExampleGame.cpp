// Include Files
//==============

#include "cExampleGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Gameobject/cGameobject2D.h>
#include <Engine/Graphics/Graphics.h>

#include <vector>

namespace
{
	std::vector<eae6320::Gameobject::cGameobject2D::Handle> s_2D_GameObject;
	auto s_isPaused = false;
	auto previousElapsedTime = 0.0f;
	auto currentElapsedTime = 0.0f;
}

// Inherited Implementation
//=========================

// Run
//----

void eae6320::cExampleGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::ESCAPE))
	{
		// Exit the application
		const auto result = Exit(EXIT_SUCCESS);
		EAE6320_ASSERT(result);
	}

	// Is the user pressing the P key?
	if (UserInput::IsKeyPressedOnce(UserInput::KeyCodes::P))
	{
		s_isPaused = !s_isPaused;
		s_isPaused ? SetSimulationRate(0.0f) : SetSimulationRate(1.0f);
	}

	// Game is not paused
	if (!s_isPaused)
	{
		// Change Simulation Rate based on whether the user is pressing the S key?
		UserInput::IsKeyPressed(UserInput::KeyCodes::S) ? SetSimulationRate(0.5f) : SetSimulationRate(1.0f);
	}

	// Is the user pressing the SPACE key?
	{
		//Change the bottom left sprite's texture
		Gameobject::cGameobject2D::s_manager.Get(s_2D_GameObject[4])->m_useAlternateTexture = UserInput::IsKeyPressed(UserInput::KeyCodes::SPACE);
	}
}

void eae6320::cExampleGame::UpdateBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	currentElapsedTime += i_elapsedSecondCount_sinceLastUpdate;
	if (currentElapsedTime - previousElapsedTime > 1.0f)
	{
		auto const& gameObject2D = Gameobject::cGameobject2D::s_manager.Get(s_2D_GameObject[3]);
		gameObject2D->m_useAlternateTexture = !gameObject2D->m_useAlternateTexture;
		previousElapsedTime = currentElapsedTime;
	}
}

void eae6320::cExampleGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	// Submit Clear Color to Graphics
	{
		Graphics::ColorFormats::sColor clearColor;
		if (!clearColor.SetColor(0.0f, 0.2f, 0.8f))
		{
			EAE6320_ASSERT(false);
			Logging::OutputError("All the SetColor parameters must be [0.0, 1.0]");
		}
		Graphics::SubmitClearColor(clearColor);
	}

	// Submit 2D Gameobjects
	{
		for (auto& _2DGameObject : s_2D_GameObject)
		{
			Graphics::SubmitGameobject2D(_2DGameObject);
		}
	}
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cExampleGame::Initialize()
{
	constexpr auto alphaTransperancyAndDrawingBothSidedTrianglesEnabled = 0x05;
	cResult result;
	{
		Gameobject::cGameobject2D::Handle gameobject2D;
		if (!((result = Gameobject::cGameobject2D::s_manager.Load("fake_go2d1_path", gameobject2D, 0, 0, 256, 256, Transform::MID_CENTER, "fake_effect1_path", "sprite.shd", "sprite.shd", alphaTransperancyAndDrawingBothSidedTrianglesEnabled, "data/Textures/happy.btf"))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_2D_GameObject.push_back(gameobject2D);
	}
	{
		Gameobject::cGameobject2D::Handle gameobject2D;
		if (!((result = Gameobject::cGameobject2D::s_manager.Load("fake_go2d2_path", gameobject2D, 0, 0, 128, 128, Transform::TOP_LEFT, "fake_effect1_path", "sprite.shd", "sprite.shd", alphaTransperancyAndDrawingBothSidedTrianglesEnabled, "data/Textures/sad.btf"))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_2D_GameObject.push_back(gameobject2D);
	}
	{
		Gameobject::cGameobject2D::Handle gameobject2D;
		if (!((result = Gameobject::cGameobject2D::s_manager.Load("fake_go2d3_path", gameobject2D, 0, 0, 128, 128, Transform::BOTTOM_RIGHT, "fake_effect1_path", "sprite.shd", "sprite.shd", alphaTransperancyAndDrawingBothSidedTrianglesEnabled, "data/Textures/sad.btf"))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_2D_GameObject.push_back(gameobject2D);
	}
	{
		Gameobject::cGameobject2D::Handle gameobject2D;
		if (!((result = Gameobject::cGameobject2D::s_manager.Load("fake_go2d4_path", gameobject2D, 0, 0, 128, 128, Transform::TOP_RIGHT, "fake_effect1_path", "sprite.shd", "sprite.shd", alphaTransperancyAndDrawingBothSidedTrianglesEnabled, "data/Textures/smiling.btf", "data/Textures/sad.btf"))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_2D_GameObject.push_back(gameobject2D);
	}
	{
		Gameobject::cGameobject2D::Handle gameobject2D;
		if (!((result = Gameobject::cGameobject2D::s_manager.Load("fake_go2d5_path", gameobject2D, 0, 0, 128, 128, Transform::BOTTOM_LEFT, "fake_effect1_path", "sprite.shd", "sprite.shd", alphaTransperancyAndDrawingBothSidedTrianglesEnabled, "data/Textures/smiling.btf", "data/Textures/sad.btf"))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_2D_GameObject.push_back(gameobject2D);
	}

OnExit:
	return result;
}

eae6320::cResult eae6320::cExampleGame::CleanUp()
{
	auto result = Results::success;
	for (auto& _2DGameObject : s_2D_GameObject)
	{
		// Clean up 2d gameobject
		{
			const auto localResult = Gameobject::cGameobject2D::s_manager.Release(_2DGameObject);
			if (!localResult)
			{
				EAE6320_ASSERT(false);
				if (result)
				{
					result = localResult;
				}
			}
		}
	}
	s_2D_GameObject.clear();
	return result;
}
