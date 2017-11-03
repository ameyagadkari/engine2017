// Include Files
//==============

#include "../UserInput.h"

#include <bitset>

// Static Data Initialization
//===========================

namespace eae6320
{
	namespace UserInput
	{
		bool g_isWindowInFocus = false;
		bool g_isMouseTracked = false;
		POINTS g_mousePoints;
	}
}

// Helper Function Declarations
//=============================

namespace
{
	std::bitset<UINT_FAST8_MAX>keyPress;
	std::bitset<UINT_FAST8_MAX>keyRelease;
}

// Interface
//==========

bool eae6320::UserInput::IsKeyPressed(const uint_fast8_t i_keyCode)
{
	if (!g_isWindowInFocus)return false;
	const auto keyState = GetAsyncKeyState(i_keyCode);
	const short isKeyDownMask = ~1;
	return (keyState & isKeyDownMask) != 0;
}

bool eae6320::UserInput::IsKeyPressedOnce(const uint_fast8_t i_keyCode)
{
	if (!g_isWindowInFocus)return false;
	const auto keyState = GetAsyncKeyState(i_keyCode);
	const short isKeyDownMask = ~1;
	if ((keyState & isKeyDownMask) != 0 && !keyPress[i_keyCode])
	{
		keyPress.set(i_keyCode);
		return true;
	}
	if ((keyState & isKeyDownMask) != 0)return false;
	if ((keyState & isKeyDownMask) == 0 && keyPress[i_keyCode])keyPress.reset(i_keyCode);
	return false;
}

bool eae6320::UserInput::IsKeyPressedAndReleased(const uint_fast8_t i_keyCode)
{
	if (!g_isWindowInFocus)return false;
	const auto keyState = GetAsyncKeyState(i_keyCode);
	const short isKeyDownMask = ~1;
	if ((keyState & isKeyDownMask) != 0 && !keyRelease[i_keyCode])
	{
		keyRelease.set(i_keyCode);
		return false;
	}
	if ((keyState & isKeyDownMask) != 0)return false;
	if ((keyState & isKeyDownMask) == 0 && keyRelease[i_keyCode])
	{
		keyRelease.reset(i_keyCode);
		return true;
	}
	return false;
}