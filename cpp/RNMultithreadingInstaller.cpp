#include "RNMultithreadingInstaller.h"
#include "ThreadPool.h"
#include <RNReanimated/Scheduler.h>
#include <RNReanimated/ShareableValue.h>
#include <RNReanimated/RuntimeManager.h>
#include <RNReanimated/RuntimeDecorator.h>
#include <RNReanimated/ErrorHandler.h>
#include "MakeJSIRuntime.h"

#define MAX_THREAD_COUNT 2

namespace mrousavy {
namespace multithreading {

static ThreadPool pool(1);
static std::unique_ptr<reanimated::RuntimeManager> manager;

//reanimated::RuntimeManager manager;

void install(jsi::Runtime& runtime) {
  // Quickly setup the runtime - this is executed in parallel, and _might_ introduce race conditions if spawnThread is called before this finishes.
  pool.enqueue([]() {
    auto runtime = makeJSIRuntime();
    reanimated::RuntimeDecorator::decorateRuntime(*runtime, "CUSTOM_THREAD");
    manager = std::make_unique<reanimated::RuntimeManager>(std::move(runtime),
                                                           std::shared_ptr<reanimated::ErrorHandler>(),
                                                           std::shared_ptr<reanimated::Scheduler>());
  });
  
  // spawnThread(run: () => T): Promise<T>
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
      // TODO: Get correct RuntimeManager instance
      auto run = reanimated::ShareableValue::adapt(runtime, arguments[0], manager.get());
      
      pool.enqueue([&resolver, &rejecter, run]() {
        try {
          auto& runtime = *manager->runtime;
          auto funcValue = run->getValue(runtime);
          auto func = funcValue.asObject(runtime).asFunction(runtime);
          auto result = func.callWithThis(runtime, func);
          
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
                      .callAsConstructor(runtime, spawnThreadCallback);
    
    return promise;
  });
  runtime.global().setProperty(runtime, "spawnThread", std::move(spawnThread));
}

} // namespace multithreading
} // namespace mrousavy
