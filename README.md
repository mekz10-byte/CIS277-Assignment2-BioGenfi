# CIS-277 Assignment 2: Network Packet Arrival Queue

## Student
Mekhi Bio-Genfi

## Design
Queue<T> uses a fixed-size vector, a front index, and a count. Enqueue uses
(frontIndex + count) % capacity. Dequeue advances frontIndex with modulo.
Nothing shifts, and the vector never grows.

PacketQueue owns a MemoryPool and a Queue<PacketDescriptor>. The queue controls
arrival order. The pool supplies and recycles storage. Each descriptor stores
an ID, a block pointer, and a length, without a second payload copy.
The pool has the same public interface as Assignment 1.

## How to Compile
From the project folder, use a compiler supporting C++23:

```sh
g++ -std=c++23 -Wall -Wextra -pedantic main.cpp PacketQueue.cpp MemoryPool.cpp -o packet_queue
g++ -std=c++23 -Wall -Wextra -pedantic tests/own_tests.cpp PacketQueue.cpp MemoryPool.cpp -o packet_tests
```

## How to Run
```sh
./packet_queue
./packet_tests
```

## Tests and Results
The demonstration processed IDs 101, 102, 103, and 104 in that order.
ID 104 arrived after ID 101 was processed. The freed block was available for
reuse, while IDs 102 and 103 stayed ahead of ID 104.

| Case | Expected | Actual |
| --- | --- | --- |
| int queue wraparound | 10 removed, then 20, 30, 40 | PASS |
| string queue | first, second, third in order | PASS |
| front without removal | First item returned; size unchanged | PASS |
| Queue overflow | false; count unchanged | PASS |
| Queue underflow | false; output unchanged | PASS |
| Capacity-one queue | Full after one enqueue; empty after dequeue | PASS |
| Packet FIFO after reuse | 11, 22, 33, 44 | PASS |
| Null pointer, zero length, oversized packet | false; counts unchanged | PASS |
| Length 1 and exactly blockSize | Accepted with correct output bytes | PASS |
| Binary 0x00 and 0xFF | Preserved exactly | PASS |
| Full packet queue | Rejected; pending 3, available 0 | PASS |
| Empty packet queue | false; ID and bytes unchanged | PASS |
| Drain all packets | pending 0, available 3 | PASS |
| 100 repeated enqueue/dequeue and receive/process cycles | Correct order, bytes, and counts | PASS |

C++23 builds completed with no warnings. Independent tests passed with AddressSanitizer
and UndefinedBehaviorSanitizer; leak detection was disabled because the test environment
cannot support it.

The supplied instructor validate.py and tests/public_tests.cpp were added unchanged.
Running python3 validate.py passed all public preflight checks, including
independent tests and compilation of the demonstration. The original workflow
was not supplied and the Canvas ZIP required sign-in, so an equivalent
.github/workflows/validate.yml runs the supplied validator with GCC 14 on GitHub.
The original workflow should replace it when available. Passing preflight is not a grade.

## Analysis Questions
1. **Why FIFO for packets and LIFO for free blocks?**
   Packets should be served in arrival order. Free blocks have no arrival priority,
   so the most recently released block can be reused first.
2. **How does the array wrap?**
   Modulo brings the index back to zero after the last slot. Only indices change;
   waiting packets do not shift.
3. **What happens at overflow and underflow?**
   The operation returns false. Keeping state unchanged prevents lost items,
   wrong counts, and overwritten output values.
4. **What are the time complexities?**
   Queue operations are O(1), excluding the cost of copying T. Successful receive
   and processNext are O(length), because they copy the packet bytes.
5. **Why copy before releasing?**
   A released block can be reused and overwritten. Copying first preserves
   the packet's data.

## AI Use
I used ChatGPT to help write the queue, packet integration, tests, and README.
The generated project was checked with C++23 builds, a demonstration, and
independent tests for FIFO, wraparound, binary data, and boundary cases.
