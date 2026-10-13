#include "PacketQueue.h"
#include <iomanip>
#include <iostream>

void counts(const PacketQueue& q)
{
    std::cout << "Pending: " << q.pending() << ", available blocks: " << q.availableBlocks() << '\n';
}
void process(PacketQueue& q)
{
    std::size_t id = 0;
    std::vector<unsigned char> bytes;
    if (q.processNext(id, bytes))
    {
        std::cout << "Processed ID " << id << ", bytes:";
        for (unsigned char b : bytes)
            std::cout << ' ' << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(b);
        std::cout << std::dec << '\n';
        counts(q);
    }
}
int main()
{
    PacketQueue q(4, 3);
    const unsigned char a[] = {0x00, 0xFF, 0x10};
    const unsigned char b[] = {0x21};
    const unsigned char c[] = {0x30, 0x00, 0xFF, 0x31};
    const unsigned char d[] = {0x40, 0x41};
    std::cout << std::boolalpha;
    q.receive(101, a, sizeof(a));
    q.receive(102, b, sizeof(b));
    q.receive(103, c, sizeof(c));
    counts(q);
    std::cout << "Receive while full: " << q.receive(999, d, sizeof(d)) << '\n';
    counts(q);
    process(q);
    std::cout << "Receive ID 104 after release: " << q.receive(104, d, sizeof(d)) << '\n';
    std::cout << "Pool can reuse ID 101's block, but FIFO remains 102, 103, 104.\n";
    counts(q);
    process(q); process(q); process(q);
    std::size_t id = 777;
    std::vector<unsigned char> bytes = {0xAA};
    std::cout << "Process empty: " << q.processNext(id, bytes)
              << ", outputs unchanged: " << (id == 777 && bytes == std::vector<unsigned char>{0xAA}) << '\n';
    counts(q);
}
