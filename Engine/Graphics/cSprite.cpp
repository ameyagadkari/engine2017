// Include Files
//==============

#include "cSprite.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Transform/sRectTransform.h>

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cSprite::Load(cSprite*& o_sprite, const int16_t i_x, const int16_t i_y, const uint16_t i_width, const uint16_t i_height, const Transform::eAnchor i_anchor)
{
	cResult result;

	const Transform::sRectTransform rectTransform(i_x, i_y, i_width, i_height, i_anchor);

	cSprite* newSprite = nullptr;

	// Allocate a new sprite
	{
		newSprite = new (std::nothrow) cSprite();
		if (!newSprite)
		{
			result = Results::outOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the sprite");
			Logging::OutputError("Failed to allocate memory for the sprite");
			goto OnExit;
		}
	}

	if (!((result = newSprite->Initialize(rectTransform))))
	{
		EAE6320_ASSERTF(false, "Initialization of new effect failed");
		goto OnExit;
	}

OnExit:

	if (result)
	{
		EAE6320_ASSERT(newSprite);
		o_sprite = newSprite;
	}
	else
	{
		if (newSprite)
		{
			newSprite->DecrementReferenceCount();
			newSprite = nullptr;
		}
		o_sprite = nullptr;
	}

	return result;
}

eae6320::Graphics::cSprite::~cSprite()
{
	CleanUp();
}