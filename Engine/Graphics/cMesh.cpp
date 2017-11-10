// Include Files
//==============

#include "cMesh.h"

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

	if (newMeshDataExtractedFromFile)
	{
		if (!((result = newMesh->Initialize(newMeshDataExtractedFromFile))))
		{
			EAE6320_ASSERTF(false, "Initialization of new mesh failed");
			goto OnExit;
		}
	}
	else
	{
		EAE6320_ASSERTF(false, "Initialization of new mesh failed as there was no data extracted from the mesh file");
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

	return result;
}

eae6320::Graphics::cMesh::~cMesh()
{
	CleanUp();
}
