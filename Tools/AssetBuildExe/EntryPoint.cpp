/*
The main() function is where the program starts execution
*/

// Include Files
//==============

#include <Engine/Results/Results.h>
#include <Tools/AssetBuildLibrary/Functions.h>

#include <cstdlib>
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

// Entry Point
//============

int main(const int i_argumentCount, char** i_arguments)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_crtBreakAlloc = 698;
	auto result = eae6320::Results::success;

	// The command line should have a path to the list of assets to build
	if (i_argumentCount == 2)
	{
		const auto* const path_assetsToBuild = i_arguments[1];
		result = eae6320::Assets::BuildAssets(path_assetsToBuild);
	}
	else
	{
		result = eae6320::Results::Failure;
		eae6320::Assets::OutputErrorMessageWithFileInfo(__FILE__, __LINE__,
			"AssetBuild.exe must be run with a single command line argument which is the path to the list of assets to build"
			" (the invalid argument count being passed to main is %u)", i_argumentCount);
	}
	return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
