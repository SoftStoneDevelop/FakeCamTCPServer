#pragma once

#include "GetLedColor.hpp"
#include "enums.hpp"
#include "Extensions/MemoryOwnerExt.hpp"

namespace FakeCamServer
{
	GetLedColor::GetLedColor() noexcept : BaseCommand("get-led-color", 0, 0)
	{
	}

	void GetLedColor::Execute(
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

		MemoryExt::fillBuffer("OK ", responce.response, responce.responseSize, mof);
		MemoryExt::fillBuffer(enums::enum_to_string(camera.getLedColor()), responce.response, responce.responseSize, mof);
		MemoryExt::fillBuffer("\n", responce.response, responce.responseSize, mof);

		promise.set_value(std::move(responce));
	}
}