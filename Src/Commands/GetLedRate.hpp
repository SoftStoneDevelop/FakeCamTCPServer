#pragma once

#include <string>
#include "BaseCommand.hpp"

namespace FakeCamServer
{
	class GetLedRate : public BaseCommand
	{
	public:
		GetLedRate() noexcept;

		void Execute(
			char* args, int lengthArgs, 
			char* result, int lengthResult,
			FakeCamera& camera
		) override;
	};
}