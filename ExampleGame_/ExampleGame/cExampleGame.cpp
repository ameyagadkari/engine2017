// Include Files
//==============

#include "cExampleGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Transform/sRectTransform.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Graphics/Graphics.h>

#include <vector>

namespace
{
	bool s_isPaused = false;
	std::vector<std::pair<eae6320::Graphics::cEffect::Handle, eae6320::Graphics::cSprite *const>> s_effectSpritePairs;
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
		UserInput::IsKeyPressed(UserInput::KeyCodes::S) ? SetSimulationRate(0.5f) : SetSimulationRate(1.0f);
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

	// Submit Effect/Sprite pair
	{
		for (auto& effectSpritePair : s_effectSpritePairs)
		{
			SubmitEffectSpritePair(effectSpritePair.first, effectSpritePair.second);
		}
	}
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cExampleGame::Initialize()
{
	cResult result;
	{
		Graphics::cEffect::Handle effect;
		if (!((result = Graphics::cEffect::s_manager.Load("fake_effect1_path", effect, "sprite.shd", "sprite_white.shd"))))
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
		s_effectSpritePairs.push_back(std::make_pair(effect, sprite));
	}
	{
		Graphics::cEffect::Handle effect;
		if (!((result = Graphics::cEffect::s_manager.Load("fake_effect2_path", effect, "sprite.shd", "sprite_color.shd"))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cSprite* sprite = nullptr;
		if (!((result = Graphics::cSprite::Load(sprite, 0, 0, 64, 64, Transform::TOP_LEFT))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_effectSpritePairs.push_back(std::make_pair(effect, sprite));
	}
	{
		Graphics::cEffect::Handle effect;
		if (!((result = Graphics::cEffect::s_manager.Load("fake_effect2_path", effect, "sprite.shd", "sprite_color.shd"))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cSprite* sprite = nullptr;
		if (!((result = Graphics::cSprite::Load(sprite, 0, 0, 64, 64, Transform::BOTTOM_RIGHT))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_effectSpritePairs.push_back(std::make_pair(effect, sprite));
	}
	{
		Graphics::cEffect::Handle effect;
		if (!((result = Graphics::cEffect::s_manager.Load("fake_effect2_path", effect, "sprite.shd", "sprite_color.shd"))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cSprite* sprite = nullptr;
		if (!((result = Graphics::cSprite::Load(sprite, 0, 0, 64, 64, Transform::TOP_RIGHT))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_effectSpritePairs.push_back(std::make_pair(effect, sprite));
	}
	{
		Graphics::cEffect::Handle effect;
		if (!((result = Graphics::cEffect::s_manager.Load("fake_effect2_path", effect, "sprite.shd", "sprite_color.shd"))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cSprite* sprite = nullptr;
		if (!((result = Graphics::cSprite::Load(sprite, 0, 0, 64, 64, Transform::BOTTOM_LEFT))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_effectSpritePairs.push_back(std::make_pair(effect, sprite));
	}

OnExit:
	return result;
}

eae6320::cResult eae6320::cExampleGame::CleanUp()
{
	auto result = Results::success;
	for (auto& effectSpritePair : s_effectSpritePairs)
	{
		// Clean up effect
		{
			const auto localResult = Graphics::cEffect::s_manager.Release(effectSpritePair.first);
			if (!localResult)
			{
				EAE6320_ASSERT(false);
				if (result)
				{
					result = localResult;
				}
			}
		}

		// Clean up sprite
		{
			effectSpritePair.second->DecrementReferenceCount();
		}
	}
	s_effectSpritePairs.clear();
	return result;
}
