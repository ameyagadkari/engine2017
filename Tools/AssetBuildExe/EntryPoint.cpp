/*
The main() function is where the program starts execution
*/

// Include Files
//==============

#include <cstdlib>
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include <Engine/Results/Results.h>
#include <Tools/AssetBuildLibrary/Functions.h>

// Entry Point
//============

int main(int i_argumentCount, char** i_arguments)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_crtBreakAlloc = 698;
	const auto result = eae6320::Assets::BuildAssets();
	return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
