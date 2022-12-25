#pragma once

#include <string_view>

#include "SetLedState.hpp"
#include "../enums.hpp"
#include "../Extensions/MemoryOwnerExt.hpp"

namespace FakeCamServer
{
	SetLedState::SetLedState() noexcept : BaseCommand("set-led-state", 2, 3)
	{
	}

	void SetLedState::Execute(
		ArrayPool::MemoryOwner<char> args,
		int argsSize,
		std::promise<CommandResponse> promise,
		FakeCamera& camera,
		const ArrayPool::MemoryOwnerFactory<char>& mof
	)
	{
		CommandResponse responce{};
		auto sview = std::basic_string_view(args.data(), argsSize);
		if (enums::enum_to_string(enums::CameraState::Off) == sview)
		{
			camera.setLedState(enums::CameraState::Off);
			MemoryExt::fillBuffer("OK\n", responce.response, responce.responseSize, mof);
		}
		else if (enums::enum_to_string(enums::CameraState::On) == sview)
		{
			camera.setLedState(enums::CameraState::On);
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
