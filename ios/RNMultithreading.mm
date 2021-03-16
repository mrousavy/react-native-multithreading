#import "RNMultithreading.h"
#import "RNMultithreadingInstaller.h"

#import <React/RCTBridge+Private.h>
#import <React/RCTUtils.h>
#import <ReactCommon/RCTTurboModuleManager.h>
#import <jsi/jsi.h>
#import <memory>

#import <RNReanimated/REAIOSScheduler.h>
#import <RNReanimated/REAIOSErrorHandler.h>

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
  
  mrousavy::multithreading::install(*(jsi::Runtime *)cxxBridge.runtime, ^{
    return std::make_shared<reanimated::REAIOSScheduler>(bridge.jsCallInvoker);
  }, ^ (reanimated::Scheduler scheduler) {
    return reanimated::REAIOSErrorHandler(scheduler);
  });
}

@end
