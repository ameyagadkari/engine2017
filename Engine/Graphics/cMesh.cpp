// Include Files
//==============

#include "cMesh.h"
#include "VertexFormats.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>

#include <new>


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
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the mesh");
			Logging::OutputError("Failed to allocate memory for the mesh");
			goto OnExit;
		}
	}

	// Extract data from loaded mesh file
	{
		// Casting data to uint8_t* for pointer arithematic

		auto data = reinterpret_cast<uint8_t*>(dataFromFile.data);

		// Extracting Type Of Index Data		

		newMeshDataExtractedFromFile.type = *reinterpret_cast<IndexDataTypes::eType*>(data);

		// Extracting Vertex Count

		data += sizeof(newMeshDataExtractedFromFile.type);
		newMeshDataExtractedFromFile.numberOfVertices = *reinterpret_cast<uint32_t*>(data);

		// Extracting Index Count

		data += sizeof(newMeshDataExtractedFromFile.numberOfVertices);
		newMeshDataExtractedFromFile.numberOfIndices = *reinterpret_cast<uint32_t*>(data);

		// Extracting Vertex Data

		data += sizeof(newMeshDataExtractedFromFile.numberOfIndices);
		newMeshDataExtractedFromFile.vertexData = reinterpret_cast<VertexFormats::sMesh*>(data);

		// Extracting Index Data

		data += newMeshDataExtractedFromFile.numberOfVertices * sizeof(VertexFormats::sMesh);
		newMeshDataExtractedFromFile.indexData = data;
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
