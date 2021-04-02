// C mrousavy

#pragma once
#include <jsi/jsi.h>

#if __has_include(<RNReanimated/Scheduler.h>)
#include <RNReanimated/Scheduler.h>
#include <RNReanimated/ErrorHandler.h>
#else
#include "Tools/Scheduler.h"
#include "SpecTools/ErrorHandler.h"
#endif

using namespace facebook;

namespace mrousavy {
namespace multithreading {

void install(jsi::Runtime& runtime,
             std::function<std::shared_ptr<reanimated::Scheduler>()> makeScheduler,
             std::function<std::shared_ptr<reanimated::ErrorHandler>(std::shared_ptr<reanimated::Scheduler>)> makeErrorHandler);

}
}
