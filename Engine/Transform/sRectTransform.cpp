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
	const auto widthMultiplier = 2.0f / s_screenWidth;
	const auto heightMultiplier = 2.0f / s_screenHeight;

	// Calculate screen coordinates according to anchor

#define EAE6320_CALCULATE_SCREEN_COORDINATES( i_xOffset, i_yOffset )												\
		screenPosition.left = ((pixelCoordinates.x - static_cast<int16_t>(width))*widthMultiplier) + i_xOffset;		\
		screenPosition.top = (pixelCoordinates.y*heightMultiplier) + i_yOffset;										\
		screenPosition.right = (pixelCoordinates.x*widthMultiplier) + i_xOffset;									\
		screenPosition.bottom = ((pixelCoordinates.y - static_cast<int16_t>(height))*heightMultiplier) + i_yOffset;

	switch (anchor)
	{
	case TopLeft:
	{
		constexpr float xOffset = 0.0f;
		constexpr float yOffset = 1.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset);
	}
	break;
	case TopCentre:
	{
		constexpr float xOffset = 0.5f;
		constexpr float yOffset = 1.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset);
	}
	break;
	case TopRight:
	{
		constexpr float xOffset = 1.0f;
		constexpr float yOffset = 1.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset);
	}
	break;
	case MidLeft:
	{
		constexpr float xOffset = 0.0f;
		constexpr float yOffset = 0.5f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset);
	}
	break;
	case MidCentre:
	{
		constexpr float xOffset = 0.5f;
		constexpr float yOffset = 0.5f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset);
	}
	break;
	case MidRight:
	{
		constexpr float xOffset = 1.0f;
		constexpr float yOffset = 0.5f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset);
	}
	break;
	case BottomLeft:
	{
		constexpr float xOffset = 0.0f;
		constexpr float yOffset = 0.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset);
	}
	break;
	case BottomCentre:
	{
		constexpr float xOffset = 0.5f;
		constexpr float yOffset = 0.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset);
	}
	break;
	case BottomRight:
	{
		constexpr float xOffset = 1.0f;
		constexpr float yOffset = 0.0f;
		EAE6320_CALCULATE_SCREEN_COORDINATES(xOffset, yOffset);
	}
	break;
	default:
		break;
	}

#undef EAE6320_CALCULATE_SCREEN_COORDINATES

}
