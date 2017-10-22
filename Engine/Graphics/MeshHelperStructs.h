#ifndef EAE6320_GRAPHICS_MESH_HELPER_STRUCTS_H
#define EAE6320_GRAPHICS_MESH_HELPER_STRUCTS_H

// Include Files
//==============

#include <cstdint>

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

// Helper Structs for Sprites
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
				uint8_t typeOfIndexData = 0;

				// Interface
				//==========

				// Initialization / Clean Up
				//--------------------------

				sMeshData(const uint8_t i_typeOfIndexData, const uint32_t i_numberOfVertices, const uint32_t i_numberOfIndices);
				~sMeshData();
			};
		}
	}
}

#endif // EAE6320_GRAPHICS_MESH_HELPER_STRUCTS_H