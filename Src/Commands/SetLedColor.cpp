#pragma once

#include <string_view>

#include "SetLedColor.hpp"
#include "enums.hpp"
#include "Extensions/MemoryOwnerExt.hpp"

namespace FakeCamServer
{
	SetLedColor::SetLedColor() noexcept : BaseCommand("set-led-color", 3, 5)
	{
	}

	void SetLedColor::Execute(
		ArrayPool::MemoryOwner<char> args,
		int argsSize,
		std::promise<CommandResponse> promise,
		FakeCamera& camera,
		const ArrayPool::MemoryOwnerFactory<char>& mof
	)
	{
		CommandResponse responce{};
		if (camera.getLedState() == enums::CameraState::Off)
		{
			MemoryExt::fillBuffer("FALSE Camera off\n", responce.response, responce.responseSize, mof);
			promise.set_value(std::move(responce));
			return;
		}

		auto sview = std::basic_string_view(args.data(), argsSize);
		if (enums::enum_to_string(enums::CameraLedColor::Red) == sview)
		{
			camera.setLedColor(enums::CameraLedColor::Red);
			MemoryExt::fillBuffer("OK\n", responce.response, responce.responseSize, mof);
		}
		else
		if (enums::enum_to_string(enums::CameraLedColor::Green) == sview)
		{
			camera.setLedColor(enums::CameraLedColor::Green);
			MemoryExt::fillBuffer("OK\n", responce.response, responce.responseSize, mof);
		}
		else
		if (enums::enum_to_string(enums::CameraLedColor::Blue) == sview)
		{
			camera.setLedColor(enums::CameraLedColor::Blue);
			MemoryExt::fillBuffer("OK\n", responce.response, responce.responseSize, mof);
		}
		else
		{
			MemoryExt::fillBuffer("FALSE incorrect argument\n", responce.response, responce.responseSize, mof);
			promise.set_value(std::move(responce));
			return;
		}

		promise.set_value(std::move(responce));
	}
}