// Include Files
//==============

#include "MeshHelperStructs.h"
#include "VertexFormats.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::Graphics::HelperStructs::sMeshData::sMeshData(const IndexDataTypes::eType i_type, const uint32_t i_numberOfVertices, const uint32_t i_numberOfIndices)
	:
	numberOfVertices(i_numberOfVertices),
	numberOfIndices(i_numberOfIndices),
	type(i_type)
{
	const auto isTypeOfIndexDataCorrect = ((type == IndexDataTypes::BIT_16) || (type == IndexDataTypes::BIT_32));
	if (isTypeOfIndexDataCorrect)
	{
		if (numberOfVertices > 2)
		{
			if ((!(numberOfIndices % 3) && (numberOfIndices > 0)))
			{
				indexData = (type == IndexDataTypes::BIT_16) ? reinterpret_cast<uint16_t*>(malloc(numberOfIndices * sizeof(uint16_t))) : nullptr;
				if (!indexData)
				{
					indexData = (type == IndexDataTypes::BIT_32) ? reinterpret_cast<uint32_t*>(malloc(numberOfIndices * sizeof(uint32_t))) : nullptr;
				}
				EAE6320_ASSERT(indexData);
				vertexData = reinterpret_cast<VertexFormats::sMesh*>(malloc(numberOfVertices * sizeof(VertexFormats::sMesh)));
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

/*eae6320::Graphics::HelperStructs::sMeshData::~sMeshData()
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
}*/