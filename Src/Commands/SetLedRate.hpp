#pragma once

#include <string>
#include "BaseCommand.hpp"

namespace FakeCamServer
{
	class SetLedRate : public BaseCommand
	{
	public:
		SetLedRate() noexcept;

		void Execute(
			char* args, int lengthArgs,
			char* result, int lengthResult,
			FakeCamera& camera
		) override;
	};
}