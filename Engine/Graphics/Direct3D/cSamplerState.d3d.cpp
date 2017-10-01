// Include Files
//==============

#include "../cSamplerState.h"

#include "Includes.h"
#include "../sContext.h"

#include <cfloat>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

// Interface
//==========

// Render
//-------

void eae6320::Graphics::cSamplerState::Bind() const
{
	auto* const direct3DImmediateContext = sContext::g_context.direct3DImmediateContext;
	EAE6320_ASSERT( direct3DImmediateContext );

	// The register must match the ID defined in the shader;
	// In our class we will only have a single sampler state
	constexpr unsigned int samplerStateRegister = 0;
	constexpr unsigned int samplerStateCount = 1;
	// Only fragment shaders sample textures (in our class)
	EAE6320_ASSERT( m_samplerState );
	direct3DImmediateContext->PSSetSamplers( samplerStateRegister, samplerStateCount, &m_samplerState );
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cSamplerState::Initialize()
{
	auto result = Results::success;

	auto* const direct3DDevice = sContext::g_context.direct3DDevice;

	D3D11_SAMPLER_DESC samplerStateDescription{};
	{
		// Linear filtering
		samplerStateDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		// If UVs go outside [0,1] wrap them around (so that textures can tile)
		samplerStateDescription.AddressU = samplerStateDescription.AddressV
			= samplerStateDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		// Default values
		samplerStateDescription.MipLODBias = 0.0f;
		samplerStateDescription.MaxAnisotropy = 1;
		samplerStateDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerStateDescription.BorderColor[0] = samplerStateDescription.BorderColor[1]
			= samplerStateDescription.BorderColor[2] = samplerStateDescription.BorderColor[3] = 1.0f;
		samplerStateDescription.MinLOD = -FLT_MAX;
		samplerStateDescription.MaxLOD = FLT_MAX;
	}
	const auto d3DResult = direct3DDevice->CreateSamplerState( &samplerStateDescription, &m_samplerState );
	if ( FAILED( d3DResult ) )
	{
		result = Results::Failure;
		EAE6320_ASSERTF( false, "CreateSamplerState() failed (HRESULT %#010x", d3DResult );
		Logging::OutputError( "Direct3D failed to create a sampler state with HRESULT %#010x", d3DResult );
		goto OnExit;
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::Graphics::cSamplerState::CleanUp()
{
	const auto result = Results::success;

	if ( m_samplerState )
	{
		m_samplerState->Release();
		m_samplerState = nullptr;
	}

	return result;
}
