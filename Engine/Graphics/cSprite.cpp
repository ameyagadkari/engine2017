// Include Files
//==============

#include "cSprite.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

// Static Data Initialization
//===========================

eae6320::Assets::cManager<eae6320::Graphics::cSprite> eae6320::Graphics::cSprite::s_manager;

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cSprite::Load(const char* const i_path, cSprite*& o_sprite, const Transform::sRectTransform& i_rectTransform)
{
	auto result = Results::Success;

	//Platform::sDataFromFile dataFromFile;
	cSprite* newSprite = nullptr;

	/*// Load the binary data
	{
	std::string errorMessage;
	if (!(result = Platform::LoadBinaryFile(i_path, dataFromFile, &errorMessage)))
	{
	EAE6320_ASSERTF(false, errorMessage.c_str());
	Logging::OutputError("Failed to load shader from file %s: %s", i_path, errorMessage.c_str());
	goto OnExit;
	}
	}*/
	// Allocate a new shader
	{
		newSprite = new (std::nothrow) cSprite();
		if (!newSprite)
		{
			result = Results::OutOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the sprite %s", i_path);
			Logging::OutputError("Failed to allocate memory for the sprite %s", i_path);
			goto OnExit;
		}
	}
	// Initialize the platform-specific graphics API shader object
	//if (!(result = newEffect->Initialize(i_path, dataFromFile)))
	if (!(result = newSprite->Initialize(i_rectTransform)))
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
	//dataFromFile.Free();

	return result;
}

eae6320::Graphics::cSprite::~cSprite()
{
	CleanUp();
}