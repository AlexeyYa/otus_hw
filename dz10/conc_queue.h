/*! @file bulk.h
 *  @brief Пакетная обработка команд
 */

#ifndef CONQUEUE_H
#define CONQUEUE_H

#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>

template <typename T>
class ConcurrentQueue
{
public:
    /*void push(const T& data)
    {
        std::unique_lock<std::mutex> lk{m_mutex};
        m_queue.emplace(data);
        m_cv.notify_one();
    }*/
    void push(T&& data)
    {
        {
            std::lock_guard<std::mutex> lk(m_mutex);
            m_queue.emplace(std::move(data));
        }
        m_cv.notify_one();
    }
    bool empty() const
    {
        std::unique_lock<std::mutex> lk(m_mutex);
        return m_queue.empty();
    }
    bool wpop(T& data)
    {
        std::unique_lock<std::mutex> lk(m_mutex);
        m_cv.wait(lk, [&](){ return !m_queue.empty() || m_exit.load(); });
        if (m_exit.load() && m_queue.empty())
            return false;
        data = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }
    void exit()
    {
        m_exit.store(true);
        m_cv.notify_all();
    }
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_exit{false};
};

#endif
