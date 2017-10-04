// Include Files
//==============

#include "cSprite.h"
#include "VertexFormats.h"
#include "SpriteHelperStructs.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Transform/sRectTransform.h>

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cSprite::Load(cSprite*& o_sprite, const Transform::sRectTransform& i_rectTransform)
{
	cResult result;

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

	if (!((result = newSprite->Initialize(i_rectTransform))))
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

// Helper Functions Definition
//----------------------------

void eae6320::Graphics::cSprite::GenerateVertexData(const Transform::sRectTransform& i_rectTransform, VertexFormats::sSprite*const& o_vertexData)
{
	SpriteHelperStructs::sScreenPosition screenPosition;
	i_rectTransform.GetScreenPosition(screenPosition);
	{
		// Bottom Right
		{
			o_vertexData[0].x = screenPosition.right;
			o_vertexData[0].y = screenPosition.bottom;

			o_vertexData[0].u = SpriteHelperStructs::g_defaultMappedUVs.right;
			o_vertexData[0].v = SpriteHelperStructs::g_defaultMappedUVs.bottom;
		}
		// Top Right
		{
			o_vertexData[1].x = screenPosition.right;
			o_vertexData[1].y = screenPosition.top;

			o_vertexData[1].u = SpriteHelperStructs::g_defaultMappedUVs.right;
			o_vertexData[1].v = SpriteHelperStructs::g_defaultMappedUVs.top;
		}
		// Bottom Left
		{
			o_vertexData[2].x = screenPosition.left;
			o_vertexData[2].y = screenPosition.bottom;

			o_vertexData[2].u = SpriteHelperStructs::g_defaultMappedUVs.left;
			o_vertexData[2].v = SpriteHelperStructs::g_defaultMappedUVs.bottom;
		}
		// Top Left
		{
			o_vertexData[3].x = screenPosition.left;
			o_vertexData[3].y = screenPosition.top;

			o_vertexData[3].u = SpriteHelperStructs::g_defaultMappedUVs.left;
			o_vertexData[3].v = SpriteHelperStructs::g_defaultMappedUVs.top;
		}
	}
#if defined( EAE6320_PLATFORM_D3D ) 
	std::swap(o_vertexData[1], o_vertexData[2]);
#endif
}
