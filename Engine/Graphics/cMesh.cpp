// Include Files
//==============

#include "cMesh.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

#include <new>

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cMesh::Load(cMesh*& o_mesh, const HelperStructs::sMeshData& i_meshData)
{
	cResult result;

	cMesh* newMesh = nullptr;

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

	if (!((result = newMesh->Initialize(i_meshData))))
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

	return result;
}

eae6320::Graphics::cMesh::~cMesh()
{
	CleanUp();
}