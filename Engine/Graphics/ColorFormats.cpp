// Include Files
//==============

#include "ColorFormats.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

// Interface
//==========

eae6320::cResult eae6320::Graphics::ColorFormats::sColor::SetColor(const float i_r, const float i_g, const float i_b, const float i_a)
{
	auto result = Results::Success;
#define MIN_VAL 0.0f
#define MAX_VAL 1.0f
	// Check Red Component
	{
		const auto localResult = (i_r >= MIN_VAL) && (i_r <= MAX_VAL);
		if (localResult)
		{
			_r = i_r;
		}
		else
		{
			result = Results::Failure;
			EAE6320_ASSERTF(localResult, "Invalid red component value: %f", i_r);
			Logging::OutputError("Trying to assign red component out of range value (%f)", i_r);
			goto OnExit;
		}
	}

	// Check Green Component
	{
		const auto localResult = (i_g >= MIN_VAL) && (i_g <= MAX_VAL);
		if (localResult)
		{
			_g = i_g;
		}
		else
		{
			result = Results::Failure;
			EAE6320_ASSERTF(localResult, "Invalid green component value: %f", i_g);
			Logging::OutputError("Trying to assign green component out of range value (%f)", i_g);
			goto OnExit;
		}
	}

	// Check Blue Component
	{
		const auto localResult = (i_b >= MIN_VAL) && (i_b <= MAX_VAL);
		if (localResult)
		{
			_b = i_b;
		}
		else
		{
			result = Results::Failure;
			EAE6320_ASSERTF(localResult, "Invalid blue component value: %f", i_b);
			Logging::OutputError("Trying to assign blue component out of range value (%f)", i_b);
			goto OnExit;
		}
	}

	// Check Alpha Component
	{
		const auto localResult = (i_a >= MIN_VAL) && (i_a <= MAX_VAL);
		if (localResult)
		{
			_a = i_a;
		}
		else
		{
			result = Results::Failure;
			EAE6320_ASSERTF(localResult, "Invalid alpha component value: %f", i_a);
			Logging::OutputError("Trying to assign alpha component out of range value (%f)", i_a);
			goto OnExit;
		}
	}

OnExit:
#undef MIN_VAL 
#undef MAX_VAL 
	return result;
}