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
	std::vector<std::pair<eae6320::Graphics::cEffect::Handle, eae6320::Graphics::cSprite::Handle>> s_effectSpritePairs;
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
			Graphics::SubmitEffectSpritePair(effectSpritePair.first, effectSpritePair.second);
		}
	}
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cExampleGame::Initialize()
{
	auto result = Results::Success;
	uint16_t currentResolutionWidth, currentResolutionHeight;
	result = GetCurrentResolution(currentResolutionWidth, currentResolutionHeight);
	if (result)
	{
		eae6320::Transform::SetCurrentResolution(currentResolutionWidth, currentResolutionHeight);
	}
	else
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	{
		Graphics::cEffect::Handle effect;
		if (!(result = Graphics::cEffect::s_manager.Load("fake_effect1_path", effect, "sprite.shd", "sprite_white.shd")))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cSprite::Handle sprite;
		Transform::sRectTransform spriteLocation(0, 0, 256, 256, Transform::MidCentre);
		if (!(result = Graphics::cSprite::s_manager.Load("fake_sprite1_path", sprite, spriteLocation)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_effectSpritePairs.push_back(std::make_pair(effect, sprite));
	}
	{
		Graphics::cEffect::Handle effect;
		if (!(result = Graphics::cEffect::s_manager.Load("fake_effect2_path", effect, "sprite.shd", "sprite_color.shd")))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cSprite::Handle sprite;
		Transform::sRectTransform spriteLocation(0, 0, 64, 64, Transform::TopLeft);
		if (!(result = Graphics::cSprite::s_manager.Load("fake_sprite2_path", sprite, spriteLocation)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_effectSpritePairs.push_back(std::make_pair(effect, sprite));
	}
	{
		Graphics::cEffect::Handle effect;
		if (!(result = Graphics::cEffect::s_manager.Load("fake_effect2_path", effect, "sprite.shd", "sprite_color.shd")))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cSprite::Handle sprite;
		Transform::sRectTransform spriteLocation(0, 0, 64, 64, Transform::BottomRight);
		if (!(result = Graphics::cSprite::s_manager.Load("fake_sprite3_path", sprite, spriteLocation)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_effectSpritePairs.push_back(std::make_pair(effect, sprite));
	}
	{
		Graphics::cEffect::Handle effect;
		if (!(result = Graphics::cEffect::s_manager.Load("fake_effect2_path", effect, "sprite.shd", "sprite_color.shd")))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cSprite::Handle sprite;
		Transform::sRectTransform spriteLocation(0, 0, 64, 64, Transform::TopRight);
		if (!(result = Graphics::cSprite::s_manager.Load("fake_sprite4_path", sprite, spriteLocation)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		s_effectSpritePairs.push_back(std::make_pair(effect, sprite));
	}
	{
		Graphics::cEffect::Handle effect;
		if (!(result = Graphics::cEffect::s_manager.Load("fake_effect2_path", effect, "sprite.shd", "sprite_color.shd")))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
		Graphics::cSprite::Handle sprite;
		Transform::sRectTransform spriteLocation(0, 0, 64, 64, Transform::BottomLeft);
		if (!(result = Graphics::cSprite::s_manager.Load("fake_sprite5_path", sprite, spriteLocation)))
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
	auto result = Results::Success;
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
			const auto localResult = Graphics::cSprite::s_manager.Release(effectSpritePair.second);
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
	s_effectSpritePairs.clear();
	return result;
}
