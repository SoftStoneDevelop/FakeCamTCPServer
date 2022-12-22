#pragma once

#include <string>
#include "..\Camera.hpp"

namespace FakeCamServer
{
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
			char* args, int lengthArgs,
			char* result, int lengthResult,
			FakeCamera& camera
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