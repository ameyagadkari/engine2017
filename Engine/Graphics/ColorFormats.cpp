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
	// Check Red Component
	{
		const auto localResult = i_r >= 0.0f && i_r <= 1.0f;
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
		const auto localResult = i_g >= 0.0f && i_g <= 1.0f;
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
		const auto localResult = i_b >= 0.0f && i_b <= 1.0f;
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
		const auto localResult = i_a >= 0.0f && i_a <= 1.0f;
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
	return result;
}