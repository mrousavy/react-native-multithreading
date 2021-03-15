#import "Multithreading.h"
#import "RNMultithreading.h"

#import <React/RCTBridge+Private.h>
#import <React/RCTUtils.h>
#import <jsi/jsi.h>

using namespace facebook;

@implementation Multithreading
@synthesize bridge = _bridge;
@synthesize methodQueue = _methodQueue;

RCT_EXPORT_MODULE()

+ (BOOL)requiresMainQueueSetup {
  return YES;
}

- (void)setBridge:(RCTBridge *)bridge
{
  _bridge = bridge;
  _setBridgeOnMainQueue = RCTIsMainQueue();
  
  RCTCxxBridge *cxxBridge = (RCTCxxBridge *)self.bridge;
  if (!cxxBridge.runtime) {
    return;
  }
  
  vision::install(*(jsi::Runtime *)cxxBridge.runtime);
}

@end
