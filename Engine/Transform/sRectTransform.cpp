// Include Files
//==============

#include "sRectTransform.h"

#include <Engine/UserSettings/UserSettings.h>
#include <Engine/Graphics/SpriteHelperStructs.h>
#include "Engine/Asserts/Asserts.h"

// Static Data Initialization
//===========================

namespace
{
	auto s_isInitialized = false;
	float s_widthMultiplier = 0.0f;
	float s_heightMultiplier = 0.0f;
}

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::Transform::sRectTransform::sRectTransform() :
	width(0),
	height(0),
	anchor(UNKNOWN)
{
	pixelCoordinates.x = 0;
	pixelCoordinates.y = 0;
}

eae6320::Transform::sRectTransform::sRectTransform(
	const int16_t i_x,
	const int16_t i_y,
	const uint16_t i_width,
	const uint16_t i_height,
	const eAnchor i_anchor) :
	width(i_width),
	height(i_height),
	anchor(i_anchor)
{
	pixelCoordinates.x = i_x;
	pixelCoordinates.y = i_y;
}

// Access
//-------

void eae6320::Transform::sRectTransform::GetScreenPosition(Graphics::HelperStructs::sScreenPosition& o_screenPosition) const
{
	if (!s_isInitialized)
	{
		uint16_t screenWidth, screenHeight;
		UserSettings::GetDesiredInitialResolutionWidth(screenWidth);
		UserSettings::GetDesiredInitialResolutionHeight(screenHeight);
		EAE6320_ASSERT(screenWidth);
		EAE6320_ASSERT(screenHeight);
		s_widthMultiplier = 2.0f / screenWidth;
		s_heightMultiplier = 2.0f / screenHeight;
		s_isInitialized = true;
	}

	// Calculate screen coordinates according to anchor

#define EAE6320_CALCULATE_SCREEN_COORDINATES( i_xOffset, i_yOffset, i_widthModifier, i_heightModifier )		\
	o_screenPosition.left = ((pixelCoordinates.x - (width * i_widthModifier))*s_widthMultiplier) + i_xOffset;	\
	o_screenPosition.top = (pixelCoordinates.y + (height * i_heightModifier))*s_heightMultiplier + i_yOffset;


	switch (anchor)
	{
	case TOP_LEFT:
	{
		constexpr auto xOffset = -1.0f;
		constexpr auto yOffset = 1.0f;
		constexpr auto widthModifier = 0.0f;
		constexpr auto heightModifier = 0.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	case TOP_CENTER:
	{
		constexpr auto xOffset = 0.0f;
		constexpr auto yOffset = 1.0f;
		constexpr auto widthModifier = 0.5f;
		constexpr auto heightModifier = 0.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	case TOP_RIGHT:
	{
		constexpr auto xOffset = 1.0f;
		constexpr auto yOffset = 1.0f;
		constexpr auto widthModifier = 1.0f;
		constexpr auto heightModifier = 0.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	case MID_LEFT:
	{
		constexpr auto xOffset = -1.0f;
		constexpr auto yOffset = 0.0f;
		constexpr auto widthModifier = 0.0f;
		constexpr auto heightModifier = 0.5f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	case MID_CENTER:
	{
		constexpr auto xOffset = 0.0f;
		constexpr auto yOffset = 0.0f;
		constexpr auto widthModifier = 0.5f;
		constexpr auto heightModifier = 0.5f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	case MID_RIGHT:
	{
		constexpr auto xOffset = 1.0f;
		constexpr auto yOffset = 0.0f;
		constexpr auto widthModifier = 1.0f;
		constexpr auto heightModifier = 0.5f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	case BOTTOM_LEFT:
	{
		constexpr auto xOffset = -1.0f;
		constexpr auto yOffset = -1.0f;
		constexpr auto widthModifier = 0.0f;
		constexpr auto heightModifier = 1.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	case BOTTOM_CENTER:
	{
		constexpr auto xOffset = 0.0f;
		constexpr auto yOffset = -1.0f;
		constexpr auto widthModifier = 0.5f;
		constexpr auto heightModifier = 1.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	case BOTTOM_RIGHT:
	{
		constexpr auto xOffset = 1.0f;
		constexpr auto yOffset = -1.0f;
		constexpr auto widthModifier = 1.0f;
		constexpr auto heightModifier = 1.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	default:;
	}
	if (anchor != UNKNOWN)
	{
		o_screenPosition.right = o_screenPosition.left + (width*s_widthMultiplier);
		o_screenPosition.bottom = o_screenPosition.top - (height*s_heightMultiplier);
	}
#undef EAE6320_CALCULATE_SCREEN_COORDINATES

}
