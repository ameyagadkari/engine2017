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
	uint8_t textureTypeMask = 0u;
	ConstantBufferFormats::sPerMaterial constantData_perMaterial;
	uint8_t offsetToAdd = 0u;
	cMaterial* newMaterial = nullptr;
	char** texturePaths = nullptr;

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

	// Allocate a new material
	{
		newMaterial = new (std::nothrow) cMaterial();
		if (!newMaterial)
		{
			result = Results::outOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the material %s", i_path);
			Logging::OutputError("Failed to allocate memory for the material %s", i_path);
			goto OnExit;
		}
	}

	texturePaths = new (std::nothrow) char*[TextureUnit::COUNT]();
	if (!texturePaths)
	{
		result = Results::outOfMemory;
		EAE6320_ASSERTF(false, "Couldn't allocate memory for the all texture path pointers %s", i_path);
		Logging::OutputError("Failed to allocate memory for the all texture path pointers %s", i_path);
		goto OnExit;
	}

	// Extract data from loaded material file
	{
		// Casting data to uintptr_t for pointer arithematic

		auto currentOffset = reinterpret_cast<uintptr_t>(dataFromFile.data);
		const auto finalOffset = currentOffset + dataFromFile.size;

		// Extracting Material Constant Buffer Data

		constantData_perMaterial = *reinterpret_cast<ConstantBufferFormats::sPerMaterial*>(currentOffset);

		// Extracting Texture Type Mask

		currentOffset += sizeof(constantData_perMaterial);
		textureTypeMask = *reinterpret_cast<uint8_t*>(currentOffset);

		// Extracting Offset To Add

		currentOffset += sizeof(textureTypeMask);
		offsetToAdd = *reinterpret_cast<uint8_t*>(currentOffset);

		// Extracting Effect Path

		currentOffset += sizeof(offsetToAdd);
		effectPath = reinterpret_cast<char*>(currentOffset);

		// Extracting all texture maps

		{
			// Color Map
			if (TextureTypes::IsColorMapPresent(textureTypeMask))
			{
				// Extracting Offset To Add

				currentOffset += offsetToAdd;
				offsetToAdd = *reinterpret_cast<uint8_t*>(currentOffset);

				// Extracting Color Map Path

				currentOffset += sizeof(offsetToAdd);
				texturePaths[TextureUnit::COLOR] = reinterpret_cast<char*>(currentOffset);
			}

			// Specular Map
			if (TextureTypes::IsSpecularMapPresent(textureTypeMask))
			{
				// Extracting Offset To Add

				currentOffset += offsetToAdd;
				offsetToAdd = *reinterpret_cast<uint8_t*>(currentOffset);

				// Extracting Specular Map Path

				currentOffset += sizeof(offsetToAdd);
				texturePaths[TextureUnit::SPECULAR] = reinterpret_cast<char*>(currentOffset);
			}

			// Normal Map
			if (TextureTypes::IsNormalMapPresent(textureTypeMask))
			{
				// Extracting Offset To Add

				currentOffset += offsetToAdd;
				offsetToAdd = *reinterpret_cast<uint8_t*>(currentOffset);

				// Extracting Normal Map Path

				currentOffset += sizeof(offsetToAdd);
				texturePaths[TextureUnit::NORMAL] = reinterpret_cast<char*>(currentOffset);
			}

			// Bump Map
			if (TextureTypes::IsBumpMapPresent(textureTypeMask))
			{
				// Extracting Offset To Add

				currentOffset += offsetToAdd;
				offsetToAdd = *reinterpret_cast<uint8_t*>(currentOffset);

				// Extracting Bump Map Path

				currentOffset += sizeof(offsetToAdd);
				texturePaths[TextureUnit::BUMP] = reinterpret_cast<char*>(currentOffset);
			}

			// Emissive Map
			if (TextureTypes::IsEmissiveMapPresent(textureTypeMask))
			{
				// Extracting Offset To Add

				currentOffset += offsetToAdd;
				offsetToAdd = *reinterpret_cast<uint8_t*>(currentOffset);

				// Extracting Emissive Map Path

				currentOffset += sizeof(offsetToAdd);
				texturePaths[TextureUnit::EMISSIVE] = reinterpret_cast<char*>(currentOffset);
			}

			// Displacement Map
			if (TextureTypes::IsDisplacementMapPresent(textureTypeMask))
			{
				// Extracting Offset To Add

				currentOffset += offsetToAdd;
				offsetToAdd = *reinterpret_cast<uint8_t*>(currentOffset);

				// Extracting Displacement Map Path

				currentOffset += sizeof(offsetToAdd);
				texturePaths[TextureUnit::DISPLACEMENT] = reinterpret_cast<char*>(currentOffset);
			}

			// Distortion Map
			if (TextureTypes::IsDistortionMapPresent(textureTypeMask))
			{
				// Extracting Offset To Add

				currentOffset += offsetToAdd;
				offsetToAdd = *reinterpret_cast<uint8_t*>(currentOffset);

				// Extracting Distortion Map Path

				currentOffset += sizeof(offsetToAdd);
				texturePaths[TextureUnit::DISTORTION] = reinterpret_cast<char*>(currentOffset);
			}
		}

		// Check EOF

		currentOffset += offsetToAdd;
		if (finalOffset != currentOffset)
		{
			result = Results::Failure;
			EAE6320_ASSERTF(false, "Redundant data found in file: \"%s\"", i_path);
			Logging::OutputError("Material file: \"%s\" has redundant data", i_path);
			goto OnExit;
		}
	}

	if (!((result = newMaterial->Initialize(effectPath, texturePaths, &constantData_perMaterial))))
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
	if (texturePaths)
	{
		delete[] texturePaths;
	}

	return result;
}

// Render
//-------

void eae6320::Graphics::cMaterial::Bind() const
{
	// Binding the effect
	{
		EAE6320_ASSERT(m_pEffect);
		m_pEffect->Bind();
	}

	// Binding the texture
	{
		for (auto i = 0u; i < TextureUnit::COUNT; i++)
		{
			if (m_maps[i].m_texture)
			{
				EAE6320_ASSERT(m_maps[i].m_pTexture);
				// TODO: If open gl bind sampler to this unit too
				m_maps[i].m_pTexture->Bind(m_maps[i].m_unitNumber);
			}
		}
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

eae6320::cResult eae6320::Graphics::cMaterial::Initialize(char const*const i_effectPath, char const*const*const i_texturepaths, void const*const i_data)
{
	auto result = Results::success;

	// Load the effect

	if (!((result = cEffect::s_manager.Load(i_effectPath, m_effect))))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	m_pEffect = cEffect::s_manager.Get(m_effect);

	// Load all the texture maps

	for (auto i = 0u; i < TextureUnit::COUNT; i++)
	{
		if (i_texturepaths[i])
		{
			if (!((result = cTexture::s_manager.Load(i_texturepaths[i], m_maps[i].m_texture))))
			{
				EAE6320_ASSERT(false);
				goto OnExit;
			}
			m_maps[i].m_unitNumber = static_cast<TextureUnit::eUnitNumber>(i);
			m_maps[i].m_pTexture = cTexture::s_manager.Get(m_maps[i].m_texture);
		}
	}

	// Initialize the constant buffer for this material
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

	// Textures Clean Up
	for (auto i = 0u; i < TextureUnit::COUNT; i++)
	{
		if (m_maps[i].m_texture)
		{
			m_maps[i].m_unitNumber = TextureUnit::UNKNOWN;
			const auto localResult = cTexture::s_manager.Release(m_maps[i].m_texture);
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