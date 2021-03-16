// Source: https://github.com/progschj/ThreadPool

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <jsi/jsi.h>

namespace mrousavy {
namespace multithreading {

class ThreadPool {
public:
  ThreadPool(size_t threadCount);
  std::future<void> enqueue(std::function<void()> task);
  ~ThreadPool();
private:
  // need to keep track of threads so we can join them
  std::vector<std::thread> workers;
  // the task queue
  std::queue<std::function<void()>> tasks;
  
  // synchronization
  std::mutex queue_mutex;
  std::condition_variable condition;
  bool stop;
};


}
}

#endif
