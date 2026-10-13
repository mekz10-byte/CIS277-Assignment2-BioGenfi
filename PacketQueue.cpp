#include "PacketQueue.h"
#include <cstring>

PacketQueue::PacketQueue(std::size_t blockSize, std::size_t blockCount)
    : pool(blockSize, blockCount), packets(blockCount) {}

bool PacketQueue::receive(std::size_t id, const unsigned char* bytes, std::size_t length)
{
    if (bytes == nullptr || length == 0 || length > pool.blockSize()
        || packets.full() || pool.availableBlocks() == 0) return false;
    auto* block = static_cast<unsigned char*>(pool.allocate());
    if (block == nullptr) return false;
    std::memcpy(block, bytes, length);
    if (!packets.enqueue(PacketDescriptor{id, block, length}))
    {
        pool.deallocate(block);
        return false;
    }
    return true;
}

bool PacketQueue::processNext(std::size_t& id, std::vector<unsigned char>& bytes)
{
    PacketDescriptor packet;
    if (!packets.front(packet)) return false;
    // Copy first so an allocation failure leaves the waiting packet intact.
    bytes.assign(packet.block, packet.block + packet.length);
    packets.dequeue(packet);
    id = packet.id;
    pool.deallocate(packet.block);
    return true;
}
std::size_t PacketQueue::pending() const { return packets.size(); }
std::size_t PacketQueue::availableBlocks() const { return pool.availableBlocks(); }
