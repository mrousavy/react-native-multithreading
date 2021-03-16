//
//  ThreadPool.cpp
//  Multithreading
//
//  Created by Marc Rousavy on 16.03.21.
//  Copyright © 2021 Facebook. All rights reserved.
//

#include "ThreadPool.h"
#include "MakeJSIRuntime.h"
#include <RNReanimated/RuntimeDecorator.h>
#include <sstream>

namespace mrousavy {
namespace multithreading {

// the constructor just launches some amount of workers
ThreadPool::ThreadPool(size_t threads): stop(false) {
  for (size_t i = 0; i < threads; ++i) {
    workers.emplace_back([this, i] {
      auto runtime = makeJSIRuntime();
      std::stringstream stringstream;
      stringstream << "THREAD #" << i;
      reanimated::RuntimeDecorator::decorateRuntime(*runtime, stringstream.str());
      
      while (true) {
        task_t task;
        
        {
          std::unique_lock<std::mutex> lock(this->queue_mutex);
          this->condition.wait(lock,
                               [this]{ return this->stop || !this->tasks.empty(); });
          if(this->stop && this->tasks.empty())
            return;
          task = std::move(this->tasks.front());
          this->tasks.pop();
        }
        
        task(*runtime);
      }
    });
  }
}

// add new work item to the pool
void ThreadPool::enqueue(task_t task) {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    
    // don't allow enqueueing after stopping the pool
    if (stop)
      throw std::runtime_error("enqueue on stopped ThreadPool");
    
    tasks.emplace(task);
  }
  condition.notify_one();
}

// the destructor joins all threads
ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true;
  }
  condition.notify_all();
  for (auto& worker : workers) {
    worker.join();
  }
}

}
}
