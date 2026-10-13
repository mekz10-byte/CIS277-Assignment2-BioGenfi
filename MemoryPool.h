#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <cstddef>
#include "Stack.h"

class MemoryPool
{
private:
    unsigned char* memory;
    bool* inUse;

    size_t blockSize_;
    size_t blockCount_;
    size_t allocatedCount_;

    Stack<void*> freeBlocks;

public:
    MemoryPool(size_t blockSize, size_t blockCount);
    ~MemoryPool();

    void* allocate();
    bool deallocate(void* ptr);

    size_t availableBlocks() const;
    size_t allocatedBlocks() const;
    size_t blockSize() const;
    size_t capacity() const;
};

#endif