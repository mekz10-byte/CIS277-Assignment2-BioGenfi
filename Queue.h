#ifndef QUEUE_H
#define QUEUE_H
#include <cstddef>
#include <vector>
#include <stdexcept>

template <typename T>
class Queue
{
private:
    std::vector<T> data;
    std::size_t frontIndex = 0;
    std::size_t count = 0;
public:
    explicit Queue(std::size_t capacity) : data(capacity)
    {
        if (capacity == 0) throw std::invalid_argument("Capacity must be positive.");
    }
    bool enqueue(const T& value)
    {
        if (full()) return false;
        data[(frontIndex + count) % data.size()] = value;
        ++count;
        return true;
    }
    bool dequeue(T& value)
    {
        if (empty()) return false;
        value = data[frontIndex];
        frontIndex = (frontIndex + 1) % data.size();
        --count;
        return true;
    }
    bool front(T& value) const
    {
        if (empty()) return false;
        value = data[frontIndex];
        return true;
    }
    bool empty() const { return count == 0; }
    bool full() const { return count == data.size(); }
    std::size_t size() const { return count; }
    std::size_t capacity() const { return data.size(); }
};
#endif
