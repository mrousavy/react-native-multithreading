#include "RNMultithreadingInstaller.h"

namespace mrousavy {
namespace multithreading {
void install(jsi::Runtime& runtime) {
  // spawnThread(() => Promise<void>)
  auto spawnThread = jsi::Function::createFromHostFunction(runtime,
                                                           jsi::PropNameID::forAscii(jsiRuntime, "spawnThread"),
                                                           1,  // func
                                                           [](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
    
    auto function = arguments[0].asObject(runtime).asFunction(runtime);
    std::thread
    
    auto keyName = convertJSIStringToNSString(runtime, arguments[0].getString(runtime));
    
    if (arguments[1].isBool()) {
      [MMKV.defaultMMKV setBool:arguments[1].getBool() forKey:keyName];
    } else if (arguments[1].isNumber()) {
      [MMKV.defaultMMKV setDouble:arguments[1].getNumber() forKey:keyName];
    } else if (arguments[1].isString()) {
      auto stringValue = convertJSIStringToNSString(runtime, arguments[1].getString(runtime));
      [MMKV.defaultMMKV setString:stringValue forKey:keyName];
    } else {
      throw jsi::JSError(runtime, "MMKV::set: 'value' argument is not of type bool, number or string!");
    }
    return jsi::Value::undefined();
  });
  jsiRuntime.global().setProperty(jsiRuntime, "mmkvSet", std::move(mmkvSet));
}
} // namespace multithreading
} // namespace mrousavy
