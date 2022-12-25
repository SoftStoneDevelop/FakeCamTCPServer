#pragma once

#include "GetLedRate.hpp"
#include "../enums.hpp"
#include "../Extensions/MemoryOwnerExt.hpp"

namespace FakeCamServer
{
	GetLedRate::GetLedRate() noexcept : BaseCommand("get-led-rate", 0, 0)
	{
	}

	void GetLedRate::Execute(
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
		MemoryExt::fillBuffer(std::to_string(camera.getRate()), responce.response, responce.responseSize, mof);
		MemoryExt::fillBuffer("\n", responce.response, responce.responseSize, mof);

		promise.set_value(std::move(responce));
	}
}
