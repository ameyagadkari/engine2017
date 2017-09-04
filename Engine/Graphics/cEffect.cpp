// Include Files
//==============

#include "cEffect.h"

#if defined( EAE6320_PLATFORM_D3D )
	#include "sContext.h"
	#include "Direct3D\Includes.h"
#endif

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cEffect::Initialize()
{
	auto result = eae6320::Results::Success;

	if (!(result = eae6320::Graphics::cShader::s_manager.Load("data/Shaders/Vertex/sprite.shd",
		m_vertexShader, eae6320::Graphics::ShaderTypes::Vertex)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	if (!(result = eae6320::Graphics::cShader::s_manager.Load("data/Shaders/Fragment/sprite.shd",
		m_fragmentShader, eae6320::Graphics::ShaderTypes::Fragment)))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
	{
		constexpr uint8_t defaultRenderState = 0;
		if (!(result = m_renderState.Initialize(defaultRenderState)))
		{
			EAE6320_ASSERT(false);
			goto OnExit;
		}
	}

#if defined( EAE6320_PLATFORM_GL )
	if (!(result = InitializeGL()))
	{
		EAE6320_ASSERT(false);
		goto OnExit;
	}
#endif

OnExit:

#if defined( EAE6320_PLATFORM_GL )
	if (!result)
	{
		if (m_programId != 0)
		{
			glDeleteProgram(m_programId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to delete the program: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
			m_programId = 0;
		}
	}
#endif

	return result;
}

eae6320::cResult eae6320::Graphics::cEffect::CleanUp()
{
	auto result = eae6320::Results::Success;

#if defined( EAE6320_PLATFORM_GL )
	// Program clean up
	if (m_programId != 0)
	{
		glDeleteProgram(m_programId);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = eae6320::Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		m_programId = 0;
	}
#endif

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
#if defined( EAE6320_PLATFORM_D3D )
	auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);
	{
		ID3D11ClassInstance* const* noInterfaces = nullptr;
		constexpr unsigned int interfaceCount = 0;
		// Vertex shader
		{
			EAE6320_ASSERT(m_vertexShader);
			auto* const shader = cShader::s_manager.Get(m_vertexShader);
			EAE6320_ASSERT(shader && shader->m_shaderObject.vertex);
			direct3dImmediateContext->VSSetShader(shader->m_shaderObject.vertex, noInterfaces, interfaceCount);
		}
		// Fragment shader
		{
			EAE6320_ASSERT(m_fragmentShader);
			auto* const shader = cShader::s_manager.Get(m_fragmentShader);
			EAE6320_ASSERT(shader && shader->m_shaderObject.fragment);
			direct3dImmediateContext->PSSetShader(shader->m_shaderObject.fragment, noInterfaces, interfaceCount);
		}
	}
#elif defined( EAE6320_PLATFORM_GL )
	{
		EAE6320_ASSERT(m_programId != 0);
		glUseProgram(m_programId);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
#endif
	m_renderState.Bind();
}
