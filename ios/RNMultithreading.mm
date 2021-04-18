#import "RNMultithreading.h"
#import "RNMultithreadingInstaller.h"

#import <React/RCTBridge+Private.h>
#import <React/RCTUtils.h>
#import <ReactCommon/RCTTurboModuleManager.h>
#import <jsi/jsi.h>
#import <memory>

#import <RNReanimated/REAIOSScheduler.h>
#import <RNReanimated/REAIOSErrorHandler.h>
#import "MakeJSIRuntime.h"

using namespace facebook;

@implementation RNMultithreading
@synthesize bridge = _bridge;
@synthesize methodQueue = _methodQueue;

RCT_EXPORT_MODULE()

+ (BOOL)requiresMainQueueSetup {
  return YES;
}

- (void)setBridge:(RCTBridge *)bridge
{
  _bridge = bridge;
  
  RCTCxxBridge *cxxBridge = (RCTCxxBridge *)self.bridge;
  if (!cxxBridge.runtime) {
    return;
  }
  
  auto callInvoker = bridge.jsCallInvoker;
  
  auto makeRuntime = []() -> std::unique_ptr<jsi::Runtime> {
    return mrousavy::multithreading::makeJSIRuntime();
  };
  auto makeScheduler = [callInvoker]() -> std::shared_ptr<reanimated::Scheduler> {
    return std::make_shared<reanimated::REAIOSScheduler>(callInvoker);
  };
  auto makeErrorHandler = [](std::shared_ptr<reanimated::Scheduler> scheduler) -> std::shared_ptr<reanimated::ErrorHandler> {
    return std::make_shared<reanimated::REAIOSErrorHandler>(scheduler);
  };
  
  mrousavy::multithreading::install(*(jsi::Runtime *)cxxBridge.runtime,
                                    makeRuntime,
                                    makeScheduler,
                                    makeErrorHandler);
}

@end
