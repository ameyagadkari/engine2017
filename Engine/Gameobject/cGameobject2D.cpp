// Include Files
//==============

#include "cGameobject2D.h"
#include <Engine/Graphics/cSprite.h>

// Static Data Initialization
//===========================

eae6320::Assets::cManager<eae6320::Gameobject::cGameobject2D> eae6320::Gameobject::cGameobject2D::s_manager;

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::Gameobject::cGameobject2D::cGameobject2D(const int16_t i_x, const int16_t i_y, const uint16_t i_width, const uint16_t i_height, const Transform::eAnchor i_anchor)
	:
	m_rectTransform(i_x, i_y, i_width, i_height, i_anchor),
	m_sprite(nullptr)
{}

eae6320::cResult eae6320::Gameobject::cGameobject2D::Load(const char* const i_path, cGameobject2D*& o_gameobject2D, const int16_t i_x, const int16_t i_y, const uint16_t i_width, const uint16_t i_height, const Transform::eAnchor i_anchor, const char* const i_effectPath, const std::string& i_vertexShaderName, const std::string& i_fragmentShaderName, const uint8_t i_renderState, const char* const i_texturePath)
{
	auto result = Results::success;

	//Platform::sDataFromFile dataFromFile;

	cGameobject2D* newGameobject2D = nullptr;

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

	// Allocate a new GameObject 2D
	{
		newGameobject2D = new (std::nothrow) cGameobject2D(i_x, i_y, i_width, i_height, i_anchor);
		if (!newGameobject2D)
		{
			result = Results::outOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the effect %s", i_path);
			Logging::OutputError("Failed to allocate memory for the effect %s", i_path);
			goto OnExit;
		}
	}
	//if (!(result = newEffect->Initialize(i_path, dataFromFile)))

	// Load the effect
	if (!((result = Graphics::cEffect::s_manager.Load(i_effectPath, newGameobject2D->m_effectHandle, i_vertexShaderName, i_fragmentShaderName, i_renderState))))
	{
		EAE6320_ASSERTF(false, "Loading of effect failed: \"%s\"", i_effectPath);
		goto OnExit;
	}

	// Load the texture
	if (!((result = Graphics::cTexture::s_manager.Load(i_texturePath, newGameobject2D->m_textureHandle))))
	{
		EAE6320_ASSERTF(false, "Loading of texture failed: \"%s\"", i_texturePath);
		goto OnExit;
	}

	// Load the sprite
	{
		if (!((result = Graphics::cSprite::Load(newGameobject2D->m_sprite, newGameobject2D->m_rectTransform))))
		{
			EAE6320_ASSERTF(false, "Loading of sprite failed");
			goto OnExit;
		}
	}

OnExit:

	if (result)
	{
		EAE6320_ASSERT(newGameobject2D);
		o_gameobject2D = newGameobject2D;
	}
	else
	{
		if (newGameobject2D)
		{
			newGameobject2D->DecrementReferenceCount();
			newGameobject2D = nullptr;
		}
		o_gameobject2D = nullptr;
	}

	//dataFromFile.Free();

	return result;
}

eae6320::cResult eae6320::Gameobject::cGameobject2D::CleanUp()
{
	auto result = Results::success;

	// Effect Clean Up
	if (m_effectHandle)
	{
		const auto localResult = Graphics::cEffect::s_manager.Release(m_effectHandle);
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}

	// Texture Clean Up
	if (m_textureHandle)
	{
		const auto localResult = Graphics::cTexture::s_manager.Release(m_textureHandle);
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}

	// Sprite Clean Up
	if (m_sprite)
	{
		m_sprite->DecrementReferenceCount();
	}
	return result;
}

// Render
//-------

void eae6320::Gameobject::cGameobject2D::BindAndDraw() const
{
	Graphics::cEffect::s_manager.Get(m_effectHandle)->Bind();
	Graphics::cTexture::s_manager.Get(m_textureHandle)->Bind(0);
	m_sprite->Draw();
}
