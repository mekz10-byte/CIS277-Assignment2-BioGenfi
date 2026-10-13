#include "../PacketQueue.h"
#include <cassert>
#include <iostream>
#include <string>

int main()
{
    Queue<int> q(3);
    int out = 99;
    assert(q.empty() && !q.full() && q.size() == 0 && q.capacity() == 3);
    assert(!q.front(out) && out == 99);
    assert(!q.dequeue(out) && out == 99);
    assert(q.enqueue(10) && q.enqueue(20) && q.enqueue(30));
    assert(q.full() && !q.enqueue(40) && q.size() == 3);
    assert(q.front(out) && out == 10 && q.size() == 3);
    assert(q.dequeue(out) && out == 10);
    assert(q.enqueue(40));
    for (int expected : {20, 30, 40}) assert(q.dequeue(out) && out == expected);
    assert(q.empty() && !q.full());
    for (int i = 0; i < 100; ++i) { assert(q.enqueue(i)); assert(q.dequeue(out) && out == i); }
    Queue<std::string> words(2);
    std::string word = "unchanged";
    assert(!words.dequeue(word) && word == "unchanged");
    assert(words.enqueue("first") && words.enqueue("second"));
    assert(words.front(word) && word == "first" && words.size() == 2);
    assert(words.dequeue(word) && word == "first");
    assert(words.enqueue("third"));
    assert(words.dequeue(word) && word == "second");
    assert(words.dequeue(word) && word == "third");
    Queue<int> single(1);
    assert(single.enqueue(7) && !single.enqueue(8));
    assert(single.dequeue(out) && out == 7 && single.empty());
    PacketQueue packets(4, 3);
    const unsigned char one[] = {0xFF};
    const unsigned char exact[] = {0x00, 0xFF, 0x01, 0x80};
    const unsigned char large[] = {1, 2, 3, 4, 5};
    assert(!packets.receive(1, nullptr, 1));
    assert(!packets.receive(1, one, 0));
    assert(!packets.receive(1, large, 5));
    assert(packets.pending() == 0 && packets.availableBlocks() == 3);
    assert(packets.receive(11, one, 1));
    assert(packets.receive(22, exact, 4));
    assert(packets.receive(33, one, 1));
    assert(!packets.receive(44, one, 1));
    assert(packets.pending() == 3 && packets.availableBlocks() == 0);
    std::size_t id = 999;
    std::vector<unsigned char> bytes;
    assert(packets.processNext(id, bytes) && id == 11 && bytes == std::vector<unsigned char>{0xFF});
    assert(packets.pending() == 2 && packets.availableBlocks() == 1);
    assert(packets.receive(44, exact, 4));
    assert(packets.processNext(id, bytes) && id == 22 && bytes == std::vector<unsigned char>(exact, exact + 4));
    assert(packets.processNext(id, bytes) && id == 33);
    assert(packets.processNext(id, bytes) && id == 44 && bytes == std::vector<unsigned char>(exact, exact + 4));
    assert(packets.pending() == 0 && packets.availableBlocks() == 3);
    id = 999; bytes = {0xAA};
    assert(!packets.processNext(id, bytes) && id == 999 && bytes == std::vector<unsigned char>{0xAA});
    for (int i = 0; i < 100; ++i)
    {
        assert(packets.receive(i, exact, 4));
        assert(packets.processNext(id, bytes) && id == static_cast<std::size_t>(i));
        assert(bytes == std::vector<unsigned char>(exact, exact + 4));
        assert(packets.availableBlocks() == 3);
    }
    std::cout << "PASS: all independent queue and packet tests.\n";
}
