// Include Files
//==============

#include "../UserInput.h"

#include <Engine/Windows/Includes.h>
#include <bitset>

namespace
{
	std::bitset<512>keyPress;
	std::bitset<512>keyRelease;
}

// Interface
//==========

bool eae6320::UserInput::IsKeyPressed( const uint_fast8_t i_keyCode )
{
	const auto keyState = GetAsyncKeyState( i_keyCode );
	const short isKeyDownMask = ~1;
	return ( keyState & isKeyDownMask ) != 0;
}

bool eae6320::UserInput::IsKeyPressedOnce(const uint_fast8_t i_keyCode)
{
	const short keyState = GetAsyncKeyState(i_keyCode);
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
	const short keyState = GetAsyncKeyState(i_keyCode);
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