#include "RNMultithreadingInstaller.h"
#include "ThreadPool.h"
#include <RNReanimated/Scheduler.h>
#include <RNReanimated/RuntimeManager.h>

#define MAX_THREAD_COUNT 2

namespace mrousavy {
namespace multithreading {

static ThreadPool pool(MAX_THREAD_COUNT);

//reanimated::RuntimeManager manager;

void install(jsi::Runtime& runtime) {
  // spawnThread(run: () => Promise<void>)
  auto spawnThread = jsi::Function::createFromHostFunction(runtime,
                                                           jsi::PropNameID::forAscii(runtime, "spawnThread"),
                                                           1,  // run
                                                           [](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
    auto function = arguments[0].asObject(runtime).asFunction(runtime);
    auto spawnThreadCallback = jsi::Function::createFromHostFunction(runtime,
                                                                     jsi::PropNameID::forAscii(runtime, "spawnThreadCallback"),
                                                                     2,
                                                                     [](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
      auto resolver = [&runtime, &arguments](jsi::Value value) {
        arguments[0]
          .asObject(runtime)
          .asFunction(runtime)
          .call(runtime, value);
      };
      auto rejecter = [&runtime, &arguments](std::string message) {
        arguments[1]
          .asObject(runtime)
          .asFunction(runtime)
          .call(runtime, jsi::JSError(runtime, message).value());
      };
      // TODO: Adapt Function -> Shared Value
      pool.enqueue([&resolver, &rejecter](const jsi::Runtime& runtime) {
        try {
          // TODO: Call adapted function and get result back
          //auto result = jsi::Value(42);
          
          // TODO: I probably have to call this on the other thread again.
          resolver(jsi::Value(42));
        } catch (std::exception& exc) {
          // TODO: I probably have to call this on the other thread again.
          rejecter(exc.what());
        }
      });
      return jsi::Value::undefined();
    });
    
    auto newPromise = runtime.global().getProperty(runtime, "Promise");
    auto promise = newPromise
                      .asObject(runtime)
                      .asFunction(runtime)
                      .call(runtime, spawnThreadCallback);
    
    return promise;
  });
  runtime.global().setProperty(runtime, "spawnThread", std::move(spawnThread));
}

} // namespace multithreading
} // namespace mrousavy
