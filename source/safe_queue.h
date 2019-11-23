#include <condition_variable>
#include <queue>
#include <variant>
#include <mutex>


#ifndef _SAFE_QUEUE_H
#define _SAFE_QUEUE_H

template <typename T> 
struct safe_queue
{
  safe_queue() : mtx(), condition(), queue() {}

  void enqueue(T t)
  {
    std::unique_lock<std::mutex> l(mtx);
    queue.push(t);
    condition.notify_one();
  }

  T dequeue_wait()
  {
    std::unique_lock<std::mutex> l(mtx);
    while (queue.empty())
    {
      condition.wait(l);
    }
    T val = queue.front();
    queue.pop();
    return val;
  }

  std::variant<bool, T> 
  dequeue_if_not_empty()
  {
    std::unique_lock<std::mutex> l(mtx);
    if (!queue.empty())
    {
      std::variant<bool, T> ans(queue.front());
      queue.pop();
      return ans;
    }
    return std::variant<bool, T>(false);
  }

  size_t size()
  {
    std::unique_lock<std::mutex> l(mtx);
    return queue.size();
  }

    private:
  mutable std::mutex mtx;
  std::condition_variable condition;
  std::queue<T> queue;
};


#endif //_SAFE_QUEUE_H