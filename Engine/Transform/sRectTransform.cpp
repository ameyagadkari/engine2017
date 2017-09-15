// Include Files
//==============

#include "sRectTransform.h"

// Static Data Initialization
//===========================

namespace
{
	uint16_t s_screenWidth = 0;
	uint16_t s_screenHeight = 0;
}

// Interface
//==========

void eae6320::Transform::SetCurrentResolution(const uint16_t i_width, const uint16_t i_height)
{
	s_screenWidth = i_width;
	s_screenHeight = i_height;
}

eae6320::Transform::sRectTransform::sRectTransform() :
	width(100),
	height(100),
	anchor(TopLeft)
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


void eae6320::Transform::sRectTransform::GenerateNewScreenCoordinates()
{
	const float widthMultiplier = 2.0f / s_screenWidth;
	const float heightMultiplier = 2.0f / s_screenHeight;

	// Calculate screen coordinates according to anchor

#define EAE6320_CALCULATE_SCREEN_COORDINATES( i_xOffset, i_yOffset, i_widthModifier, i_heightModifier )		\
	screenPosition.left = ((pixelCoordinates.x - (width * i_widthModifier))*widthMultiplier) + i_xOffset;	\
	screenPosition.top = (pixelCoordinates.y + (height * i_heightModifier))*heightMultiplier + i_yOffset;
	

	switch (anchor)
	{
	case TopLeft:
	{
		constexpr float xOffset = -1.0f;
		constexpr float yOffset = 1.0f;
		constexpr float widthModifier = 0.0f;
		constexpr float heightModifier = 0.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	case TopCentre:
	{
		constexpr float xOffset = 0.0f;
		constexpr float yOffset = 1.0f;
		constexpr float widthModifier = 0.5f;
		constexpr float heightModifier = 0.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	case TopRight:
	{
		constexpr float xOffset = 1.0f;
		constexpr float yOffset = 1.0f;
		constexpr float widthModifier = 1.0f;
		constexpr float heightModifier = 0.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	case MidLeft:
	{
		constexpr float xOffset = -1.0f;
		constexpr float yOffset = 0.0f;
		constexpr float widthModifier = 0.0f;
		constexpr float heightModifier = 0.5f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	case MidCentre:
	{
		constexpr float xOffset = 0.0f;
		constexpr float yOffset = 0.0f;
		constexpr float widthModifier = 0.5f;
		constexpr float heightModifier = 0.5f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	case MidRight:
	{
		constexpr float xOffset = 1.0f;
		constexpr float yOffset = 0.0f;
		constexpr float widthModifier = 1.0f;
		constexpr float heightModifier = 0.5f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	case BottomLeft:
	{
		constexpr float xOffset = -1.0f;
		constexpr float yOffset = -1.0f;
		constexpr float widthModifier = 0.0f;
		constexpr float heightModifier = 1.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	case BottomCentre:
	{
		constexpr float xOffset = 0.0f;
		constexpr float yOffset = -1.0f;
		constexpr float widthModifier = 0.5f;
		constexpr float heightModifier = 1.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset, widthModifier, heightModifier)
	}
	break;
	case BottomRight:
	{
		constexpr float xOffset = 1.0f;
		constexpr float yOffset = -1.0f;
		constexpr float widthModifier = 1.0f;
		constexpr float heightModifier = 1.0f;
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
