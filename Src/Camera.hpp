#pragma once

#include <atomic>
#include "enums.hpp"

namespace FakeCamServer
{
	class FakeCamera
	{
	public:
		FakeCamera() noexcept : state_{ enums::CameraState::Off }, color_{ enums::CameraLedColor::Red }, rate_{0}
		{

		}

		FakeCamera(const FakeCamera& other) = delete;
		FakeCamera(FakeCamera&& other) = delete;

		FakeCamera& operator=(FakeCamera&& other) = delete;
		FakeCamera& operator=(const FakeCamera& other) = delete;

		void setLedState(const enums::CameraState state) noexcept
		{
			state_ = state;
		}

		const enums::CameraState getLedState() const noexcept
		{
			return state_;
		}

		void setLedColor(const enums::CameraLedColor color) noexcept
		{
			color_ = color;
		}

		const enums::CameraLedColor getLedColor() const noexcept
		{
			return color_;
		}

		void setRate(const int rate) noexcept
		{
			rate_ = rate;
		}

		const int getRate() const noexcept
		{
			return rate_;
		}

	private:
		std::atomic<enums::CameraState> state_;
		std::atomic<enums::CameraLedColor> color_;
		std::atomic<int> rate_;
	};
}//namespace FakeCamServer
