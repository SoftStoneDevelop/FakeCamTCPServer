#pragma once

#include "RequestProcessor.hpp"

namespace FakeCamServer
{
	RequestProcessor::RequestProcessor(
		std::shared_ptr<CommandManager> manager,
		std::shared_ptr<ArrayPool::MemoryOwnerFactory<char>> mof
	) : manager_{ std::move(manager) }, mof_{ std::move(mof) }, args_{nullptr}
	{
	}

	RequestProcessor::~RequestProcessor()
	{
		if (args_)
			delete args_;
	}

	void RequestProcessor::processData(
		const ArrayPool::MemoryOwner<char>& requestBuffer,
		const int requestDataSize,
		ArrayPool::MemoryOwner<char>& response,
		int& responseSize
	)
	{
		responseSize = 0;
		for (int i = 0; i < requestDataSize; i++)
		{
			if (requestBuffer.data()[i] == '\n')
			{
				if (skipToNewline_)
				{
					skipToNewline_ = false;
					continue;
				}
				
				if (recvArg_)
				{
					auto command = manager_->getCommands()[commandIndx_];
					if (command->GetMinArgLength() == 0 && argsSize_ == 0)
					{
						auto future = manager_->enqueue(args_ ? std::move(*args_) : ArrayPool::MemoryOwner<char>(), argsSize_, command);
						args_ = nullptr;
						recvArg_ = false;
						auto result = future.get();
						copyToResponseBuffer(std::move(result), response, responseSize);
						
					    commandIndx_ = 0;
						argsSize_ = 0;
						continue;
					}
				}
				else
				{
					fillBuffer("FALSE unknown command\n", response, responseSize);
				}
			}

			if (recvArg_)
			{
				if (reciveArgs(requestBuffer, requestDataSize, i, response, responseSize))
				{
					auto command = manager_->getCommands()[commandIndx_];
					auto future = manager_->enqueue(std::move(*args_), argsSize_, command);
					args_ = nullptr;
					auto result = future.get();
					copyToResponseBuffer(std::move(result), response, responseSize);

					commandIndx_ = 0;
					argsSize_ = 0;
				}
			}
			else
			{
				searchCommand(requestBuffer, requestDataSize, i);
			}
		}
	}

	void RequestProcessor::searchCommand(
		const ArrayPool::MemoryOwner<char>& requestBuff,
		const int& requestSize,
		const int& requestIndx
	)
	{
		bool needRescan = false;
		if (!partFindCommand(requestBuff, requestIndx))
		{
			commandIndx_ = 0;
			needRescan = positionMath_ != 0;
			positionMath_ = 0;
			recvArg_ = false;
			argsSize_ = 0;
		}

		if (needRescan)
			partFindCommand(requestBuff, requestIndx);
	}

	bool RequestProcessor::partFindCommand(
		const ArrayPool::MemoryOwner<char>& requestBuff,
		const int& requestIndx
	)
	{
		bool find = false;
		auto& commands = manager_->getCommands();
		for (int j = commandIndx_; j < commands.size(); j++)
		{
			auto command = commands[j];
			if (command->GetText().size() > positionMath_ && command->GetText()[positionMath_] == requestBuff.data()[requestIndx])
			{
				commandIndx_ = j;
				positionMath_++;
				find = true;
				if (command->GetText().size() == positionMath_)
				{
					recvArg_ = true;
					argsSize_ = 0;
					positionMath_ = 0;
				}
				break;
			}
		}

		return find;
	}

	bool RequestProcessor::reciveArgs(
		const ArrayPool::MemoryOwner<char>& requestBuff,
		const int& requestSize,
		int& requestIndx,
		ArrayPool::MemoryOwner<char>& responce,
		int& responceSize
	)
	{
		auto& commands = manager_->getCommands();
		if (args_ && args_->size() < commands[commandIndx_]->GetMaxArgLength())
		{
			delete args_;
			args_ = mof_->newRentMemory(commands[commandIndx_]->GetMaxArgLength());
		}

		for (; requestIndx < requestSize; requestIndx++)
		{
			if (requestBuff.data()[requestIndx] == ' ' && (argsSize_ == 0 || (requestIndx >= 0 && requestBuff.data()[requestIndx - 1] == ' ')))
				continue;

			if (requestBuff.data()[requestIndx] == '\n')
			{
				if (argsSize_ < commands[commandIndx_]->GetMinArgLength())
				{
					fillBuffer("FALSE Not enough arguments\n", responce, responceSize);
					recvArg_ = false;
					return false;
				}
				else
				{
					recvArg_ = false;
					return true;
				}
			}

			if (argsSize_ == commands[commandIndx_]->GetMaxArgLength())
			{
				fillBuffer("FALSE Wrong arguments length\n", responce, responceSize);
				skipToNewline_ = true;
				return false;
			}

			if (!args_)
			{
				args_ = mof_->newRentMemory(commands[commandIndx_]->GetMaxArgLength());
			}

			args_->data()[argsSize_ - 1] = requestBuff.data()[requestIndx];
			argsSize_++;
		}

		return false;
	}

	void RequestProcessor::fillBuffer(
		std::string&& responce,
		ArrayPool::MemoryOwner<char>& responceBuff,
		int& responceSize
	)
	{
		if ((responceBuff.size() - responceSize) < responce.size())
		{
			auto newResponce = mof_->rentMemory((responceSize + responce.size()) * 2);
			if (responceSize > 0)
			{
				std::copy(responceBuff.data(), responceBuff.data() + responceSize, newResponce.data());
			}
			responceBuff = std::move(newResponce);
		}

		for (int i = 0; i < responce.size(); i++)
		{
			responceBuff.data()[responceSize++] = responce[i];
		}
	}

	void RequestProcessor::copyToResponseBuffer(
		CommandResponse response,
		ArrayPool::MemoryOwner<char>& responceBuff,
		int& responceSize
	)
	{
		if ((responceBuff.size() - responceSize) < response.responseSize)
		{
			auto newResponse = mof_->rentMemory((responceSize + response.responseSize) * 2);
			if (responceSize > 0)
			{
				std::copy(responceBuff.data(), responceBuff.data() + responceSize, newResponse.data());
				responceBuff = std::move(newResponse);
			}
		}

		std::copy(response.response.data(), response.response.data() + response.responseSize, responceBuff.data() + responceSize);
		responceSize += response.responseSize;
	}

}//namespace FakeCamServer