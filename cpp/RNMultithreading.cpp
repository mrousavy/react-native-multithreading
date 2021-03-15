#include "RNMultithreading.h"

namespace vision {
void install(jsi::Runtime& runtime) {
  // spawnThread(() => Promise<void>)
  auto spawnThread = jsi::Function::createFromHostFunction(jsiRuntime,
                                                           jsi::PropNameID::forAscii(jsiRuntime, "spawnThread"),
                                                           1,  // func
                                                           [](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
    if (!arguments[0].is()) throw jsi::JSError(runtime, "MMKV::set: First argument ('key') has to be of type string!");
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
}
