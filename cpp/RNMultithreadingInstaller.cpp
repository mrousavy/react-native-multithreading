#include "RNMultithreadingInstaller.h"
#include "ThreadPool.h"

#define MAX_THREAD_COUNT 2

namespace mrousavy {
namespace multithreading {
void install(jsi::Runtime& runtime) {
  ThreadPool pool(MAX_THREAD_COUNT);
  
  // spawnThread(() => Promise<void>)
  auto spawnThread = jsi::Function::createFromHostFunction(runtime,
                                                           jsi::PropNameID::forAscii(runtime, "spawnThread"),
                                                           1,  // func
                                                           [&pool](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
    auto function = arguments[0].asObject(runtime).asFunction(runtime);
    pool.enqueue([=]() {
      return 0;
    });
    
    return jsi::Value::undefined();
  });
  runtime.global().setProperty(runtime, "spawnThread", std::move(spawnThread));
}
} // namespace multithreading
} // namespace mrousavy
