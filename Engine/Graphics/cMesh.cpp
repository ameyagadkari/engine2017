// Include Files
//==============

#include "cMesh.h"
#include "VertexFormats.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>

#include <new>


// Static Data Initialization
//===========================

eae6320::Assets::cManager<eae6320::Graphics::cMesh> eae6320::Graphics::cMesh::s_manager;

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cMesh::Load(const char* const i_path, cMesh*& o_mesh)
{
	auto result = Results::success;

	Platform::sDataFromFile dataFromFile;
	HelperStructs::sMeshData newMeshDataExtractedFromFile;
	cMesh* newMesh = nullptr;

	// Load the binary data
	{
		std::string errorMessage;
		if (!((result = LoadBinaryFile(i_path, dataFromFile, &errorMessage))))
		{
			EAE6320_ASSERTF(false, errorMessage.c_str());
			Logging::OutputError("Failed to load mesh from file %s: %s", i_path, errorMessage.c_str());
			goto OnExit;
		}
	}

	// Allocate a new mesh
	{
		newMesh = new (std::nothrow) cMesh();
		if (!newMesh)
		{
			result = Results::outOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the mesh %s", i_path);
			Logging::OutputError("Failed to allocate memory for the mesh %s", i_path);
			goto OnExit;
		}
	}

	// Extract data from loaded mesh file
	{
		// Casting data to uintptr_t for pointer arithematic

		auto currentOffset = reinterpret_cast<uintptr_t>(dataFromFile.data);
		const auto finalOffset = currentOffset + dataFromFile.size;

		// Extracting Type Of Index Data		

		newMeshDataExtractedFromFile.type = *reinterpret_cast<IndexDataTypes::eType*>(currentOffset);

		// Extracting Vertex Count

		currentOffset += sizeof(newMeshDataExtractedFromFile.type);
		newMeshDataExtractedFromFile.numberOfVertices = *reinterpret_cast<uint32_t*>(currentOffset);

		// Extracting Index Count

		currentOffset += sizeof(newMeshDataExtractedFromFile.numberOfVertices);
		newMeshDataExtractedFromFile.numberOfIndices = *reinterpret_cast<uint32_t*>(currentOffset);

		// Extracting Vertex Data

		currentOffset += sizeof(newMeshDataExtractedFromFile.numberOfIndices);
		newMeshDataExtractedFromFile.vertexData = reinterpret_cast<VertexFormats::sMesh*>(currentOffset);

		// Extracting Index Data

		currentOffset += newMeshDataExtractedFromFile.numberOfVertices * sizeof(VertexFormats::sMesh);
		newMeshDataExtractedFromFile.indexData = reinterpret_cast<void*>(currentOffset);

		// Check EOF

		currentOffset += newMeshDataExtractedFromFile.numberOfIndices * (newMeshDataExtractedFromFile.type == IndexDataTypes::BIT_16 ? sizeof(uint16_t) : sizeof(uint32_t));
		if(finalOffset != currentOffset)
		{
			result = Results::Failure;
			EAE6320_ASSERTF(false, "Redundant data found in file: \"%s\"", i_path);
			Logging::OutputError("Mesh file: \"%s\" has redundant data", i_path);
			goto OnExit;
		}
	}

	if (!((result = newMesh->Initialize(&newMeshDataExtractedFromFile))))
	{
		EAE6320_ASSERTF(false, "Initialization of new mesh failed");
		goto OnExit;
	}

OnExit:

	if (result)
	{
		EAE6320_ASSERT(newMesh);
		o_mesh = newMesh;
	}
	else
	{
		if (newMesh)
		{
			newMesh->DecrementReferenceCount();
			newMesh = nullptr;
		}
		o_mesh = nullptr;
	}
	dataFromFile.Free();
	newMeshDataExtractedFromFile.vertexData = nullptr;
	newMeshDataExtractedFromFile.indexData = nullptr;

	return result;
}

eae6320::Graphics::cMesh::~cMesh()
{
	CleanUp();
}
