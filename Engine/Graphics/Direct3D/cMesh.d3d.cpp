// Include Files
//==============

#include "../cMesh.h"

#include "Includes.h"
#include "../sContext.h"
#include "../VertexFormats.h"
#include "../MeshHelperStructs.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>

// Static Data Initialization
//===========================

ID3D11InputLayout* eae6320::Graphics::cMesh::ms_vertexInputLayout = nullptr;

// Implementation
//===============

// Initialization / Clean Up

eae6320::cResult eae6320::Graphics::cMesh::Initialize(HelperStructs::sMeshData const*const& i_meshData)
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
		if ((result = LoadBinaryFile("data/Shaders/Vertex/vertexInputLayout_mesh.busl", vertexShaderDataFromFile, &errorMessage)))
		{
			// Create the vertex layout

			// These elements must match the VertexFormats::sMesh layout struct exactly.
			// They instruct Direct3D how to match the binary data in the vertex buffer
			// to the input elements in a vertex shader
			// (by using so-called "semantic" names so that, for example,
			// "POSITION" here matches with "POSITION" in shader code).
			// Note that OpenGL uses arbitrarily assignable number IDs to do the same thing.
			constexpr unsigned int vertexElementCount = 3;
			D3D11_INPUT_ELEMENT_DESC layoutDescription[vertexElementCount] = {};
			{
				// Slot 0

				// POSITION
				// 3 floats == 12 bytes
				// Offset = 0
				{
					auto& positionElement = layoutDescription[0];

					positionElement.SemanticName = "POSITION";
					positionElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
					positionElement.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					positionElement.InputSlot = 0;
					positionElement.AlignedByteOffset = offsetof(eae6320::Graphics::VertexFormats::sMesh, x);
					positionElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
					positionElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
				}

				// Slot 1

				// TEXTURE_COORDINATES
				// 2 uint16_t == 4 bytes
				// Offset = 12
				{
					auto& positionElement = layoutDescription[1];

					positionElement.SemanticName = "TEXCOORD";
					positionElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
					positionElement.Format = DXGI_FORMAT_R16G16_FLOAT;
					positionElement.InputSlot = 0;
					positionElement.AlignedByteOffset = offsetof(eae6320::Graphics::VertexFormats::sMesh, u);
					positionElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
					positionElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
				}

				// Slot 2

				// COLOR
				// 4 uint8_t == 4 bytes
				// Offset = 16
				{
					auto& colorElement = layoutDescription[2];

					colorElement.SemanticName = "COLOR";
					colorElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
					colorElement.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					colorElement.InputSlot = 0;
					colorElement.AlignedByteOffset = offsetof(eae6320::Graphics::VertexFormats::sMesh, r);
					colorElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
					colorElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
				}
			}

			const auto d3DResult = direct3DDevice->CreateInputLayout(layoutDescription, vertexElementCount,
				vertexShaderDataFromFile.data, vertexShaderDataFromFile.size, &ms_vertexInputLayout);
			if (FAILED(d3DResult))
			{
				result = Results::Failure;
				EAE6320_ASSERTF(false, "Mesh vertex input layout creation failed (HRESULT %#010x)", d3DResult);
				Logging::OutputError("Direct3D failed to create the Mesh vertex input layout (HRESULT %#010x)", d3DResult);
				vertexShaderDataFromFile.Free();
				goto OnExit;
			}

			vertexShaderDataFromFile.Free();
		}
		else
		{
			EAE6320_ASSERTF(false, errorMessage.c_str());
			Logging::OutputError("The mesh vertex input layout shader couldn't be loaded: %s", errorMessage.c_str());
			goto OnExit;
		}
	}

	// Vertex Buffer
	{
		D3D11_BUFFER_DESC bufferDescription{};
		{
			const auto bufferSize = i_meshData->numberOfVertices * sizeof(VertexFormats::sMesh);
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
			EAE6320_ASSERT(i_meshData->vertexData);
			initialData.pSysMem = i_meshData->vertexData;
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

	// Index Buffer
	{
		m_numberOfIndices = i_meshData->numberOfIndices;
		m_type = i_meshData->type;
		D3D11_BUFFER_DESC bufferDescription{};
		{
			const auto bufferSize = (m_type == IndexDataTypes::BIT_16) ? m_numberOfIndices * sizeof(uint16_t) : m_numberOfIndices * sizeof(uint32_t);
			EAE6320_ASSERT(bufferSize < (uint64_t(1u) << (sizeof(bufferDescription.ByteWidth) * 8)));
			bufferDescription.ByteWidth = static_cast<unsigned int>(bufferSize);
			bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
			bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
			bufferDescription.MiscFlags = 0;
			bufferDescription.StructureByteStride = 0;	// Not used
		}
		D3D11_SUBRESOURCE_DATA initialData{};
		{
			EAE6320_ASSERT(i_meshData->indexData);
			initialData.pSysMem = i_meshData->indexData;
			// (The other data members are ignored for non-texture buffers)
		}

		const auto d3DResult = direct3DDevice->CreateBuffer(&bufferDescription, &initialData, &m_indexBuffer);
		if (FAILED(d3DResult))
		{
			result = Results::Failure;
			EAE6320_ASSERTF(false, "Geometry index buffer creation failed (HRESULT %#010x)", d3DResult);
			Logging::OutputError("Direct3D failed to create a geometry index buffer (HRESULT %#010x)", d3DResult);
			goto OnExit;
		}
	}

OnExit:

	return result;
}

eae6320::cResult eae6320::Graphics::cMesh::CleanUp()
{
	const auto result = Results::success;

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}
	if (ms_vertexInputLayout)
	{
		ms_vertexInputLayout->Release();
		ms_vertexInputLayout = nullptr;
	}

	return result;
}

void eae6320::Graphics::cMesh::Draw() const
{
	auto* const direct3DImmediateContext = sContext::g_context.direct3DImmediateContext;
	EAE6320_ASSERT(direct3DImmediateContext);
	// Bind a specific vertex buffer to the device as a data source
	{
		EAE6320_ASSERT(m_vertexBuffer);
		constexpr unsigned int startingSlot = 0;
		constexpr unsigned int vertexBufferCount = 1;
		// The "stride" defines how large a single vertex is in the stream of data
		constexpr unsigned int bufferStride = sizeof(VertexFormats::sMesh);
		// It's possible to start streaming data in the middle of a vertex buffer
		constexpr unsigned int bufferOffset = 0;
		direct3DImmediateContext->IASetVertexBuffers(startingSlot, vertexBufferCount, &m_vertexBuffer, &bufferStride, &bufferOffset);
	}
	// Bind a specific index buffer to the device as a data source for indices
	{
		EAE6320_ASSERT(m_indexBuffer);
		// Every index is a 16/32 bit unsigned integer
		const auto dxgiFormat = (m_type == IndexDataTypes::BIT_16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		// The indices start at the beginning of the buffer
		constexpr unsigned int offset = 0;
		direct3DImmediateContext->IASetIndexBuffer(m_indexBuffer, dxgiFormat, offset);
	}

	if (VertexFormats::g_layoutType != VertexFormats::MESH)
	{
		VertexFormats::g_layoutType = VertexFormats::MESH;
		// Specify what kind of data the vertex buffer holds
		// Set the layout (which defines how to interpret a single vertex)
		{
			EAE6320_ASSERT(ms_vertexInputLayout);
			direct3DImmediateContext->IASetInputLayout(ms_vertexInputLayout);
		}
		// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
		// the vertex buffer was defined as a triangle list
		// (meaning that every primitive is a triangle and will be defined by three vertices)
		direct3DImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	// Render triangles from the currently-bound vertex buffer
	{
		// It's possible to start rendering primitives in the middle of the stream
		constexpr unsigned int indexOfFirstIndexToUse = 0;
		constexpr unsigned int offsetToAddToEachIndex = 0;
		direct3DImmediateContext->DrawIndexed(m_numberOfIndices, indexOfFirstIndexToUse, offsetToAddToEachIndex);
	}
}