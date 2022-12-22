#pragma once

#include <string>
#include "BaseCommand.hpp"

namespace FakeCamServer
{
	class GetLedState : public BaseCommand
	{
	public:
		GetLedState() noexcept;

		void Execute(
			char* args, int lengthArgs, 
			char* result, int lengthResult,
			FakeCamera& camera
		) override;
	};
}