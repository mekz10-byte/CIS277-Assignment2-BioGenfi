#include "MemoryPool.h"
#include <stdexcept>

MemoryPool::MemoryPool(size_t blockSize, size_t blockCount)
    : memory(nullptr),
      inUse(nullptr),
      blockSize_(blockSize),
      blockCount_(blockCount),
      allocatedCount_(0),
      freeBlocks(blockCount == 0 ? 1 : blockCount)
{
    if (blockSize == 0 || blockCount == 0)
    {
        throw std::invalid_argument(
            "Block size and block count must be greater than zero."
        );
    }

    memory = new unsigned char[blockSize_ * blockCount_];
    inUse = new bool[blockCount_];

    for (size_t i = 0; i < blockCount_; i++)
    {
        inUse[i] = false;

        void* blockAddress = memory + (i * blockSize_);
        freeBlocks.push(blockAddress);
    }
}


MemoryPool::~MemoryPool()
{
    delete[] inUse;
    delete[] memory;
}


void* MemoryPool::allocate()
{
    if (freeBlocks.empty())
    {
        return nullptr;
    }

    void* block = freeBlocks.pop();

    unsigned char* blockPtr =
        static_cast<unsigned char*>(block);

    size_t index =
        static_cast<size_t>(blockPtr - memory) / blockSize_;

    inUse[index] = true;
    allocatedCount_++;

    return block;
}


bool MemoryPool::deallocate(void* ptr)
{
    if (ptr == nullptr)
    {
        return false;
    }

    unsigned char* blockPtr =
        static_cast<unsigned char*>(ptr);

    if (blockPtr < memory ||
        blockPtr >= memory + (blockSize_ * blockCount_))
    {
        return false;
    }

    size_t offset =
        static_cast<size_t>(blockPtr - memory);

    if (offset % blockSize_ != 0)
    {
        return false;
    }

    size_t index = offset / blockSize_;

    if (!inUse[index])
    {
        return false;
    }

    inUse[index] = false;
    allocatedCount_--;

    freeBlocks.push(ptr);

    return true;
}


size_t MemoryPool::availableBlocks() const
{
    return freeBlocks.size();
}


size_t MemoryPool::allocatedBlocks() const
{
    return allocatedCount_;
}


size_t MemoryPool::blockSize() const
{
    return blockSize_;
}


size_t MemoryPool::capacity() const
{
    return blockSize_ * blockCount_;
}