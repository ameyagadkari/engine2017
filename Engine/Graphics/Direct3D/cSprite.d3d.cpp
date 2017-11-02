// Include Files
//==============

#include "../cSprite.h"

#include "Includes.h"
#include "../sContext.h"
#include "../VertexFormats.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Transform/sRectTransform.h>

// Static Data Initialization
//===========================

ID3D11InputLayout* eae6320::Graphics::cSprite::ms_vertexInputLayout = nullptr;

namespace
{
	// Since a sprite is always a quad the vertex count will always be 4

	constexpr unsigned int s_vertexCount = 4;
}

// Implementation
//===============

// Initialization / Clean Up

eae6320::cResult eae6320::Graphics::cSprite::Initialize(const Transform::sRectTransform& i_rectTransform)
{
	auto result = Results::success;

	auto* const direct3DDevice = sContext::g_context.direct3DDevice;
	EAE6320_ASSERT(direct3DDevice);

	// Initialize vertex format
	if (!ms_vertexInputLayout)
	{
		// Load the compiled binary vertex shader for the input layout
		Platform::sDataFromFile vertexShaderDataFromFile;
		std::string errorMessage;
		if ((result = LoadBinaryFile("data/Shaders/Vertex/vertexInputLayout_sprite.busl", vertexShaderDataFromFile, &errorMessage)))
		{
			// Create the vertex layout

			// These elements must match the VertexFormats::sSprite layout struct exactly.
			// They instruct Direct3D how to match the binary data in the vertex buffer
			// to the input elements in a vertex shader
			// (by using so-called "semantic" names so that, for example,
			// "POSITION" here matches with "POSITION" in shader code).
			// Note that OpenGL uses arbitrarily assignable number IDs to do the same thing.
			constexpr unsigned int vertexElementCount = 2;
			D3D11_INPUT_ELEMENT_DESC layoutDescription[vertexElementCount] = {};
			{
				// Slot 0

				// POSITION
				// 2 floats == 8 bytes
				// Offset = 0
				{
					auto& positionElement = layoutDescription[0];

					positionElement.SemanticName = "POSITION";
					positionElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
					positionElement.Format = DXGI_FORMAT_R32G32_FLOAT;
					positionElement.InputSlot = 0;
					positionElement.AlignedByteOffset = offsetof(eae6320::Graphics::VertexFormats::sSprite, x);
					positionElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
					positionElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
				}

				// Slot 1

				// TEXTURE_COORDINATES
				// 2 uint16_t == 4 bytes
				// Offset = 8
				{
					auto& uvsElement = layoutDescription[1];

					uvsElement.SemanticName = "TEXCOORD";
					uvsElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
					uvsElement.Format = DXGI_FORMAT_R16G16_FLOAT;
					uvsElement.InputSlot = 0;
					uvsElement.AlignedByteOffset = offsetof(eae6320::Graphics::VertexFormats::sSprite, u);
					uvsElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
					uvsElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
				}
			}

			const auto d3DResult = direct3DDevice->CreateInputLayout(layoutDescription, vertexElementCount,
				vertexShaderDataFromFile.data, vertexShaderDataFromFile.size, &ms_vertexInputLayout);
			if (FAILED(d3DResult))
			{
				result = Results::Failure;
				EAE6320_ASSERTF(false, "Sprite vertex input layout creation failed (HRESULT %#010x)", d3DResult);
				Logging::OutputError("Direct3D failed to create the sprite vertex input layout (HRESULT %#010x)", d3DResult);
				vertexShaderDataFromFile.Free();
				goto OnExit;
			}

			vertexShaderDataFromFile.Free();
		}
		else
		{
			EAE6320_ASSERTF(false, errorMessage.c_str());
			Logging::OutputError("The sprite vertex input layout shader couldn't be loaded: %s", errorMessage.c_str());
			goto OnExit;
		}
	}
	// Vertex Buffer
	{
		VertexFormats::sSprite vertexData[s_vertexCount];
		{
			GenerateVertexData(i_rectTransform, vertexData);
		}
		D3D11_BUFFER_DESC bufferDescription{};
		{
			const auto bufferSize = s_vertexCount * sizeof(VertexFormats::sSprite);
			EAE6320_ASSERT(bufferSize < (uint64_t(1u) << (sizeof(bufferDescription.ByteWidth) * 8)));
			bufferDescription.ByteWidth = static_cast<unsigned int>(bufferSize);
			bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
			bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
			bufferDescription.MiscFlags = 0;
			bufferDescription.StructureByteStride = 0;	// Not used
		}
		D3D11_SUBRESOURCE_DATA initialData{};
		{
			EAE6320_ASSERT(vertexData);
			initialData.pSysMem = vertexData;
			// (The other data members are ignored for non-texture buffers)
		}

		const auto d3DResult = direct3DDevice->CreateBuffer(&bufferDescription, &initialData, &m_vertexBuffer);
		if (FAILED(d3DResult))
		{
			result = Results::Failure;
			EAE6320_ASSERTF(false, "Geometry vertex buffer creation failed (HRESULT %#010x)", d3DResult);
			Logging::OutputError("Direct3D failed to create a geometry vertex buffer (HRESULT %#010x)", d3DResult);
			goto OnExit;
		}
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::Graphics::cSprite::CleanUp()
{
	const auto result = Results::success;

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
	if (ms_vertexInputLayout)
	{
		ms_vertexInputLayout->Release();
		ms_vertexInputLayout = nullptr;
	}

	return result;
}

void eae6320::Graphics::cSprite::Draw() const
{
	auto* const direct3DImmediateContext = sContext::g_context.direct3DImmediateContext;
	EAE6320_ASSERT(direct3DImmediateContext);
	// Bind a specific vertex buffer to the device as a data source
	{
		EAE6320_ASSERT(m_vertexBuffer);
		constexpr unsigned int startingSlot = 0;
		constexpr unsigned int vertexBufferCount = 1;
		// The "stride" defines how large a single vertex is in the stream of data
		constexpr unsigned int bufferStride = sizeof(VertexFormats::sSprite);
		// It's possible to start streaming data in the middle of a vertex buffer
		constexpr unsigned int bufferOffset = 0;
		direct3DImmediateContext->IASetVertexBuffers(startingSlot, vertexBufferCount, &m_vertexBuffer, &bufferStride, &bufferOffset);
	}

	if (VertexFormats::g_layoutType != VertexFormats::SPRITE)
	{
		VertexFormats::g_layoutType = VertexFormats::SPRITE;
		// Specify what kind of data the vertex buffer holds
		// Set the layout (which defines how to interpret a single vertex)
		{
			EAE6320_ASSERT(ms_vertexInputLayout);
			direct3DImmediateContext->IASetInputLayout(ms_vertexInputLayout);
		}
		// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
		// the vertex buffer was defined as a triangle stri[
		// (meaning that every primitive is a triangle but only the first will be defined by three vertices
		//  the rest of the triangles will use the previous two vertices and the next vertex)
		direct3DImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	}

	// Render triangles from the currently-bound vertex buffer
	{
		// It's possible to start rendering primitives in the middle of the stream
		constexpr unsigned int indexOfFirstVertexToRender = 0;
		direct3DImmediateContext->Draw(s_vertexCount, indexOfFirstVertexToRender);
	}
}
