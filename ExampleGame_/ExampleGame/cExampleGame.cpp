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
	bool s_isPaused = false;
	std::vector<eae6320::Gameobject::cGameobject2D::Handle> s_2D_GameObject;
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
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::SPACE))
	{
		//Change the bottom left sprite's texture and effect

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
	/*{
		Graphics::cEffect::Handle effect;
		if (!((result = Graphics::cEffect::s_manager.Load("fake_effect1_path", effect, "sprite.shd", "sprite.shd", alphaTransperancyAndDrawingBothSidedTrianglesEnabled))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cTexture::Handle texture;
		if (!((result = Graphics::cTexture::s_manager.Load("data/Textures/happy.btf", texture))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cSprite* sprite = nullptr;
		if (!((result = Graphics::cSprite::Load(sprite, 0, 0, 256, 256, Transform::MID_CENTER))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_effectTextureSpriteTuple.push_back(std::make_tuple(effect, texture, sprite));
	}
	{
		Graphics::cEffect::Handle effect;
		if (!((result = Graphics::cEffect::s_manager.Load("fake_effect1_path", effect, "sprite.shd", "sprite.shd", alphaTransperancyAndDrawingBothSidedTrianglesEnabled))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cTexture::Handle texture;
		if (!((result = Graphics::cTexture::s_manager.Load("data/Textures/sad.btf", texture))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cSprite* sprite = nullptr;
		if (!((result = Graphics::cSprite::Load(sprite, 0, 0, 128, 128, Transform::TOP_LEFT))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_effectTextureSpriteTuple.push_back(std::make_tuple(effect, texture, sprite));
	}
	{
		Graphics::cEffect::Handle effect;
		if (!((result = Graphics::cEffect::s_manager.Load("fake_effect1_path", effect, "sprite.shd", "sprite.shd", alphaTransperancyAndDrawingBothSidedTrianglesEnabled))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cTexture::Handle texture;
		if (!((result = Graphics::cTexture::s_manager.Load("data/Textures/sad.btf", texture))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cSprite* sprite = nullptr;
		if (!((result = Graphics::cSprite::Load(sprite, 0, 0, 128, 128, Transform::BOTTOM_RIGHT))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_effectTextureSpriteTuple.push_back(std::make_tuple(effect, texture, sprite));
	}
	{
		Graphics::cEffect::Handle effect;
		if (!((result = Graphics::cEffect::s_manager.Load("fake_effect1_path", effect, "sprite.shd", "sprite.shd", alphaTransperancyAndDrawingBothSidedTrianglesEnabled))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cTexture::Handle texture;
		if (!((result = Graphics::cTexture::s_manager.Load("data/Textures/smiling.btf", texture))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cSprite* sprite = nullptr;
		if (!((result = Graphics::cSprite::Load(sprite, 0, 0, 128, 128, Transform::TOP_RIGHT))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_effectTextureSpriteTuple.push_back(std::make_tuple(effect, texture, sprite));
	}
	{
		Graphics::cEffect::Handle effect;
		if (!((result = Graphics::cEffect::s_manager.Load("fake_effect1_path", effect, "sprite.shd", "sprite.shd", alphaTransperancyAndDrawingBothSidedTrianglesEnabled))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cTexture::Handle texture;
		if (!((result = Graphics::cTexture::s_manager.Load("data/Textures/smiling.btf", texture))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cSprite* sprite = nullptr;
		if (!((result = Graphics::cSprite::Load(sprite, 0, 0, 128, 128, Transform::BOTTOM_LEFT))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_effectTextureSpriteTuple.push_back(std::make_tuple(effect, texture, sprite));
	}*/
	{
	Gameobject::cGameobject2D::Handle gameobject2D;
		if (!((result = Gameobject::cGameobject2D::s_manager.Load("fake_go2d1_path", gameobject2D, 0, 0, 256, 256, Transform::MID_CENTER, "fake_effect1_path", "sprite.shd", "sprite.shd", alphaTransperancyAndDrawingBothSidedTrianglesEnabled, "data/Textures/happy.btf"))))
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
