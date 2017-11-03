/*
This file contains platform-specific global variables
*/

#ifndef EAE6320_APPLICATION_USERINPUT_WIN_H
#define EAE6320_APPLICATION_USERINPUT_WIN_H

// Data
//=====

// These are set and reset by a windows message
extern bool g_isWindowInFocus;
extern bool g_isMouseTracked;
extern POINTS g_mousePoints;

#endif	// EAE6320_APPLICATION_USERINPUT_WIN_H