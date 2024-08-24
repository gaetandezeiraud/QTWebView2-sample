#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <queue>
#include <functional>
#include <mutex>

class Dispatcher {
private:
    std::queue<std::function<void()>> _workQueue;
    std::mutex _queueMutex;

public:
    void Invoke(std::function<void()> action)
    {
        std::lock_guard<std::mutex> lock(_queueMutex);
        _workQueue.push(action);
    }

    void WorkLoop()
    {
        std::lock_guard<std::mutex> lock(_queueMutex);
        while (!_workQueue.empty())
        {
            std::function<void()> func = _workQueue.front();

            // Execute the function
            func();

            _workQueue.pop();
        }
    }

    bool IsEmpty()
    {
        std::lock_guard<std::mutex> lock(_queueMutex);
        return _workQueue.empty();
    }

    int Count()
    {
        std::lock_guard<std::mutex> lock(_queueMutex);
        return _workQueue.size();
    }
};

#endif // DISPATCHER_H
