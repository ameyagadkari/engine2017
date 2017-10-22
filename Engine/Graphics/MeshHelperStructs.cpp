// Include Files
//==============

#include "MeshHelperStructs.h"
#include "VertexFormats.h"

#include <cstdlib>

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::Graphics::HelperStructs::sMeshData::sMeshData(const uint8_t i_typeOfIndexData, const uint32_t i_numberOfVertices, const uint32_t i_numberOfIndices) :
	vertexData(reinterpret_cast<VertexFormats::sMesh*>(malloc(i_numberOfVertices * sizeof(VertexFormats::sMesh)))),
	numberOfVertices(i_numberOfVertices),
	numberOfIndices(i_numberOfIndices),
	typeOfIndexData(i_typeOfIndexData)
{
	indexData = (i_typeOfIndexData == (sizeof(uint16_t) * 8)) ? reinterpret_cast<uint16_t*>(malloc(i_numberOfIndices * sizeof(uint16_t))) : nullptr;
	if (!indexData)
	{
		indexData = (i_typeOfIndexData == (sizeof(uint32_t) * 8)) ? reinterpret_cast<uint32_t*>(malloc(i_numberOfIndices * sizeof(uint32_t))) : nullptr;
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