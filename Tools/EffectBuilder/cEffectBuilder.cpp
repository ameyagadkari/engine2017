// Include Files
//==============

#include "cEffectBuilder.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Graphics/cRenderState.h>
#include <External/Lua/Includes.h>
#include <Tools/AssetBuildLibrary/Functions.h>

#include <fstream>


// Helper Function Declarations
//=============================

namespace
{
	using namespace eae6320::Assets;

	std::string vertexShaderPath;
	std::string fragmentShaderPath;
	uint8_t renderStateBits = 0;

	eae6320::cResult LoadBaseTable(lua_State& io_luaState);
	eae6320::cResult LoadShadersTable(lua_State& io_luaState);
	eae6320::cResult LoadShaderPath(lua_State& io_luaState, char const*const i_key, std::string& o_path);
	eae6320::cResult LoadRenderStatesTable(lua_State& io_luaState);
	eae6320::cResult LoadIndividualRenderState(lua_State& io_luaState, char const*const i_key);

}

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult cEffectBuilder::Build(const std::vector<std::string>&)
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
			const auto byteCountToWrite = sizeof(renderStateBits);
			fout.write(reinterpret_cast<const char*>(&renderStateBits), byteCountToWrite);
			if (!fout.good())
			{
				result = Results::fileWriteFail;
				OutputErrorMessageWithFileInfo(m_path_target, "Failed to write %zu bytes for render state bits", byteCountToWrite);
				goto OnExit;
			}
		}
		// Write vertex shader path
		{
			const auto byteCountToWrite = vertexShaderPath.length() + 1;

			// Write length of the path
			{
				const auto length = static_cast<uint8_t>(byteCountToWrite);
				fout.write(reinterpret_cast<const char*>(&length), 1);
				if (!fout.good())
				{
					result = Results::fileWriteFail;
					OutputErrorMessageWithFileInfo(m_path_target, "Failed to write 1 byte for vertex shader path length");
					goto OnExit;
				}
			}

			// Write actual path with null terminator
			{
				fout.write(vertexShaderPath.c_str(), byteCountToWrite);
				if (!fout.good())
				{
					result = Results::fileWriteFail;
					OutputErrorMessageWithFileInfo(m_path_target, "Failed to write %zu bytes for vertex shader path", byteCountToWrite);
					goto OnExit;
				}
			}
		}
		// Write fragment shader path
		{
			const auto byteCountToWrite = fragmentShaderPath.length() + 1;

			// Write length of the path
			{
				const auto length = static_cast<uint8_t>(byteCountToWrite);
				fout.write(reinterpret_cast<const char*>(&length), 1);
				if (!fout.good())
				{
					result = Results::fileWriteFail;
					OutputErrorMessageWithFileInfo(m_path_target, "Failed to write 1 byte for fragment shader path length");
					goto OnExit;
				}
			}

			// Write actual path with null terminator
			{
				fout.write(fragmentShaderPath.c_str(), byteCountToWrite);
				if (!fout.good())
				{
					result = Results::fileWriteFail;
					OutputErrorMessageWithFileInfo(m_path_target, "Failed to write %zu bytes for fragment shader path", byteCountToWrite);
					goto OnExit;
				}
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
		if (!((result = LoadShadersTable(io_luaState))))
		{
			return result;
		}
		if (!((result = LoadRenderStatesTable(io_luaState))))
		{
			return result;
		}
		return result;
	}

	eae6320::cResult LoadShadersTable(lua_State& io_luaState)
	{
		auto result = eae6320::Results::success;
		constexpr auto* const key = "shaders";
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
			if (!((result = LoadShaderPath(io_luaState, "vertex", vertexShaderPath))))
			{
				OutputErrorMessageWithFileInfo(__FILE__, "Failed to get vertex shader path");
				goto OnExit;
			}

			if (!((result = LoadShaderPath(io_luaState, "fragment", fragmentShaderPath))))
			{
				OutputErrorMessageWithFileInfo(__FILE__, "Failed to get fragment shader path");
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

		// Pop the shaders table
		lua_pop(&io_luaState, 1);

		return result;
	}

	eae6320::cResult LoadShaderPath(lua_State& io_luaState, char const*const i_key, std::string& o_path)
	{
		auto result = eae6320::Results::success;
		lua_pushstring(&io_luaState, i_key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			result = eae6320::Results::invalidFile;
			OutputErrorMessageWithFileInfo(__FILE__, "No value for key:\"%s\" was found in the table", i_key);
			goto OnExit;
		}
		if (lua_isstring(&io_luaState, -1))
		{
			o_path = lua_tostring(&io_luaState, -1);
			/*const char * const assetType = "shaders";
			if (!eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(sourceRelativePath, assetType, vertexShaderPathString, &errorMessage))
			{
				wereThereErrors = true;
				fprintf_s(stderr, "Cannot convert Convert Source Relative Path %s To Built Relative Path for Asset Type %s....Error: %s", sourceRelativePath, assetType, errorMessage.c_str());
				goto OnExit;
			}
			vertexShaderPath = _strdup(vertexShaderPathString.c_str());*/
		}
		else
		{
			result = eae6320::Results::invalidFile;
			OutputErrorMessageWithFileInfo(__FILE__, "The value at \"%s\" must be a string (instead of a %s)", i_key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}

	OnExit:
		lua_pop(&io_luaState, 1);

		return result;
	}

	eae6320::cResult LoadRenderStatesTable(lua_State& io_luaState)
	{
		auto result = eae6320::Results::success;
		constexpr auto* const key = "render_states";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			renderStateBits = 0;
			goto OnExit;
		}
		if (lua_istable(&io_luaState, -1))
		{
			if (!((result = LoadIndividualRenderState(io_luaState, "alpha_transparency"))))
			{
				goto OnExit;
			}

			if (!((result = LoadIndividualRenderState(io_luaState, "depth_buffering"))))
			{
				goto OnExit;
			}

			if (!((result = LoadIndividualRenderState(io_luaState, "draw_both_triangle_sides"))))
			{
				goto OnExit;
			}

			if (!((result = LoadIndividualRenderState(io_luaState, "wire_frame_mode"))))
			{
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

		// Pop the render states table
		lua_pop(&io_luaState, 1);

		return result;
	}

	eae6320::cResult LoadIndividualRenderState(lua_State& io_luaState, char const*const i_key)
	{
		auto result = eae6320::Results::success;
		lua_pushstring(&io_luaState, i_key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			goto OnExit;
		}
		if (lua_isboolean(&io_luaState, -1))
		{
			if (lua_toboolean(&io_luaState, -1))
			{
				if (!strcmp(i_key, "alpha_transparency"))
				{
					eae6320::Graphics::RenderStates::EnableAlphaTransparency(renderStateBits);
				}
				else if (!strcmp(i_key, "depth_buffering"))
				{
					eae6320::Graphics::RenderStates::EnableDepthBuffering(renderStateBits);				
				}
				else if (!strcmp(i_key, "draw_both_triangle_sides"))
				{
					eae6320::Graphics::RenderStates::EnableDrawingBothTriangleSides(renderStateBits);
				}
				else if (!strcmp(i_key, "wire_frame_mode"))
				{
					eae6320::Graphics::RenderStates::EnableWireFrameMode(renderStateBits);
				}
			}
			else
			{
				if (!strcmp(i_key, "alpha_transparency"))
				{
					eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
				}
				else if (!strcmp(i_key, "depth_buffering"))
				{
					eae6320::Graphics::RenderStates::DisableDepthBuffering(renderStateBits);
				}
				else if (!strcmp(i_key, "draw_both_triangle_sides"))
				{
					eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);
				}
				else if (!strcmp(i_key, "wire_frame_mode"))
				{
					eae6320::Graphics::RenderStates::DisableWireFrameMode(renderStateBits);
				}		
			}
		}
		else
		{
			result = eae6320::Results::invalidFile;
			OutputErrorMessageWithFileInfo(__FILE__, "The value at \"%s\" must be a boolean (instead of a %s)", i_key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}

	OnExit:
		lua_pop(&io_luaState, 1);

		return result;
	}

}
