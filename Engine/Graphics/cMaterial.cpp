// Include Files
//==============

#include "cMaterial.h"

#include <Engine/Platform/Platform.h>
#include "ConstantBufferFormats.h"

// Static Data Initialization
//===========================

eae6320::Assets::cManager<eae6320::Graphics::cMaterial> eae6320::Graphics::cMaterial::s_manager;

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cMaterial::Load(const char* const i_path, cMaterial*& o_material)
{
	auto result = Results::success;

	Platform::sDataFromFile dataFromFile;
	char* effectPath = nullptr;
	char* texturePath = nullptr;
	ConstantBufferFormats::sPerMaterial constantData_perMaterial;
	uint8_t offsetToAdd = 0;
	cMaterial* newMaterial = nullptr;

	// Load the binary data
	{
		std::string errorMessage;
		if (!((result = LoadBinaryFile(i_path, dataFromFile, &errorMessage))))
		{
			EAE6320_ASSERTF(false, errorMessage.c_str());
			Logging::OutputError("Failed to load material from file %s: %s", i_path, errorMessage.c_str());
			goto OnExit;
		}
	}

	// Extract data from loaded effect file
	{
		// Casting data to uintptr_t for pointer arithematic

		auto data = reinterpret_cast<uintptr_t>(dataFromFile.data);

		// Extracting Material Constant Buffer Data

		constantData_perMaterial = *reinterpret_cast<ConstantBufferFormats::sPerMaterial*>(data);

		// Extracting Offset To Add

		data += sizeof(constantData_perMaterial);
		offsetToAdd = *reinterpret_cast<uint8_t*>(data);

		// Extracting Effect Path

		data += sizeof(offsetToAdd);
		effectPath = reinterpret_cast<char*>(data);

		// Extracting Offset To Add

		data += offsetToAdd;
		offsetToAdd = *reinterpret_cast<uint8_t*>(data);

		// Extracting Texture Path

		data += sizeof(offsetToAdd);
		texturePath = reinterpret_cast<char*>(data);
	}

	if (!((result = newMaterial->Initialize(effectPath, texturePath, &constantData_perMaterial))))
	{
		EAE6320_ASSERTF(false, "Initialization of new effect failed");
		goto OnExit;
	}

OnExit:

	if (result)
	{
		EAE6320_ASSERT(newMaterial);
		o_material = newMaterial;
	}
	else
	{
		if (newMaterial)
		{
			newMaterial->DecrementReferenceCount();
			newMaterial = nullptr;
		}
		o_material = nullptr;
	}
	dataFromFile.Free();

	return result;
}

// Render
//-------

void eae6320::Graphics::cMaterial::Bind() const
{
	// Binding the effect
	{
		EAE6320_ASSERT(m_effect);
		auto const*const effect = cEffect::s_manager.Get(m_effect);
		EAE6320_ASSERT(effect);
		effect->Bind();
	}

	// Binding the texture
	{
		EAE6320_ASSERT(m_texture);
		auto const*const texture = cTexture::s_manager.Get(m_texture);
		EAE6320_ASSERT(texture);
		texture->Bind(0);
	}

	// Binding the constant buffer
	{
		m_constantBuffer_perMaterial.Bind(
			// In our class only fragment shader uses per-material constant data
			ShaderTypes::Fragment);
	}
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cMaterial::Initialize(char const*const i_effectPath, char const*const i_texturepath, void const*const i_data)
{
	auto result = Results::success;

	if (!((result = cEffect::s_manager.Load(i_effectPath, m_effect))))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	if (!((result = cTexture::s_manager.Load(i_texturepath, m_texture))))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	if (!((result = m_constantBuffer_perMaterial.Initialize(i_data))))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

OnExit:
	return result;
}

eae6320::cResult eae6320::Graphics::cMaterial::CleanUp()
{
	auto result = Results::success;

	// Effect Clean Up
	if (m_effect)
	{
		const auto localResult = cEffect::s_manager.Release(m_effect);
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
	if (m_texture)
	{
		const auto localResult = cTexture::s_manager.Release(m_texture);
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}

	// Constant buffer clean up
	{
		const auto localResult = m_constantBuffer_perMaterial.CleanUp();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}

	return result;
}

// Implementation
//===============

// Initialization / Clean Up
//--------------------------

eae6320::Graphics::cMaterial::~cMaterial()
{
	CleanUp();
}