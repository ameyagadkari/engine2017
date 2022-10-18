// Include Files
//==============

#include "../cTexture.h"

#include "Includes.h"
#include "../sContext.h"

#include <algorithm>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <new>

// Helper Function Declarations
//=============================

namespace
{
    DXGI_FORMAT GetDxgiFormat(const eae6320::Graphics::TextureFormats::Compression::eType i_compressionType);
}

// Interface
//==========

// Render
//-------

void eae6320::Graphics::cTexture::Bind(const unsigned int i_id) const
{
    auto* const direct3DImmediateContext = sContext::g_context.direct3DImmediateContext;
    EAE6320_ASSERT(direct3DImmediateContext);

    constexpr unsigned int viewCount = 1;
    direct3DImmediateContext->PSSetShaderResources(i_id, viewCount, &m_textureView);
}

// Implementation
//===============

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cTexture::Initialize(const char* const i_path, const void* const i_textureData, const size_t i_textureDataSize)
{
    auto result = Results::success;

    auto* const direct3DDevice = sContext::g_context.direct3DDevice;
    EAE6320_ASSERT(direct3DDevice);

    ID3D11Texture2D* resource = nullptr;
    D3D11_SUBRESOURCE_DATA* subResourceData = nullptr;

    // Allocate data for a "subresource" for each MIP level
    // (Subresources are the way that Direct3D deals with textures that act like a single resource
    // but that actually have multiple textures associated with that single resource
    // (e.g. MIP maps, volume textures, texture arrays))
    const auto mipMapCount = static_cast<uint_fast8_t>(m_info.mipMapCount);
    {
        subResourceData = new (std::nothrow) D3D11_SUBRESOURCE_DATA[mipMapCount];
        if (!subResourceData)
        {
            result = Results::outOfMemory;
            EAE6320_ASSERTF(false, "Couldn't allocate an array of %u subresource data structs", mipMapCount);
            Logging::OutputError("Failed to allocate an array of %u subresource data structs for the texture %s",
                mipMapCount, i_path);
            goto OnExit;
        }
    }
    // Fill in the data for each MIP level
    const auto width = static_cast<uint_fast16_t>(m_info.width);
    const auto height = static_cast<uint_fast16_t>(m_info.height);
    {
        auto currentWidth = width;
        auto currentHeight = height;
        auto currentOffset = reinterpret_cast<uintptr_t>(i_textureData);
        const auto finalOffset = currentOffset + i_textureDataSize;
        const auto blockSize = GetSizeOfBlock(m_info.compressionType);
        for (uint_fast8_t i = 0; i < mipMapCount; ++i)
        {
            // Calculate how much memory this MIP level uses
            const auto blockCount_singleRow = (currentWidth + 3) / 4;
            const auto byteCount_singleRow = blockCount_singleRow * blockSize;
            const auto rowCount = (currentHeight + 3) / 4;
            const auto byteCount_currentMipLevel = byteCount_singleRow * rowCount;
            // Set the data into the subresource
            {
                auto& currentSubResourceData = subResourceData[i];
                currentSubResourceData.pSysMem = reinterpret_cast<void*>(currentOffset);
                currentSubResourceData.SysMemPitch = static_cast<unsigned int>(byteCount_singleRow);
                currentSubResourceData.SysMemSlicePitch = static_cast<unsigned int>(byteCount_currentMipLevel);
            }
            // Update current data for next iteration
            {
                currentOffset += byteCount_currentMipLevel;
                if (currentOffset <= finalOffset)
                {
                    currentWidth = std::max(currentWidth / 2, 1u);
                    currentHeight = std::max(currentHeight / 2, 1u);
                }
                else
                {
                    result = Results::invalidFile;
                    EAE6320_ASSERTF(false, "Texture file %s is too small to contain MIP map #%u",
                        i_path, i);
                    Logging::OutputError("The texture file %s is too small to contain MIP map #%u",
                        i_path, i);
                    goto OnExit;
                }
            }
        }
        EAE6320_ASSERTF(currentOffset == finalOffset, "The texture file %s has more texture data (%u) than it should (%u)",
            i_path, finalOffset, currentOffset);
    }
    // Create the resource
    const auto dxgiFormat = GetDxgiFormat(m_info.compressionType);
    {
        D3D11_TEXTURE2D_DESC textureDescription{};
        {
            textureDescription.Width = static_cast<unsigned int>(width);
            textureDescription.Height = static_cast<unsigned int>(height);
            textureDescription.MipLevels = static_cast<unsigned int>(mipMapCount);
            textureDescription.ArraySize = 1;
            textureDescription.Format = dxgiFormat;
            {
                DXGI_SAMPLE_DESC& sampleDescription = textureDescription.SampleDesc;
                sampleDescription.Count = 1;    // No multisampling
                sampleDescription.Quality = 0;    // Doesn't matter when Count is 1
            }
            textureDescription.Usage = D3D11_USAGE_IMMUTABLE;    // The texture will never change once it's been created
            textureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            textureDescription.CPUAccessFlags = 0;    // No CPU access is necessary
            textureDescription.MiscFlags = 0;
        }
        const auto d3DResult = direct3DDevice->CreateTexture2D(&textureDescription, subResourceData, &resource);
        if (FAILED(d3DResult))
        {
            result = Results::Failure;
            EAE6320_ASSERTF(false, "CreateTexture2D() failed for %s with HRESULT %#010x", i_path, d3DResult);
            Logging::OutputError("Direct3D failed to create a texture from %s with HRESULT %#010x", i_path, d3DResult);
            goto OnExit;
        }
    }
    // Create the view
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescription{};
        {
            shaderResourceViewDescription.Format = dxgiFormat;
            shaderResourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            {
                auto& shaderResourceView2dDescription = shaderResourceViewDescription.Texture2D;
                shaderResourceView2dDescription.MostDetailedMip = 0;    // Use the highest resolution in the texture resource
                shaderResourceView2dDescription.MipLevels = -1;    // Use all MIP levels
            }
        }
        const auto d3DResult = direct3DDevice->CreateShaderResourceView(resource, &shaderResourceViewDescription, &m_textureView);
        if (FAILED(d3DResult))
        {
            result = Results::Failure;
            EAE6320_ASSERTF(false, "CreateShaderResourceView() failed for %s with HRESULT %#010x", i_path, d3DResult);
            Logging::OutputError("Direct3D failed to create a shader resource view for %s with HRESULT %#010x", i_path, d3DResult);
            goto OnExit;
        }
    }

OnExit:

    // The texture resource is always released, even on success
    // (the view will hold its own reference to the resource)
    if (resource)
    {
        resource->Release();
        resource = nullptr;
    }
    if (subResourceData)
    {
        delete[] subResourceData;
        subResourceData = nullptr;
    }

    return result;
}

eae6320::cResult eae6320::Graphics::cTexture::CleanUp()
{
    const auto result = Results::success;

    if (m_textureView)
    {
        m_textureView->Release();
        m_textureView = nullptr;
    }

    return result;
}

// Helper Function Definitions
//============================

namespace
{
    DXGI_FORMAT GetDxgiFormat(const eae6320::Graphics::TextureFormats::Compression::eType i_compressionType)
    {
        switch (i_compressionType)
        {
        case eae6320::Graphics::TextureFormats::Compression::BC1: return DXGI_FORMAT_BC1_UNORM;
        case eae6320::Graphics::TextureFormats::Compression::BC3: return DXGI_FORMAT_BC3_UNORM;
        default: ;
        }

        // Other formats are possible, but not for our class
        EAE6320_ASSERT(false);
        return DXGI_FORMAT_UNKNOWN;
    }
}
