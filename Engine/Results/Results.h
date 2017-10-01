/*
	This file defines general results that any system can use
*/

#ifndef EAE6320_RESULTS_H
#define EAE6320_RESULTS_H

// Include Files
//==============

#include "cResult.h"

// Result Definitions
//===================

namespace eae6320
{
	namespace Results
	{
		// General non-specific success/failure results
		constexpr cResult success( IsSuccess, System::General, __LINE__, Severity::Success );
		constexpr cResult Failure( IsFailure, System::General, __LINE__ );

		// This is returned when something is wrong with a file
		// (the file exists, but there is something wrong with its contents e.g. it is formatted incorrectly)
		constexpr cResult invalidFile( IsFailure, System::General, __LINE__ );
		// This is returned when a required file doesn't exist
		constexpr cResult fileDoesntExist( IsFailure, System::General, __LINE__ );
		// This is returned when something fails because not enough memory was available
		constexpr cResult outOfMemory( IsFailure, System::General, __LINE__ );
		// This is returned when something fails because some context-specific amount of time passed
		// (e.g. the task might have succeeded if more time had been spent,
		// the task might succeed if tried again at a later time, etc.)
		constexpr cResult timeOut( IsFailure, System::General, __LINE__, Results::Severity::Warning );

		// This can be compared against if the caller wants to know if no value was assigned
		constexpr cResult undefined;
	}
}

#endif	// EAE6320_RESULTS_H
