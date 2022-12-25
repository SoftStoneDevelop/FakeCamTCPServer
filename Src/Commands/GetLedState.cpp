#pragma once

#include "GetLedState.hpp"
#include "../enums.hpp"
#include "../Extensions/MemoryOwnerExt.hpp"

namespace FakeCamServer
{
	GetLedState::GetLedState() noexcept : BaseCommand("get-led-state", 0, 0)
	{
	}

	void GetLedState::Execute(
		ArrayPool::MemoryOwner<char> args,
		int argsSize,
		std::promise<CommandResponse> promise,
		FakeCamera& camera,
		const ArrayPool::MemoryOwnerFactory<char>& mof
	)
	{
		CommandResponse responce{};
		
		MemoryExt::fillBuffer("OK ", responce.response, responce.responseSize, mof);
		MemoryExt::fillBuffer(enums::enum_to_string(camera.getLedState()), responce.response, responce.responseSize, mof);
		MemoryExt::fillBuffer("\n", responce.response, responce.responseSize, mof);

		promise.set_value(std::move(responce));
	}
}
