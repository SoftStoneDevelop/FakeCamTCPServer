#pragma once

#include <string>

#include "../../ArrayPool/Src/MemoryOwner.hpp"
#include "../../ArrayPool/Src/MemoryOwnerFactory.hpp"

namespace MemoryExt
{
    static void fillBuffer(
        std::string&& str,
        ArrayPool::MemoryOwner<char>& memory,
        int& memorySize,
        const ArrayPool::MemoryOwnerFactory<char>& mof
    )
    {
        if ((memory.size() - memorySize) < str.size())
        {
            auto newResponce = mof.rentMemory((memorySize + str.size()) * 2);
            if (memorySize > 0)
            {
                std::copy(memory.data(), memory.data() + memorySize, newResponce.data());
            }
            memory = std::move(newResponce);
        }

        for (int i = 0; i < str.size(); i++)
        {
            memory.data()[memorySize++] = str[i];
        }
    }
}