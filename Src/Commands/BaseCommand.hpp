#pragma once

#include <string>
#include <future>

#include "..\Camera.hpp"
#include "ArrayPool.hpp"
#include "MemoryOwner.hpp"
#include "MemoryOwnerFactory.hpp"

namespace FakeCamServer
{
	struct CommandResponse
	{
		ArrayPool::MemoryOwner<char> response;
		int responseSize;
	};

	class BaseCommand
	{
	public:
		BaseCommand(
			std::string&& text,
			int minArgLength,
			int maxArgLength
		) noexcept : text_(text), minArgLength_(minArgLength), maxArgLength_(maxArgLength)
		{

		}

		virtual void Execute(
			ArrayPool::MemoryOwner<char> args,
			int argsSize,
			std::promise<CommandResponse> promise,
			FakeCamera& camera,
			const ArrayPool::MemoryOwnerFactory<char>& mof
		) = 0;

		const std::string& GetText() const noexcept
		{
			return text_;
		}

		const int GetMinArgLength() const noexcept
		{
			return minArgLength_;
		}

		const int GetMaxArgLength() const noexcept
		{
			return maxArgLength_;
		}
	private:
		std::string text_;
		int minArgLength_;
		int maxArgLength_;
	};
}