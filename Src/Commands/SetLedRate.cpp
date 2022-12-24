#pragma once

#include <array>
#include <iterator>

#include "SetLedRate.hpp"
#include "enums.hpp"
#include "Extensions/MemoryOwnerExt.hpp"

namespace FakeCamServer
{
	SetLedRate::SetLedRate() noexcept : BaseCommand("set-led-rate", 1, 1)
	{
	}

	void SetLedRate::Execute(
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
		
		static const char numbers[] =
		{
			'0',
			'1',
			'2',
			'3',
			'4',
			'5'
		};
		
		auto it = std::find(std::begin(numbers), std::end(numbers), args.data()[0]);
		if (std::end(numbers) == it)
		{
			MemoryExt::fillBuffer("FALSE incorrect argument\n", responce.response, responce.responseSize, mof);
			promise.set_value(std::move(responce));
			return;
		}

		int rate = std::distance(numbers, it);
		camera.setRate(rate);
		MemoryExt::fillBuffer("OK\n", responce.response, responce.responseSize, mof);
		promise.set_value(std::move(responce));
	}
}