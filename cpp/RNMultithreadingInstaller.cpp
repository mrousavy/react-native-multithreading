#include "RNMultithreadingInstaller.h"
#include "ThreadPool.h"

#define MAX_THREAD_COUNT 2

namespace mrousavy {
namespace multithreading {
void install(jsi::Runtime& runtime) {
  ThreadPool pool(MAX_THREAD_COUNT);
  // TODO: Create runtimes for each thread pool? can I do on-demand instead?
  
  // spawnThread(() => Promise<void>)
  auto spawnThread = jsi::Function::createFromHostFunction(runtime,
                                                           jsi::PropNameID::forAscii(runtime, "spawnThread"),
                                                           1,  // func
                                                           [&pool](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
    auto function = arguments[0].asObject(runtime).asFunction(runtime);
    auto spawnThreadCallback = jsi::Function::createFromHostFunction(runtime,
                                                                     jsi::PropNameID::forAscii(runtime, "spawnThreadCallback"),
                                                                     2,
                                                                     [&](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
      auto resolver = [&runtime, &arguments](jsi::Value value) {
        arguments[0]
          .asObject(runtime)
          .asFunction(runtime)
          .call(runtime, jsi::Array::createWithElements(runtime, value), 1);
      };
      auto rejecter = [&runtime, &arguments](std::string message) {
        arguments[1]
          .asObject(runtime)
          .asFunction(runtime)
          .call(runtime, jsi::Array::createWithElements(runtime, jsi::JSError(runtime, message)), 1);
      };
      // TODO: Adapt Function -> Shared Value
      pool.enqueue([&resolver, &rejecter]() {
        try {
          // TODO: Call adapted function and get result back
          //auto result = jsi::Value(42);
          //resolver(jsi::Value(42));
        } catch (std::exception& exc) {
          rejecter(exc.what());
        }
      });
      return jsi::Value::undefined();
    });
    
    auto newPromise = runtime.global().getProperty(runtime, "Promise");
    auto promise = newPromise
                      .asObject(runtime)
                      .asFunction(runtime)
                      .call(runtime, jsi::Array::createWithElements(runtime, spawnThreadCallback), 1);
    
    return promise;
  });
  runtime.global().setProperty(runtime, "spawnThread", std::move(spawnThread));
}
} // namespace multithreading
} // namespace mrousavy
