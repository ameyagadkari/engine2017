// Include Files
//==============

#include "cEffect.h"

// Static Data Initialization
//===========================

eae6320::Assets::cManager<eae6320::Graphics::cEffect> eae6320::Graphics::cEffect::s_manager;

namespace
{
	const std::string s_relativeVertexShaderPath = "data/Shaders/Vertex/";
	const std::string s_relativeFragmentShaderPath = "data/Shaders/Fragment/";
}

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cEffect::Load(const char* const i_path, cEffect*& o_effect, const std::string& i_vertexShaderName, const std::string& i_fragmentShaderName, const uint8_t& i_renderState)
{
	auto result = Results::success;

	//Platform::sDataFromFile dataFromFile;
	cEffect* newEffect = nullptr;

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
		newEffect = new (std::nothrow) cEffect();
		if (!newEffect)
		{
			result = Results::outOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the effect %s", i_path);
			Logging::OutputError("Failed to allocate memory for the effect %s", i_path);
			goto OnExit;
		}
	}
	//if (!(result = newEffect->Initialize(i_path, dataFromFile)))
	if (!((result = newEffect->Initialize(i_vertexShaderName, i_fragmentShaderName, i_renderState))))
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
	//dataFromFile.Free();

	return result;
}

eae6320::cResult eae6320::Graphics::cEffect::Initialize(const std::string& i_vertexShaderName, const std::string& i_fragmentShaderName, const uint8_t& i_renderState)
{
	auto result = Results::success;

	if (!((result = cShader::s_manager.Load((s_relativeVertexShaderPath + i_vertexShaderName).c_str(),
		m_vertexShader, ShaderTypes::Vertex))))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	if (!((result = cShader::s_manager.Load((s_relativeFragmentShaderPath + i_fragmentShaderName).c_str(),
		m_fragmentShader, ShaderTypes::Fragment))))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	{
		if (!((result = m_renderState.Initialize(i_renderState))))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
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
