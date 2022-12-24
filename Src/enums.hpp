#pragma once

#include <string>
#include <stdexcept>

namespace enums
{
	enum CameraState
	{
		Off = 0,
		On = 1
	};

	static std::string enum_to_string(CameraState state)
	{
		static const char* cameraState[] =
		{
			"Off",
			"On"
		};

		switch (state)
		{
		case enums::CameraState::Off:
		{
			return cameraState[0];
		}
		case enums::CameraState::On:
		{
			return cameraState[1];
		}
		default:
		{
			throw new std::exception("no match found for CameraState");
		}
		}
	}

	enum CameraLedColor
	{
		Red = 0,
		Green = 1,
		Blue = 2
	};

	static std::string enum_to_string(CameraLedColor color)
	{
		static const char* cameraLedColorStr[] =
		{
			"Red",
			"Green",
			"Blue"
		};

		switch (color)
		{
		case enums::CameraLedColor::Red:
		{
			return cameraLedColorStr[0];
		}
		case enums::CameraLedColor::Green:
		{
			return cameraLedColorStr[1];
		}
		case enums::CameraLedColor::Blue:
		{
			return cameraLedColorStr[2];
		}
		default:
		{
			throw new std::exception("no match found for CameraLedColor");
		}
		}
	}
}//namespace enums