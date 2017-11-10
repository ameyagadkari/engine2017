#ifndef EAE6320_GRAPHICS_MESH_HELPER_STRUCTS_H
#define EAE6320_GRAPHICS_MESH_HELPER_STRUCTS_H

// Include Files
//==============

#include <cstdint>
#include <cstdlib>

// Forward Declarations
//=====================

namespace eae6320
{
	namespace Graphics
	{
		namespace VertexFormats
		{
			struct sMesh;
		}
	}
}

// Index Data Types
//=================

namespace eae6320
{
	namespace Graphics
	{
		// There is either 16 bit or 32 bit index data
		namespace IndexDataTypes
		{
			enum eType : uint8_t
			{
				UNKNOWN = 0,
				BIT_16 = 16,
				BIT_32 = 32
			};
		}
	}
}

// Helper Structs for Meshes
//===========================

namespace eae6320
{
	namespace Graphics
	{
		namespace HelperStructs
		{
			struct sMeshData
			{
				// Data
				//=====

				VertexFormats::sMesh* vertexData = nullptr;
				void* indexData = nullptr;
				uint32_t numberOfVertices = 0;
				uint32_t numberOfIndices = 0;
				IndexDataTypes::eType type = IndexDataTypes::UNKNOWN;

				// Interface
				//==========

				// Initialization / Clean Up
				//--------------------------

				sMeshData() = default;
				sMeshData(const IndexDataTypes::eType i_type, const uint32_t i_numberOfVertices, const uint32_t i_numberOfIndices);
				~sMeshData()
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
			};
		}
	}
}

#endif // EAE6320_GRAPHICS_MESH_HELPER_STRUCTS_H