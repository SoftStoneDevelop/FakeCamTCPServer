#pragma once

#include <string>
#include "BaseCommand.hpp"

namespace FakeCamServer
{
	class SetLedState : public BaseCommand
	{
	public:
		SetLedState() noexcept;

		void Execute(
			char* args, int lengthArgs,
			char* result, int lengthResult,
			FakeCamera& camera
		) override;
	};
}