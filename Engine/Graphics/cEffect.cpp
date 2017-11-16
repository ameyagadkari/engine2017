// Include Files
//==============

#include "cEffect.h"

#include <Engine/Platform/Platform.h>

// Static Data Initialization
//===========================

eae6320::Assets::cManager<eae6320::Graphics::cEffect> eae6320::Graphics::cEffect::s_manager;

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cEffect::Load(const char* const i_path, cEffect*& o_effect)
{
	auto result = Results::success;

	Platform::sDataFromFile dataFromFile;
	char* vertexShaderPath = nullptr;
	char* fragmentShaderPath = nullptr;
	uint8_t renderState = 0xff;
	uint8_t offsetToAdd = 0;
	cEffect* newEffect = nullptr;

	// Load the binary data
	{
		std::string errorMessage;
		if (!((result = LoadBinaryFile(i_path, dataFromFile, &errorMessage))))
		{
			EAE6320_ASSERTF(false, errorMessage.c_str());
			Logging::OutputError("Failed to load effect from file %s: %s", i_path, errorMessage.c_str());
			goto OnExit;
		}
	}

	// Allocate a new effect
	{
		newEffect = new (std::nothrow) cEffect();
		if (!newEffect)
		{
			result = Results::outOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the effect %s", i_path);
			Logging::OutputError("Failed to allocate memory for the effect %s", i_path);
			goto OnExit;
		}
	}

	// Extract data from loaded effect file
	{
		// Casting data to uint8_t* for pointer arithematic

		auto data = reinterpret_cast<uint8_t*>(dataFromFile.data);

		// Extracting Render State Bits	

		renderState = *data;

		// Extracting Offset To Add

		data += sizeof(renderState);
		offsetToAdd = *data;

		// Extracting Vertex Shader Path

		data += sizeof(offsetToAdd);
		vertexShaderPath = reinterpret_cast<char*>(data);

		// Extracting Offset To Add

		data += offsetToAdd;
		offsetToAdd = *data;

		// Extracting Fragment Shader Path

		data += sizeof(offsetToAdd);
		fragmentShaderPath = reinterpret_cast<char*>(data);
	}

	if (!((result = newEffect->Initialize(vertexShaderPath, fragmentShaderPath, renderState))))
	{
		EAE6320_ASSERTF(false, "Initialization of new effect failed");
		goto OnExit;
	}

OnExit:

	if (result)
	{
		EAE6320_ASSERT(newEffect);
		o_effect = newEffect;
	}
	else
	{
		if (newEffect)
		{
			newEffect->DecrementReferenceCount();
			newEffect = nullptr;
		}
		o_effect = nullptr;
	}
	dataFromFile.Free();

	return result;
}

eae6320::cResult eae6320::Graphics::cEffect::Initialize(char const*const i_vertexShaderPath, char const*const i_fragmentShaderPath, const uint8_t i_renderState)
{
	auto result = Results::success;

	if (!((result = cShader::s_manager.Load(i_vertexShaderPath, m_vertexShader, ShaderTypes::Vertex))))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	if (!((result = cShader::s_manager.Load(i_fragmentShaderPath, m_fragmentShader, ShaderTypes::Fragment))))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	if (!((result = m_renderState.Initialize(i_renderState))))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}


	if (!((result = InitializePlatformSpecific())))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::Graphics::cEffect::CleanUp()
{
	auto result = Results::success;

	{
		const auto localResult = CleanUpPlatformSpecific();
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}

	// Vertex Shader Clean Up
	if (m_vertexShader)
	{
		const auto localResult = cShader::s_manager.Release(m_vertexShader);
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}

	// Fragment Shader Clean Up
	if (m_fragmentShader)
	{
		const auto localResult = cShader::s_manager.Release(m_fragmentShader);
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}

	// Render State Clean Up
	{
		const auto localResult = m_renderState.CleanUp();
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

// Render
//-------

void eae6320::Graphics::cEffect::Bind() const
{
	BindPlatformSpecific();
	m_renderState.Bind();
}

// Implementation
//===============

// Initialization / Clean Up
//--------------------------

eae6320::Graphics::cEffect::~cEffect()
{
	CleanUp();
}
