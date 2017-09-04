/*
	WinMain() is the entry point of a Windows program
	(instead of the familiar main() in a console program)
*/

// Include Files
//==============

#include "cExampleGame.h"
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

// Entry Point
//============

int WINAPI WinMain( HINSTANCE i_thisInstanceOfTheApplication, HINSTANCE, char* i_commandLineArguments, int i_initialWindowDisplayState )
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_crtBreakAlloc = 698;
	return eae6320::Application::Run<eae6320::cExampleGame>( i_thisInstanceOfTheApplication, i_commandLineArguments, i_initialWindowDisplayState );
}
