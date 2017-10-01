// Include Files
//==============

#include "Includes.h"
#include "../cEffect.h"
#include "../sContext.h"

// Implementation
//===============

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cEffect::InitializePlatformSpecific()
{
	// Nothing Special is required to be done here.

	return eae6320::Results::success;
}

eae6320::cResult eae6320::Graphics::cEffect::CleanUpPlatformSpecific()
{
	// Nothing Special is required to be done here.

	return eae6320::Results::success;
}

// Render
//-------

void eae6320::Graphics::cEffect::BindPlatformSpecific() const
{
	auto* const direct3dImmediateContext = sContext::g_context.direct3DImmediateContext;
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
}