// Include Files
//==============

#include "../cRenderState.h"

#include "Includes.h"
#include "../sContext.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

// Interface
//==========

// Render
//-------

void eae6320::Graphics::cRenderState::Bind() const
{
    auto* const direct3DImmediateContext = sContext::g_context.direct3DImmediateContext;
    EAE6320_ASSERT(direct3DImmediateContext);

    // Alpha Transparency
    {
        EAE6320_ASSERT(m_blendState);
        const float* const noBlendFactor = nullptr;
        const auto defaultSampleMask = 0xffffffff;
        direct3DImmediateContext->OMSetBlendState(m_blendState, noBlendFactor, defaultSampleMask);
    }
    // Depth Buffering
    {
        EAE6320_ASSERT(m_depthStencilState);
        const unsigned int unusedStencilReference = 0;
        direct3DImmediateContext->OMSetDepthStencilState(m_depthStencilState, unusedStencilReference);
    }
    // Draw Both Triangle Sides
    {
        EAE6320_ASSERT(m_rasterizerState);
        direct3DImmediateContext->RSSetState(m_rasterizerState);
    }
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cRenderState::CleanUp()
{
    const auto result = Results::success;

    if (m_blendState)
    {
        m_blendState->Release();
        m_blendState = nullptr;
    }
    if (m_depthStencilState)
    {
        m_depthStencilState->Release();
        m_depthStencilState = nullptr;
    }
    if (m_rasterizerState)
    {
        m_rasterizerState->Release();
        m_rasterizerState = nullptr;
    }

    return result;
}

// Implementation
//===============

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cRenderState::InitializeFromBits()
{
    auto result = Results::success;

    auto* const direct3DDevice = sContext::g_context.direct3DDevice;

    // Alpha Transparency
    {
        D3D11_BLEND_DESC blendStateDescription{};
        // Alpha-to-coverage isn't used
        blendStateDescription.AlphaToCoverageEnable = FALSE;
        // All render targets use the same blending
        // (in our class we will only ever use a single render target)
        blendStateDescription.IndependentBlendEnable = FALSE;
        {
            auto& renderTargetBlendDescription = blendStateDescription.RenderTarget[0];
            if (IsAlphaTransparencyEnabled())
            {
                renderTargetBlendDescription.BlendEnable = TRUE;

                // result = ( source * source.a ) + ( destination * ( 1 - source.a ) )
                renderTargetBlendDescription.SrcBlend = D3D11_BLEND_SRC_ALPHA;
                renderTargetBlendDescription.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
                renderTargetBlendDescription.BlendOp = D3D11_BLEND_OP_ADD;
                renderTargetBlendDescription.SrcBlendAlpha = D3D11_BLEND_ONE;
                renderTargetBlendDescription.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
                renderTargetBlendDescription.BlendOpAlpha = D3D11_BLEND_OP_ADD;
            }
            else
            {
                renderTargetBlendDescription.BlendEnable = FALSE;

                // (Since blending is disabled the remaining states don't matter,
                // but it doesn't hurt to set them to explicitly disable alpha transparency)
                renderTargetBlendDescription.SrcBlend = D3D11_BLEND_ONE;
                renderTargetBlendDescription.DestBlend = D3D11_BLEND_ZERO;
                renderTargetBlendDescription.BlendOp = D3D11_BLEND_OP_ADD;
                renderTargetBlendDescription.SrcBlendAlpha = D3D11_BLEND_ONE;
                renderTargetBlendDescription.DestBlendAlpha = D3D11_BLEND_ZERO;
                renderTargetBlendDescription.BlendOpAlpha = D3D11_BLEND_OP_ADD;
            }
            renderTargetBlendDescription.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        }
        const auto d3DResult = direct3DDevice->CreateBlendState(&blendStateDescription, &m_blendState);
        if (FAILED(d3DResult))
        {
            result = Results::Failure;
            EAE6320_ASSERTF(false, "CreateBlendState() failed (HRESULT %#010x)", d3DResult);
            Logging::OutputError("Direct3D failed to create a blend render state object from %u with HRESULT %#010x", m_bits, d3DResult);
            goto OnExit;
        }
    }
    // Depth Buffering
    {
        D3D11_DEPTH_STENCIL_DESC depthStateDescription{};
        // We don't use the stencil buffer in our class
        depthStateDescription.StencilEnable = FALSE;
        depthStateDescription.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        depthStateDescription.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
        depthStateDescription.FrontFace.StencilFunc = depthStateDescription.BackFace.StencilFunc =
            D3D11_COMPARISON_ALWAYS;
        depthStateDescription.FrontFace.StencilDepthFailOp = depthStateDescription.BackFace.StencilDepthFailOp =
            depthStateDescription.FrontFace.StencilPassOp = depthStateDescription.BackFace.StencilPassOp =
            depthStateDescription.FrontFace.StencilFailOp = depthStateDescription.BackFace.StencilFailOp =
            D3D11_STENCIL_OP_KEEP;

        depthStateDescription.DepthFunc =
            (depthStateDescription.DepthEnable = static_cast<BOOL>(IsDepthTestingEnabled())) ?
            // The new fragment becomes a pixel if its depth is less than what has previously been written
            D3D11_COMPARISON_LESS :
            // Don't test the depth buffer
            D3D11_COMPARISON_ALWAYS;
        /*if (IsDepthTestingEnabled())
        {
            // The new fragment becomes a pixel if its depth is less than what has previously been written
            depthStateDescription.DepthEnable = TRUE;
            depthStateDescription.DepthFunc = D3D11_COMPARISON_LESS;
        }
        else
        {
            // Don't test the depth buffer
            depthStateDescription.DepthEnable = FALSE;
            depthStateDescription.DepthFunc = D3D11_COMPARISON_ALWAYS;
        }*/
        depthStateDescription.DepthWriteMask =
            IsDepthWritingEnabled() ?
            // Write to the depth buffer
            D3D11_DEPTH_WRITE_MASK_ALL :
            // Don't write to the depth buffer
            D3D11_DEPTH_WRITE_MASK_ZERO;
        /*if (IsDepthWritingEnabled())
        {
            // Write to the depth buffer
            depthStateDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        }
        else
        {
            // Don't write to the depth buffer
            depthStateDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        }*/
        const auto d3DResult = direct3DDevice->CreateDepthStencilState(&depthStateDescription, &m_depthStencilState);
        if (FAILED(d3DResult))
        {
            result = Results::Failure;
            EAE6320_ASSERTF(false, "CreateDepthStencilState() failed (HRESULT %#010x)", d3DResult);
            Logging::OutputError("Direct3D failed to create a depth/stencil render state object from %u with HRESULT %#010x", m_bits, d3DResult);
            goto OnExit;
        }
    }
    // Draw Both Triangle Sides
    {
        D3D11_RASTERIZER_DESC rasterizerStateDescription{};

        rasterizerStateDescription.FillMode =
            IsWireFrameModeEnabled() ?
            // Draw wireframe geometry (i.e. not solid)
            D3D11_FILL_WIREFRAME :
            // Draw solid geometry (i.e. not wireframe)
            D3D11_FILL_SOLID;

        // Triangles use left-handed winding order
        // (opposite from OpenGL)
        rasterizerStateDescription.FrontCounterClockwise = FALSE;
        // No depth bias
        rasterizerStateDescription.DepthBias = 0;
        rasterizerStateDescription.SlopeScaledDepthBias = 0.0f;
        rasterizerStateDescription.DepthBiasClamp = 0.0f;
        // Use default depth clipping
        rasterizerStateDescription.DepthClipEnable = TRUE;
        // Disable "scissoring" (i.e. render all pixels in the render target)
        rasterizerStateDescription.ScissorEnable = FALSE;
        // Disable multisampling/antialiasing
        rasterizerStateDescription.MultisampleEnable = FALSE;
        rasterizerStateDescription.AntialiasedLineEnable = FALSE;
        rasterizerStateDescription.CullMode =
            ShouldBothTriangleSidesBeDrawn() ?
            // Don't cull any triangles
            D3D11_CULL_NONE :
            // Cull triangles that are facing backwards
            D3D11_CULL_BACK;

        const auto d3DResult = direct3DDevice->CreateRasterizerState(&rasterizerStateDescription, &m_rasterizerState);
        if (FAILED(d3DResult))
        {
            result = Results::Failure;
            EAE6320_ASSERTF(false, "CreateRasterizerState() failed (HRESULT %#010x)", d3DResult);
            Logging::OutputError("Direct3D failed to create a rasterizer render state object from %u with HRESULT %#010x", m_bits, d3DResult);
            goto OnExit;
        }
    }

OnExit:

    return result;
}
