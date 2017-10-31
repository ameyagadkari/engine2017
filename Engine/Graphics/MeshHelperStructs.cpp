// Include Files
//==============

#include "MeshHelperStructs.h"
#include "VertexFormats.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

#include <cstdlib>

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::Graphics::HelperStructs::sMeshData::sMeshData(const uint8_t i_typeOfIndexData, const uint32_t i_numberOfVertices, const uint32_t i_numberOfIndices) :
	numberOfVertices(i_numberOfVertices),
	numberOfIndices(i_numberOfIndices),
	typeOfIndexData(i_typeOfIndexData)
{
	const auto isTypeOfIndexDataCorrect = ((i_typeOfIndexData == (sizeof(uint16_t) * 8)) || (i_typeOfIndexData == (sizeof(uint32_t) * 8)));
	if (isTypeOfIndexDataCorrect)
	{
		if (i_numberOfVertices > 2)
		{
			if ((!(i_numberOfIndices % 3) && (i_numberOfIndices > 0)))
			{
				indexData = (i_typeOfIndexData == (sizeof(uint16_t) * 8)) ? reinterpret_cast<uint16_t*>(malloc(i_numberOfIndices * sizeof(uint16_t))) : nullptr;
				if (!indexData)
				{
					indexData = (i_typeOfIndexData == (sizeof(uint32_t) * 8)) ? reinterpret_cast<uint32_t*>(malloc(i_numberOfIndices * sizeof(uint32_t))) : nullptr;
				}
				EAE6320_ASSERT(indexData);
				vertexData = reinterpret_cast<VertexFormats::sMesh*>(malloc(i_numberOfVertices * sizeof(VertexFormats::sMesh)));
				EAE6320_ASSERT(vertexData);
			}
			else
			{
				EAE6320_ASSERTF(false, "The number of indices should be a mutiple of 3");
				Logging::OutputError("Failed to initialize the mesh data because number of indices is not a multiple of 3");
			}
		}
		else
		{
			EAE6320_ASSERTF(false, "There should be atleast 3 vertices");
			Logging::OutputError("Failed to initialize the mesh data because number of vertices is less than 3");
		}
	}
	else
	{
		EAE6320_ASSERTF(isTypeOfIndexDataCorrect, "Incorrect index data type");
		Logging::OutputError("Failed to initialize the mesh data because of incorrect data type");
	}
}

eae6320::Graphics::HelperStructs::sMeshData::~sMeshData()
{
	if (vertexData)
	{
		free(vertexData);
		vertexData = nullptr;
	}
	if (indexData)
	{
		free(indexData);
		indexData = nullptr;
	}
}