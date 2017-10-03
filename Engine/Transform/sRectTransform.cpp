// Include Files
//==============

#include "sRectTransform.h"

#include <Engine/UserSettings/UserSettings.h>

// Static Data Initialization
//===========================

namespace
{
	auto is_initialized = false;
	uint16_t s_screenWidth = 0;	
	uint16_t s_screenHeight = 0;
}

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::Transform::sRectTransform::sRectTransform() :
	width(100),
	height(100),
	anchor(TOP_LEFT)
{
	pixelCoordinates.x = 0;
	pixelCoordinates.y = 0;
	GenerateNewScreenCoordinates();
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
	GenerateNewScreenCoordinates();
}

// Initialization / Clean Up
//--------------------------

void eae6320::Transform::sRectTransform::GenerateNewScreenCoordinates()
{
	if (!is_initialized)
	{
		UserSettings::GetDesiredInitialResolutionWidth(s_screenWidth);
		UserSettings::GetDesiredInitialResolutionHeight(s_screenHeight);
		is_initialized = true;
	}

	const auto widthMultiplier = 2.0f / s_screenWidth;
	const auto heightMultiplier = 2.0f / s_screenHeight;

	// Calculate screen coordinates according to anchor

#define EAE6320_CALCULATE_SCREEN_COORDINATES( i_xOffset, i_yOffset, i_widthModifier, i_heightModifier )		\
	screenPosition.left = ((pixelCoordinates.x - (width * i_widthModifier))*widthMultiplier) + i_xOffset;	\
	screenPosition.top = (pixelCoordinates.y + (height * i_heightModifier))*heightMultiplier + i_yOffset;


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
	default:
		break;
	}
	screenPosition.right = screenPosition.left + (width*widthMultiplier);
	screenPosition.bottom = screenPosition.top - (height*heightMultiplier);
#undef EAE6320_CALCULATE_SCREEN_COORDINATES

}
