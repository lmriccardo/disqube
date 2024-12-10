#ifndef _QUEUE_HPP
#define _QUEUE_HPP

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>

namespace Lib::Concurrency
{
    template <typename T>
    class Queue
    {
    private:
        std::queue<T> _queue;           // The queue containing elements of type T
        std::mutex _mutex;              // The mutex used for concurrency
        std::condition_variable _empty; // Conditional variable on items availability
        std::condition_variable _full;  // Conditional variable on residual space
        std::size_t _capacity;          // The total capacity of the queue

    public:
        Queue(const std::size_t capacity) : _capacity(capacity) {};
        Queue(const Queue<T> &other) = delete;
        ~Queue() = default;

        // The assign operator cannot be used since the mutex cannot be copied.
        // This holds also for the two condition variables
        Queue<T> &operator=(const Queue<T> &other) = delete;

        std::size_t getQueueCapacity() const;
        std::size_t getNofElements();
        bool isEmpty();

        void push(const T &element);
        T pop();
    };

    template <typename T>
    inline std::size_t Queue<T>::getQueueCapacity() const
    {
        return _capacity;
    }

    template <typename T>
    inline std::size_t Queue<T>::getNofElements()
    {
        // Lock the access to the resources. This operations
        // must be synchronized since it is critical
        std::unique_lock<std::mutex> lock(_mutex);

        return _queue.size();
    }

    template <typename T>
    inline bool Queue<T>::isEmpty()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _queue.empty();
    }

    template <typename T>
    inline void Queue<T>::push(const T &element)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _full.wait(lock, [this]()
                   { return this->_queue.size() < this->_capacity; });

        // When the condition has reached the situation in which
        // it can re-acquire the lock then push the element into the queue
        _queue.push(element);

        // Notify the waiting thread
        _empty.notify_one();
    }

    template <typename T>
    inline T Queue<T>::pop()
    {
        std::unique_lock<std::mutex> lock(_mutex);

        // Timeout waiting
        if (_empty.wait_for(lock, std::chrono::milliseconds(250),
                            [this]()
                            { return !this->_queue.empty(); }))
        {
            T element = _queue.front();
            _queue.pop();

            _full.notify_one();
            return std::move(element);
        }
        else
        {
            throw std::runtime_error("Event: timeout");
        }
    }

    template <typename T>
    using Queue_ptr = std::shared_ptr<Queue<T>>;
}

#endif