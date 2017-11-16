/*
The main() function is where the program starts execution
*/

// Include Files
//==============

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include "cMaterialBuilder.h"

// Entry Point
//============

int main(const int i_argumentCount, char** i_arguments)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_crtBreakAlloc = 698;
	return eae6320::Assets::Build<eae6320::Assets::cMaterialBuilder>(i_arguments, i_argumentCount);
}
