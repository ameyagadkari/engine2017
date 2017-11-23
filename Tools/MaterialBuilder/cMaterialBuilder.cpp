// Include Files
//==============

#include "cMaterialBuilder.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Graphics/ConstantBufferFormats.h>
#include <External/Lua/Includes.h>
#include <Tools/AssetBuildLibrary/Functions.h>

#include <fstream>



// Helper Function Declarations
//=============================

namespace
{
	using namespace eae6320::Assets;

	std::string effectPath;
	std::string texturePath;
	eae6320::Graphics::ConstantBufferFormats::sPerMaterial constantData_perMaterial;

	eae6320::cResult LoadBaseTable(lua_State& io_luaState);
	eae6320::cResult LoadConstantBufferDataTable(lua_State& io_luaState);
	eae6320::cResult LoadColorTable(lua_State& io_luaState);
}

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult cMaterialBuilder::Build(const std::vector<std::string>&)
{
	auto result = Results::success;

	lua_State* luaState = nullptr;

	// Open the file for writing
	std::ofstream fout(m_path_target, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
	if (!fout.is_open())
	{
		result = Results::Failure;
		OutputErrorMessageWithFileInfo(m_path_target, "Target effect file couldn't be opened for writing");
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

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1

	if (!((result = LoadBaseTable(*luaState))))
	{
		result = Results::invalidFile;
		OutputErrorMessageWithFileInfo(m_path_target, "Failed to load base table");
		goto OnExit;
	}

	lua_pop(luaState, 1);

	// Write the effect data to a file
	{
		// Write render state bits
		{
			const auto byteCountToWrite = sizeof(constantData_perMaterial);
			fout.write(reinterpret_cast<const char*>(&constantData_perMaterial), byteCountToWrite);
			if (!fout.good())
			{
				result = Results::fileWriteFail;
				OutputErrorMessageWithFileInfo(m_path_target, "Failed to write %zu bytes for per material constant data", byteCountToWrite);
				goto OnExit;
			}
		}
		// Write effect shader path
		if (!((result = WriteFilePath(fout, effectPath))))
		{
			result = Results::fileWriteFail;
			OutputErrorMessageWithFileInfo(m_path_target, "Failed to write effect path");
			goto OnExit;
		}
		// Write texture shader path
		if (!((result = WriteFilePath(fout, texturePath))))
		{
			result = Results::fileWriteFail;
			OutputErrorMessageWithFileInfo(m_path_target, "Failed to write texture path");
			goto OnExit;
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

	if (fout.is_open())
	{
		fout.close();
		if (fout.is_open())
		{
			if (result)
			{
				result = Results::Failure;
			}
			OutputErrorMessageWithFileInfo(m_path_target, "Failed to close the target effect file after writing");
		}
	}

	return result;
}

// Helper Function Definitions
//============================

namespace
{
	eae6320::cResult LoadBaseTable(lua_State& io_luaState)
	{
		auto result = eae6320::Results::success;
		if (!((result = LoadConstantBufferDataTable(io_luaState))))
		{
			return result;
		}
		if (!((result = LoadFilePath(io_luaState, "effect", "effects", effectPath))))
		{
			OutputErrorMessageWithFileInfo(__FILE__, "Failed to get effect path");
			return result;
		}
		if (!((result = LoadFilePath(io_luaState, "texture", "textures", texturePath, false))))
		{
			OutputErrorMessageWithFileInfo(__FILE__, "Failed to get texture path");
			return result;
		}
		return result;
	}

	eae6320::cResult LoadConstantBufferDataTable(lua_State& io_luaState)
	{
		auto result = eae6320::Results::success;
		constexpr auto* const key = "constant_buffer_data";
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
			if (!((result = LoadColorTable(io_luaState))))
			{
				OutputErrorMessageWithFileInfo(__FILE__, "Failed to load the color table");
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

		// Pop the constant buffer data table
		lua_pop(&io_luaState, 1);

		return result;
	}

	eae6320::cResult LoadColorTable(lua_State& io_luaState)
	{
		auto result = eae6320::Results::success;
		constexpr auto* const key = "g_color";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			goto OnExit;
		}
		if (lua_istable(&io_luaState, -1))
		{
			const auto colorCount = luaL_len(&io_luaState, -1);
			float rgba[] = { 0.0f,0.0f,0.0f,0.0f };
			if (colorCount == 4)
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
				constantData_perMaterial.g_color.r = (rgba[0] >= 0.0f) ? ((rgba[0] <= 1.0f) ? rgba[0] : 1.0f) : 0.0f;
				constantData_perMaterial.g_color.g = (rgba[1] >= 0.0f) ? ((rgba[1] <= 1.0f) ? rgba[1] : 1.0f) : 0.0f;
				constantData_perMaterial.g_color.b = (rgba[2] >= 0.0f) ? ((rgba[2] <= 1.0f) ? rgba[2] : 1.0f) : 0.0f;
				constantData_perMaterial.g_color.a = (rgba[3] >= 0.0f) ? ((rgba[3] <= 1.0f) ? rgba[3] : 1.0f) : 0.0f;
			}
			else
			{
				result = eae6320::Results::invalidFile;
				OutputErrorMessageWithFileInfo(__FILE__, "There are %d channels instead of 4", colorCount);
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
		lua_pop(&io_luaState, 1);

		return result;
	}
}
