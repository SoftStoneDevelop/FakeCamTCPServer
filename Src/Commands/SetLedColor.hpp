#pragma once

#include <string>
#include "BaseCommand.hpp"

namespace FakeCamServer
{
	class SetLedColor : public BaseCommand
	{
	public:
		SetLedColor() noexcept;

		void Execute(
			char* args, int lengthArgs, 
			char* result, int lengthResult,
			FakeCamera& camera
		) override;
	};
}