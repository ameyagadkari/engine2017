// Include Files
//==============

#include "cEffect.h"

namespace
{
	const std::string s_relativeVertexShaderPath = "data/Shaders/Vertex/";
	const std::string s_relativeFragmentShaderPath = "data/Shaders/Fragment/";
}

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cEffect::Initialize(const std::string& i_vertexShaderName, const std::string& i_fragmentShaderName, const uint8_t& i_renderState)
{
	auto result = eae6320::Results::Success;

	if (!(result = eae6320::Graphics::cShader::s_manager.Load((s_relativeVertexShaderPath + i_vertexShaderName).c_str(),
		m_vertexShader, eae6320::Graphics::ShaderTypes::Vertex)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	if (!(result = eae6320::Graphics::cShader::s_manager.Load((s_relativeFragmentShaderPath + i_fragmentShaderName).c_str(),
		m_fragmentShader, eae6320::Graphics::ShaderTypes::Fragment)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	{
		if (!(result = m_renderState.Initialize(i_renderState)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

	if (!(result = InitializePlatformSpecific()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::Graphics::cEffect::CleanUp()
{
	auto result = eae6320::Results::Success;

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
