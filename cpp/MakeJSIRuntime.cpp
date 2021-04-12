//
//  MakeJSIRuntime.cpp
//  Multithreading
//
//  Created by Marc Rousavy on 02.04.21.
//  Copyright © 2021 Facebook. All rights reserved.
//

#include "MakeJSIRuntime.h"

#include <jsi/jsi.h>


#if __has_include(<hermes/hermes.h>)
// Hermes (https://hermesengine.dev)
#include <hermes/hermes.h>
#elif __has_include(<v8runtime/V8RuntimeFactory.h>)
// V8 (https://github.com/Kudo/react-native-v8)
#include <v8runtime/V8RuntimeFactory.h>
#elif __has_include(<jsi/JSCRuntime.h>)
// JSC (default)
#include <jsi/JSCRuntime.h>
#else
#error No jsi Runtime found!
#endif

using namespace facebook;

namespace mrousavy {
namespace multithreading {

std::unique_ptr<jsi::Runtime> makeJSIRuntime() {
#if __has_include(<hermes/hermes.h>)
  return facebook::hermes::makeHermesRuntime();
#elif __has_include(<v8runtime/V8RuntimeFactory.h>)
  return facebook::createV8Runtime("");
#elif __has_include(<jsi/JSCRuntime.h>)
  return facebook::jsc::makeJSCRuntime();
#else
  return std::unique_ptr<jsi::Runtime>(null);
#endif
}

}
}
