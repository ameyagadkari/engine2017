// Include Files
//==============

#include "cMeshBuilder.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Graphics/MeshHelperStructs.h>
#include <Engine/Graphics/VertexFormats.h>
#include <Engine/Math/Half.h>
#include <External/Lua/Includes.h>
#include <Tools/AssetBuildLibrary/Functions.h>

#include <fstream>


// Helper Function Declarations
//=============================

namespace
{
	using namespace eae6320::Graphics::HelperStructs;
	using namespace eae6320::Assets;

	eae6320::cResult LoadBaseTable(lua_State& io_luaState, sMeshData& io_meshData);
	eae6320::cResult LoadVerticesTable(lua_State& io_luaState, sMeshData& io_meshData);
	eae6320::cResult LoadIndicesTable(lua_State& io_luaState, sMeshData& io_meshData);
	eae6320::cResult LoadPositionTable(lua_State& io_luaState, sMeshData& io_meshData, const int i_index);
	eae6320::cResult LoadColorTable(lua_State& io_luaState, sMeshData& io_meshData, const int i_index);
	eae6320::cResult LoadUVTable(lua_State& io_luaState, sMeshData& io_meshData, const int i_index);
	uint8_t RoundColorChannel(const float i_value);

	uint32_t originalNumberOfVertices = 0;
	const uint8_t paddingValue = 0xdd;
	uint8_t paddingRequired = 0;
}

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult cMeshBuilder::Build(const std::vector<std::string>&)
{
	auto result = Results::success;

	lua_State* luaState = nullptr;
	sMeshData* newMeshDataExtractedFromFile = nullptr;

	// Open the file for writing
	std::ofstream fout(m_path_target, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
	if (!fout.is_open())
	{
		result = Results::Failure;
		OutputErrorMessageWithFileInfo(m_path_target, "Target mesh file couldn't be opened for writing");
		goto OnExit;
	}

	// Create a new Lua state
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			result = Results::outOfMemory;
			OutputErrorMessageWithFileInfo(m_path_target, "Failed to create a new Lua state");
			goto OnExit;
		}
	}

	// Load the asset file as a "chunk",
	// meaning there will be a callable function at the top of the stack
	const auto stackTopBeforeLoad = lua_gettop(luaState);
	{
		const auto luaResult = luaL_loadfile(luaState, m_path_source);
		if (luaResult != LUA_OK)
		{
			result = Results::Failure;
			OutputErrorMessageWithFileInfo(m_path_target, lua_tostring(luaState, -1));
			// Pop the error message
			lua_pop(luaState, 1);
			goto OnExit;
		}
	}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	{
		constexpr auto argumentCount = 0;
		constexpr auto returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
		constexpr auto noErrorHandler = 0;
		const auto luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noErrorHandler);
		if (luaResult == LUA_OK)
		{
			// A well-behaved asset file will only return a single value
			const auto returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
			if (returnedValueCount == 1)
			{
				// A correct asset file _must_ return a table
				if (!lua_istable(luaState, -1))
				{
					result = Results::invalidFile;
					OutputErrorMessageWithFileInfo(m_path_target, "Asset files must return a table (instead of a %s)", luaL_typename(luaState, -1));
					// Pop the returned non-table value
					lua_pop(luaState, 1);
					goto OnExit;
				}
			}
			else
			{
				result = Results::invalidFile;
				OutputErrorMessageWithFileInfo(m_path_target, "Asset files must return a single table (instead of %d values)", returnedValueCount);
				// Pop every value that was returned
				lua_pop(luaState, returnedValueCount);
				goto OnExit;
			}
		}
		else
		{
			result = Results::invalidFile;
			OutputErrorMessageWithFileInfo(m_path_target, lua_tostring(luaState, -1));
			// Pop the error message
			lua_pop(luaState, 1);
			goto OnExit;
		}
	}

	// Allocate a new meshData
	{
		newMeshDataExtractedFromFile = new (std::nothrow) sMeshData();
		if (!newMeshDataExtractedFromFile)
		{
			result = Results::outOfMemory;
			OutputErrorMessageWithFileInfo(m_path_target, "Failed to allocate memory for the mesh data");
			goto OnExit;
		}
	}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1

	if (!((result = LoadBaseTable(*luaState, *newMeshDataExtractedFromFile))))
	{
		result = Results::invalidFile;
		OutputErrorMessageWithFileInfo(m_path_target, "Failed to load base table");
		goto OnExit;
	}

	lua_pop(luaState, 1);

	// Write the mesh data to a file
	{
		originalNumberOfVertices = newMeshDataExtractedFromFile->numberOfVertices;
		if (newMeshDataExtractedFromFile->type == Graphics::IndexDataTypes::BIT_32)
		{
			newMeshDataExtractedFromFile->numberOfVertices |= (INT32_MAX + 1u);
		}
		// Write vertex count
		{
			const auto byteCountToWrite = sizeof(newMeshDataExtractedFromFile->numberOfVertices);
			fout.write(reinterpret_cast<const char*>(&newMeshDataExtractedFromFile->numberOfVertices), byteCountToWrite);
			if (!fout.good())
			{
				result = Results::fileWriteFail;
				OutputErrorMessageWithFileInfo(m_path_target, "Failed to write %zu bytes for vertex count", byteCountToWrite);
				goto OnExit;
			}
		}
		// Write index count
		{
			const auto byteCountToWrite = sizeof(newMeshDataExtractedFromFile->numberOfIndices);
			fout.write(reinterpret_cast<const char*>(&newMeshDataExtractedFromFile->numberOfIndices), byteCountToWrite);
			if (!fout.good())
			{
				result = Results::fileWriteFail;
				OutputErrorMessageWithFileInfo(m_path_target, "Failed to write %zu bytes for index count", byteCountToWrite);
				goto OnExit;
			}
		}
		// Write index data
		{
			const auto byteCountToWrite = (newMeshDataExtractedFromFile->type == Graphics::IndexDataTypes::BIT_16) ? newMeshDataExtractedFromFile->numberOfIndices * sizeof(uint16_t) : newMeshDataExtractedFromFile->numberOfIndices * sizeof(uint32_t);
			fout.write(reinterpret_cast<const char*>(newMeshDataExtractedFromFile->indexData), byteCountToWrite);
			if (!fout.good())
			{
				result = Results::fileWriteFail;
				OutputErrorMessageWithFileInfo(m_path_target, "Failed to write %zu bytes for index data", byteCountToWrite);
				goto OnExit;
			}

			// Calculate and add required padding to align vertex data
			paddingRequired = byteCountToWrite % alignof(Graphics::VertexFormats::sMesh);
			if (paddingRequired > 0)
			{
				for (uint8_t i = 0; i < paddingRequired; i++)
				{
					fout.write(reinterpret_cast<const char*>(&paddingValue), 1u);
					if (!fout.good())
					{
						result = Results::fileWriteFail;
						OutputErrorMessageWithFileInfo(m_path_target, "Failed to write %zu bytes for padding", 1u);
						goto OnExit;
					}
				}
			}
		}
		// Write vertex data
		{
			const auto byteCountToWrite = originalNumberOfVertices * sizeof(Graphics::VertexFormats::sMesh);
			fout.write(reinterpret_cast<const char*>(newMeshDataExtractedFromFile->vertexData), byteCountToWrite);
			if (!fout.good())
			{
				result = Results::fileWriteFail;
				OutputErrorMessageWithFileInfo(m_path_target, "Failed to write %zu bytes for vertex data", byteCountToWrite);
				goto OnExit;
			}
		}
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

	if (fout.is_open())
	{
		fout.close();
		if (fout.is_open())
		{
			if (result)
			{
				result = Results::Failure;
			}
			OutputErrorMessageWithFileInfo(m_path_target, "Failed to close the target mesh file after writing");
		}
	}

	return result;
}

// Helper Function Definitions
//============================

namespace
{
	eae6320::cResult LoadBaseTable(lua_State& io_luaState, sMeshData& io_meshData)
	{
		auto result = eae6320::Results::success;
		if (!((result = LoadVerticesTable(io_luaState, io_meshData))))
		{
			return result;
		}
		if (!((result = LoadIndicesTable(io_luaState, io_meshData))))
		{
			return result;
		}
		return result;
	}

	eae6320::cResult LoadVerticesTable(lua_State& io_luaState, sMeshData& io_meshData)
	{
		auto result = eae6320::Results::success;
		constexpr auto* const key = "vertices";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			result = eae6320::Results::invalidFile;
			OutputErrorMessageWithFileInfo(__FILE__, "No value for key:\"%s\" was found in the table", key);
			goto OnExit;
		}
		if (lua_istable(&io_luaState, -1))
		{
			const auto vertexCount = luaL_len(&io_luaState, -1);
			if (vertexCount > 2 && vertexCount < INT32_MAX)
			{
				io_meshData.numberOfVertices = static_cast<uint32_t>(vertexCount);
				io_meshData.vertexData = reinterpret_cast<eae6320::Graphics::VertexFormats::sMesh*>(malloc(io_meshData.numberOfVertices * sizeof(eae6320::Graphics::VertexFormats::sMesh)));
				io_meshData.type = (io_meshData.numberOfVertices > UINT16_MAX) ? eae6320::Graphics::IndexDataTypes::BIT_32 :
					eae6320::Graphics::IndexDataTypes::BIT_16;

				// Remember that Lua arrays are 1-based and not 0-based!
				for (auto i = 1; i <= vertexCount; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						result = eae6320::Results::invalidFile;
						OutputErrorMessageWithFileInfo(__FILE__, "No value for key: \"%d\"was found in the table", i);
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
					if (lua_istable(&io_luaState, -1))
					{
						const auto arrayIndex = i - 1;
						if (!((result = LoadPositionTable(io_luaState, io_meshData, arrayIndex))))
						{
							lua_pop(&io_luaState, 1);
							goto OnExit;
						}
						if (!((result = LoadColorTable(io_luaState, io_meshData, arrayIndex))))
						{
							lua_pop(&io_luaState, 1);
							goto OnExit;
						}
						if (!((result = LoadUVTable(io_luaState, io_meshData, arrayIndex))))
						{
							lua_pop(&io_luaState, 1);
							goto OnExit;
						}
						lua_pop(&io_luaState, 1);
					}
					else
					{
						result = eae6320::Results::invalidFile;
						OutputErrorMessageWithFileInfo(__FILE__, "The value at \"%d\" must be a table (instead of a %s)", i, luaL_typename(&io_luaState, -1));
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}
			}
			else
			{
				result = eae6320::Results::invalidFile;
				OutputErrorMessageWithFileInfo(__FILE__, "The number of vertex count should be greater than two and less than %d", INT32_MAX);
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::invalidFile;
			OutputErrorMessageWithFileInfo(__FILE__, "The value at \"%s\" must be a table (instead of a %s)", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}
	OnExit:
		// Pop the vertices table
		lua_pop(&io_luaState, 1);

		return result;
	}

	eae6320::cResult LoadIndicesTable(lua_State& io_luaState, sMeshData& io_meshData)
	{
		auto result = eae6320::Results::success;
		constexpr auto* const key = "indices";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			result = eae6320::Results::invalidFile;
			OutputErrorMessageWithFileInfo(__FILE__, "No value for key:\"%s\" was found in the table", key);
			goto OnExit;
		}
		if (lua_istable(&io_luaState, -1))
		{
			const auto indexCount = luaL_len(&io_luaState, -1);
			if (indexCount % 3 == 0)
			{
				io_meshData.numberOfIndices = static_cast<uint32_t>(indexCount);
				uint16_t * indexData16Bit = nullptr;
				uint32_t * indexData32Bit = nullptr;
				if (io_meshData.type == eae6320::Graphics::IndexDataTypes::BIT_32)
				{
					io_meshData.indexData = malloc(io_meshData.numberOfIndices * sizeof(uint32_t));
					indexData32Bit = reinterpret_cast<uint32_t*>(io_meshData.indexData);
				}
				else
				{
					io_meshData.indexData = malloc(io_meshData.numberOfIndices * sizeof(uint16_t));
					indexData16Bit = reinterpret_cast<uint16_t*>(io_meshData.indexData);
				}
				// Remember that Lua arrays are 1-based and not 0-based!
				for (auto i = 1; i <= indexCount; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						result = eae6320::Results::invalidFile;
						OutputErrorMessageWithFileInfo(__FILE__, "No value for key: \"%d\"was found in the table", i);
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
					if (lua_isnumber(&io_luaState, -1))
					{
#if defined( EAE6320_PLATFORM_D3D )
						if (io_meshData.type == eae6320::Graphics::IndexDataTypes::BIT_16)
						{
							indexData16Bit[io_meshData.numberOfIndices - i] = static_cast<uint16_t>(lua_tonumber(&io_luaState, -1));
						}
						else
						{
							indexData32Bit[io_meshData.numberOfIndices - i] = static_cast<uint32_t>(lua_tonumber(&io_luaState, -1));
						}
#elif defined( EAE6320_PLATFORM_GL )
						if (io_meshData.type == eae6320::Graphics::IndexDataTypes::BIT_16)
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
						OutputErrorMessageWithFileInfo(__FILE__, "The value isn't a number!");
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}
			}
			else
			{
				result = eae6320::Results::invalidFile;
				OutputErrorMessageWithFileInfo(__FILE__, "There are %d indices which is incorrect as we are drawing triangles", indexCount);
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::invalidFile;
			OutputErrorMessageWithFileInfo(__FILE__, "The value at \"%s\" must be a table (instead of a %s)", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}
	OnExit:
		// Pop the indices table
		lua_pop(&io_luaState, 1);

		return result;
	}

	eae6320::cResult LoadPositionTable(lua_State& io_luaState, sMeshData& io_meshData, const int i_index)
	{
		auto result = eae6320::Results::success;
		constexpr auto* const key = "position";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			result = eae6320::Results::invalidFile;
			OutputErrorMessageWithFileInfo(__FILE__, "No value for key:\"%s\" was found in the table", key);
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
						OutputErrorMessageWithFileInfo(__FILE__, "No value for key: \"%d\"was found in the table", i);
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
						OutputErrorMessageWithFileInfo(__FILE__, "The value isn't a number!");
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}
				io_meshData.vertexData[i_index].x = xyz[0];
				io_meshData.vertexData[i_index].y = xyz[1];
				io_meshData.vertexData[i_index].z = xyz[2];
			}
			else
			{
				result = eae6320::Results::invalidFile;
				OutputErrorMessageWithFileInfo(__FILE__, "There are %d coordinates instead of 3", positionCount);
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::invalidFile;
			OutputErrorMessageWithFileInfo(__FILE__, "The value at \"%s\" must be a table (instead of a %s)", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}
	OnExit:
		// Pop the position table
		lua_pop(&io_luaState, 1);

		return result;
	}

	eae6320::cResult LoadColorTable(lua_State& io_luaState, sMeshData& io_meshData, const int i_index)
	{
		auto result = eae6320::Results::success;
		constexpr auto* const key = "color";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			result = eae6320::Results::invalidFile;
			OutputErrorMessageWithFileInfo(__FILE__, "No value for key:\"%s\" was found in the table", key);
			goto OnExit;
		}
		if (lua_istable(&io_luaState, -1))
		{
			const auto colorCount = luaL_len(&io_luaState, -1);
			float rgba[] = { 0.0f,0.0f,0.0f,1.0f };
			if ((colorCount == 3) || (colorCount == 4))
			{
				for (auto i = 1; i <= colorCount; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						result = eae6320::Results::invalidFile;
						OutputErrorMessageWithFileInfo(__FILE__, "No value for key: \"%d\"was found in the table", i);
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
						OutputErrorMessageWithFileInfo(__FILE__, "The value isn't a number!");
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}
				io_meshData.vertexData[i_index].r = RoundColorChannel(rgba[0]);
				io_meshData.vertexData[i_index].g = RoundColorChannel(rgba[1]);
				io_meshData.vertexData[i_index].b = RoundColorChannel(rgba[2]);
				io_meshData.vertexData[i_index].a = RoundColorChannel(rgba[3]);
			}
			else
			{
				result = eae6320::Results::invalidFile;
				OutputErrorMessageWithFileInfo(__FILE__, "There are %d channels instead of 3 or 4", colorCount);
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::invalidFile;
			OutputErrorMessageWithFileInfo(__FILE__, "The value at \"%s\" must be a table (instead of a %s)", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}
	OnExit:
		// Pop the color table
		lua_pop(&io_luaState, 1);

		return result;
	}

	eae6320::cResult LoadUVTable(lua_State& io_luaState, sMeshData& io_meshData, const int i_index)

	{
		auto result = eae6320::Results::success;
		constexpr auto* const key = "uv";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			result = eae6320::Results::invalidFile;
			OutputErrorMessageWithFileInfo(__FILE__, "No value for key:\"%s\" was found in the table", key);
			goto OnExit;
		}
		if (lua_istable(&io_luaState, -1))
		{
			const auto uvCount = luaL_len(&io_luaState, -1);
			float uv[] = { 0.0f,0.0f };
			if (uvCount == 2)
			{
				for (auto i = 1; i <= uvCount; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						result = eae6320::Results::invalidFile;
						OutputErrorMessageWithFileInfo(__FILE__, "No value for key: \"%d\"was found in the table", i);
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
					if (lua_isnumber(&io_luaState, -1))
					{
						uv[i - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
						lua_pop(&io_luaState, 1);
					}
					else
					{
						result = eae6320::Results::invalidFile;
						OutputErrorMessageWithFileInfo(__FILE__, "The value isn't a number!");
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}
				io_meshData.vertexData[i_index].u = eae6320::Math::MakeHalfFromFloat(uv[0]);
#if defined( EAE6320_PLATFORM_D3D )
				io_meshData.vertexData[i_index].v = eae6320::Math::MakeHalfFromFloat(1.0f - uv[1]);
#elif defined( EAE6320_PLATFORM_GL )
				io_meshData.vertexData[i_index].v = eae6320::Math::MakeHalfFromFloat(uv[1]);
#endif
			}
			else
			{
				result = eae6320::Results::invalidFile;
				OutputErrorMessageWithFileInfo(__FILE__, "There are %d texture coordinates instead of 2", uvCount);
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::invalidFile;
			OutputErrorMessageWithFileInfo(__FILE__, "The value at \"%s\" must be a table (instead of a %s)", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}
	OnExit:
		// Pop the position table
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