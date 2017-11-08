// Include Files
//==============

#include "cMesh.h"
#include "MeshHelperStructs.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

// these will go away
#include <External/Lua/Includes.h>
#include "VertexFormats.h"
#include <iostream>
// !these will go away

#include <new>

// Static Data Initialization
//===========================

namespace
{
	eae6320::Graphics::HelperStructs::sMeshData* newMeshDataExtractedFromFile = nullptr;
	lua_State* luaState = nullptr;
}

// Helper Function Declarations
//=============================

namespace
{
	eae6320::cResult LoadBaseTable(lua_State& io_luaState);
	eae6320::cResult LoadVerticesTable(lua_State& io_luaState);
	eae6320::cResult LoadIndicesTable(lua_State& io_luaState);
	eae6320::cResult LoadPositionTable(lua_State& io_luaState, const int i_index);
	eae6320::cResult LoadColorTable(lua_State& io_luaState, const int i_index);
	uint8_t RoundColorChannel(const float i_value);
}

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cMesh::Load(const char* const i_path, cMesh*& o_mesh)
{
	auto result = Results::success;

	//Platform::sDataFromFile dataFromFile;
	cMesh* newMesh = nullptr;

	/*// Load the binary data
	{
	std::string errorMessage;
	if (!(result = Platform::LoadBinaryFile(i_path, dataFromFile, &errorMessage)))
	{
	EAE6320_ASSERTF(false, errorMessage.c_str());
	Logging::OutputError("Failed to load shader from file %s: %s", i_path, errorMessage.c_str());
	goto OnExit;
	}
	}*/

	// For now process lua file here
	{
		// Create a new Lua state

		{
			luaState = luaL_newstate();
			if (!luaState)
			{
				result = Results::outOfMemory;
				std::cerr << "Failed to create a new Lua state" << std::endl;
				goto OnExit;
			}
		}

		// Load the asset file as a "chunk",
		// meaning there will be a callable function at the top of the stack
		const auto stackTopBeforeLoad = lua_gettop(luaState);
		{
			const auto luaResult = luaL_loadfile(luaState, i_path);
			if (luaResult != LUA_OK)
			{
				result = Results::Failure;
				std::cerr << lua_tostring(luaState, -1) << std::endl;
				// Pop the error message
				lua_pop(luaState, 1);
				goto OnExit;
			}
		}
		// Execute the "chunk", which should load the asset
		// into a table at the top of the stack
		{
			constexpr int argumentCount = 0;
			constexpr int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
			constexpr int noErrorHandler = 0;
			const auto luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noErrorHandler);
			if (luaResult == LUA_OK)
			{
				// A well-behaved asset file will only return a single value
				const int returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
				if (returnedValueCount == 1)
				{
					// A correct asset file _must_ return a table
					if (!lua_istable(luaState, -1))
					{
						result = Results::invalidFile;
						std::cerr << "Asset files must return a table (instead of a "
							<< luaL_typename(luaState, -1) << ")" << std::endl;
						// Pop the returned non-table value
						lua_pop(luaState, 1);
						goto OnExit;
					}
				}
				else
				{
					result = Results::invalidFile;
					std::cerr << "Asset files must return a single table (instead of "
						<< returnedValueCount << " values)" << std::endl;
					// Pop every value that was returned
					lua_pop(luaState, returnedValueCount);
					goto OnExit;
				}
			}
			else
			{
				result = Results::invalidFile;
				std::cerr << lua_tostring(luaState, -1) << std::endl;
				// Pop the error message
				lua_pop(luaState, 1);
				goto OnExit;
			}
		}

		// If this code is reached the asset file was loaded successfully,
		// and its table is now at index -1

		newMeshDataExtractedFromFile = new (std::nothrow) HelperStructs::sMeshData();
		if (!((result = LoadBaseTable(*luaState))))
		{
			//OutputErrorMessage("Failed to load initial table", __FILE__);
			result = Results::invalidFile;
			goto OnExit;
		}

		lua_pop(luaState, 1);

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

	if (luaState)
	{
		// If I haven't made any mistakes
		// there shouldn't be anything on the stack
		// regardless of any errors
		EAE6320_ASSERT(lua_gettop(luaState) == 0);

		lua_close(luaState);
		luaState = nullptr;
	}

	if (newMeshDataExtractedFromFile)
	{
		delete newMeshDataExtractedFromFile;
	}

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

// Helper Function Definitions
//============================

namespace
{
	eae6320::cResult LoadBaseTable(lua_State& io_luaState)
	{
		auto result = eae6320::Results::success;
		if (!((result = LoadIndicesTable(io_luaState))))
		{
			return result;
		}
		if (!((result = LoadVerticesTable(io_luaState))))
		{
			return result;
		}
		return result;
	}
	eae6320::cResult LoadVerticesTable(lua_State& io_luaState)
	{
		auto result = eae6320::Results::success;
		constexpr auto* const key = "vertices";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			result = eae6320::Results::invalidFile;
			std::cerr << "No value for key:\"" << key << "\" was found in the table" << std::endl;
			goto OnExit;
		}
		if (lua_istable(&io_luaState, -1))
		{
			const auto vertexCount = luaL_len(&io_luaState, -1);
			if (vertexCount > 2)
			{
				newMeshDataExtractedFromFile->numberOfVertices = static_cast<uint32_t>(vertexCount);
				newMeshDataExtractedFromFile->vertexData = reinterpret_cast<eae6320::Graphics::VertexFormats::sMesh*>(malloc(newMeshDataExtractedFromFile->numberOfVertices * sizeof(eae6320::Graphics::VertexFormats::sMesh)));
				// Remember that Lua arrays are 1-based and not 0-based!
				for (auto i = 1; i <= vertexCount; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						result = eae6320::Results::invalidFile;
						std::cerr << "No value for key: \"" << i << "\"was found in the table" << std::endl;
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
					if (lua_istable(&io_luaState, -1))
					{
						if (!((result = LoadPositionTable(io_luaState, i - 1))))
						{
							lua_pop(&io_luaState, 1);
							goto OnExit;
						}
						if (!((result = LoadColorTable(io_luaState, i - 1))))
						{
							lua_pop(&io_luaState, 1);
							goto OnExit;
						}
						lua_pop(&io_luaState, 1);
					}
					else
					{
						result = eae6320::Results::invalidFile;
						std::cerr << "The value at \"" << i << "\" must be a table "
							"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}
			}
			else
			{
				result = eae6320::Results::invalidFile;
				std::cerr << "The number of vertex count should be greater than two as we are drawing triangles" << std::endl;
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::invalidFile;
			std::cerr << "The value at \"" << key << "\" must be a table "
				"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
			goto OnExit;
		}
	OnExit:
		// Pop the vertices table
		lua_pop(&io_luaState, 1);

		return result;
	}
	eae6320::cResult LoadIndicesTable(lua_State& io_luaState)
	{
		auto result = eae6320::Results::success;
		constexpr auto* const key = "indices";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			result = eae6320::Results::invalidFile;
			std::cerr << "No value for key:\"" << key << "\" was found in the table" << std::endl;
			goto OnExit;
		}
		if (lua_istable(&io_luaState, -1))
		{
			const auto indexCount = luaL_len(&io_luaState, -1);
			if (indexCount % 3 == 0)
			{
				newMeshDataExtractedFromFile->numberOfIndices = static_cast<uint32_t>(indexCount);
				uint16_t * indexData16Bit = nullptr;
				uint32_t * indexData32Bit = nullptr;
				if (newMeshDataExtractedFromFile->numberOfIndices > USHRT_MAX)
				{
					newMeshDataExtractedFromFile->typeOfIndexData = 32;
					newMeshDataExtractedFromFile->indexData = malloc(newMeshDataExtractedFromFile->numberOfIndices * sizeof(uint32_t));
					indexData32Bit = reinterpret_cast<uint32_t*>(newMeshDataExtractedFromFile->indexData);
				}
				else
				{
					newMeshDataExtractedFromFile->typeOfIndexData = 16;
					newMeshDataExtractedFromFile->indexData = malloc(newMeshDataExtractedFromFile->numberOfIndices * sizeof(uint16_t));
					indexData16Bit = reinterpret_cast<uint16_t*>(newMeshDataExtractedFromFile->indexData);
				}
				// Remember that Lua arrays are 1-based and not 0-based!
				for (auto i = 1; i <= indexCount; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						result = eae6320::Results::invalidFile;
						std::cerr << "No value for key: \"" << i << "\"was found in the table" << std::endl;
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
					if (lua_isnumber(&io_luaState, -1))
					{
#if defined( EAE6320_PLATFORM_D3D )
						if (newMeshDataExtractedFromFile->typeOfIndexData == 16)
						{
							indexData16Bit[newMeshDataExtractedFromFile->numberOfIndices - i] = static_cast<uint16_t>(lua_tonumber(&io_luaState, -1));
						}
						else
						{
							indexData32Bit[newMeshDataExtractedFromFile->numberOfIndices - i] = static_cast<uint32_t>(lua_tonumber(&io_luaState, -1));
						}
#elif defined( EAE6320_PLATFORM_GL )
						if (newMeshDataExtractedFromFile->typeOfIndexData == 16)
						{
							indexData16Bit[i - 1] = static_cast<uint16_t>(lua_tonumber(&io_luaState, -1));
						}
						else
						{
							indexData32Bit[i - 1] = static_cast<uint32_t>(lua_tonumber(&io_luaState, -1));
						}
#endif
						lua_pop(&io_luaState, 1);
					}
					else
					{
						result = eae6320::Results::invalidFile;
						std::cerr << "The value isn't a number!" << std::endl;
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}
			}
			else
			{
				result = eae6320::Results::invalidFile;
				std::cerr << "There are " << indexCount << " indices which is incorrect as we are drawing triangles" << std::endl;
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::invalidFile;
			std::cerr << "The value at \"" << key << "\" must be a table "
				"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
			goto OnExit;
		}
	OnExit:
		// Pop the indices table
		lua_pop(&io_luaState, 1);

		return result;
	}
	eae6320::cResult LoadPositionTable(lua_State& io_luaState, const int i_index)
	{
		auto result = eae6320::Results::success;
		constexpr auto* const key = "position";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			result = eae6320::Results::invalidFile;
			std::cerr << "No value for key:\"" << key << "\" was found in the table" << std::endl;
			goto OnExit;
		}
		if (lua_istable(&io_luaState, -1))
		{
			const auto positionCount = luaL_len(&io_luaState, -1);
			float xyz[] = { 0.0f,0.0f,0.0f };
			if (positionCount == 3)
			{
				for (auto i = 1; i <= positionCount; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						result = eae6320::Results::invalidFile;
						std::cerr << "No value for key: \"" << i << "\"was found in the table" << std::endl;
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
					if (lua_isnumber(&io_luaState, -1))
					{
						xyz[i - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
						lua_pop(&io_luaState, 1);
					}
					else
					{
						result = eae6320::Results::invalidFile;
						std::cerr << "The value isn't a number!" << std::endl;
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}
				newMeshDataExtractedFromFile->vertexData[i_index].x = xyz[0];
				newMeshDataExtractedFromFile->vertexData[i_index].y = xyz[1];
				newMeshDataExtractedFromFile->vertexData[i_index].z = xyz[2];
			}
			else
			{
				result = eae6320::Results::invalidFile;
				std::cerr << "There are " << positionCount << " coordinates instead of 3" << std::endl;
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::invalidFile;
			std::cerr << "The value at \"" << key << "\" must be a table "
				"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
			goto OnExit;
		}
	OnExit:
		// Pop the position table
		lua_pop(&io_luaState, 1);

		return result;
	}
	eae6320::cResult LoadColorTable(lua_State& io_luaState, const int i_index)
	{
		auto result = eae6320::Results::success;
		constexpr auto* const key = "color";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			result = eae6320::Results::invalidFile;
			std::cerr << "No value for key:\"" << key << "\" was found in the table" << std::endl;
			goto OnExit;
		}
		if (lua_istable(&io_luaState, -1))
		{
			const auto colorCount = luaL_len(&io_luaState, -1);
			float rgba[] = { 0.0f,0.0f,0.0f,0.0f };
			if ((colorCount == 3) || (colorCount == 4))
			{
				for (auto i = 1; i <= colorCount; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						result = eae6320::Results::invalidFile;
						std::cerr << "No value for key: \"" << i << "\"was found in the table" << std::endl;
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
					if (lua_isnumber(&io_luaState, -1))
					{
						rgba[i - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
						lua_pop(&io_luaState, 1);
					}
					else
					{
						result = eae6320::Results::invalidFile;
						std::cerr << "The value isn't a number!" << std::endl;
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}
				newMeshDataExtractedFromFile->vertexData[i_index].r = RoundColorChannel(rgba[0]);
				newMeshDataExtractedFromFile->vertexData[i_index].g = RoundColorChannel(rgba[1]);
				newMeshDataExtractedFromFile->vertexData[i_index].b = RoundColorChannel(rgba[2]);
				newMeshDataExtractedFromFile->vertexData[i_index].a = RoundColorChannel(rgba[3]);
			}
			else
			{
				result = eae6320::Results::invalidFile;
				std::cerr << "There are " << colorCount << " channels instead of 3 or 4" << std::endl;
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::invalidFile;
			std::cerr << "The value at \"" << key << "\" must be a table "
				"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
			goto OnExit;
		}
	OnExit:
		// Pop the color table
		lua_pop(&io_luaState, 1);

		return result;
	}
	uint8_t RoundColorChannel(const float i_value)
	{
		if (i_value < 0.0f)return 0;
		if (i_value > 1.0f)return UCHAR_MAX;
		return static_cast<uint8_t>(round(i_value * UCHAR_MAX));
	}
}