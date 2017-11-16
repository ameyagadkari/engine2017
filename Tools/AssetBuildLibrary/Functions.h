/*
	This file contains functions used by asset build tools
*/

#ifndef EAE6320_ASSETBUILD_FUNCTIONS_H
#define EAE6320_ASSETBUILD_FUNCTIONS_H

// Include Files
//==============

#include <Engine/Results/Results.h>
#include <External/Lua/Includes.h>

#include <string>

// Interface
//==========

namespace eae6320
{
	namespace Assets
	{
		cResult BuildAssets();

		// Helper Function Declarations
		//-----------------------------

		// These functions define functionality that is required by multiple builders

		cResult LoadFilePath(lua_State& io_luaState, char const*const i_key, std::string& o_path);
		cResult WriteFilePath(std::ofstream& io_fout, const std::string& i_path);

		// Error / Warning Output
		//-----------------------

		// These functions output asset build errors or warnings in the best way for the platform's build tools
		// (e.g. for Visual Studio the messages are formatted so that they'll show up in the Error List window)

		void OutputErrorMessage( const char* const i_errorMessage, ... );
		void OutputErrorMessageWithFileInfo( const char* const i_filePath,
			const char* const i_errorMessage, ... );
		void OutputErrorMessageWithFileInfo( const char* const i_filePath,
			const unsigned int i_lineNumber,
			const char* const i_errorMessage, ... );
		void OutputErrorMessageWithFileInfo( const char* const i_filePath,
			const unsigned int i_lineNumber, const unsigned int i_columnNumber,
			const char* const i_errorMessage, ... );

		void OutputWarningMessage( const char* const i_warningMessage, ... );
		void OutputWarningMessageWithFileInfo( const char* const i_filePath,
			const char* const i_warningMessage, ... );
		void OutputWarningMessageWithFileInfo( const char* const i_filePath,
			const unsigned int i_lineNumber,
			const char* const i_warningMessage, ... );
		void OutputWarningMessageWithFileInfo( const char* const i_filePath,
			const unsigned int i_lineNumber, const unsigned int i_columnNumber,
			const char* const i_warningMessage, ... );
	}
}

#endif	// EAE6320_ASSETBUILD_FUNCTIONS_H
