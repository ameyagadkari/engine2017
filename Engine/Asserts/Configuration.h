/*
	This file provides configurable settings
	that can be used to control assert behavior
*/

#ifndef EAE6320_ASSERTS_CONFIGURATION_H
#define EAE6320_ASSERTS_CONFIGURATION_H

// By default asserts are only enabled for debug builds,
// but you can #define it differently as necessary
#ifdef _DEBUG
	#define EAE6320_ASSERTS_ARE_ENABLED
#endif

#ifdef EAE6320_ASSERTS_ARE_ENABLED
	// For platforms and debuggers that support it
	// it can be useful to output the assert message to the debugger
	// so that there's a record of the message
	#define EAE6320_ASSERTS_SHOULD_PRINT_TO_DEBUGGER

#endif

#endif	// EAE6320_ASSERTS_CONFIGURATION_H
