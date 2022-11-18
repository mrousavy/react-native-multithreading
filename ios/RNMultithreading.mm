#import "RNMultithreading.h"
#import "RNMultithreadingInstaller.h"

#import <React/RCTBridge+Private.h>
#import <React/RCTUtils.h>
#import <ReactCommon/RCTTurboModuleManager.h>
#import <jsi/jsi.h>
#import <memory>

#import <RNReanimated/REAIOSScheduler.h>
#import "MultithreadingScheduler.h"
#import <RNReanimated/REAIOSErrorHandler.h>
#import "MakeJSIRuntime.h"

using namespace facebook;

@implementation RNMultithreading
@synthesize bridge = _bridge;
@synthesize methodQueue = _methodQueue;
static NSMutableDictionary * plugins;

RCT_EXPORT_MODULE()

+ (BOOL)requiresMainQueueSetup {
    return YES;
}

+(NSMutableDictionary*)getPlugins
{
    if (!plugins)
        plugins = [[NSMutableDictionary alloc] init];
    return plugins;
}

+(void)registerPlugin:(NSString *)name registerWithRuntime:(MultithreadingPlugin)registerWithRuntime
{
    if (!plugins) plugins = [[NSMutableDictionary alloc] init];
    [plugins setValue:registerWithRuntime forKey:name];
    
    
}


- (NSData *) loadBundle {
    NSURL *bundleUrl = [[NSBundle mainBundle] URLForResource:@"index" withExtension:@"js"];
    NSData *data = [NSData dataWithContentsOfURL:bundleUrl];
    return data;
}


- (dispatch_queue_t) createQueue {

    
    dispatch_queue_attr_t qos = dispatch_queue_attr_make_with_qos_class(DISPATCH_QUEUE_SERIAL, QOS_CLASS_USER_INITIATED, -1);
    dispatch_queue_t recordingQueue = dispatch_queue_create("recordingQueue", qos);
    return recordingQueue;
}

RCT_EXPORT_BLOCKING_SYNCHRONOUS_METHOD(install)
{
    RCTCxxBridge* cxxBridge = (RCTCxxBridge*)_bridge;
    if (cxxBridge == nil) {
        return @false;
    }
    
    auto callInvoker = cxxBridge.jsCallInvoker;

    auto makeRuntime = []() -> std::unique_ptr<jsi::Runtime> {
        return mrousavy::multithreading::makeJSIRuntime();
    };
    
    dispatch_queue_t processQueue = [self createQueue];
    auto makeScheduler = [callInvoker,processQueue]() -> std::shared_ptr<mrousavy::multithreading::MultithreadingScheduler> {
        return std::make_shared<mrousavy::multithreading::MultithreadingScheduler>(callInvoker,processQueue);
    };
    auto makeErrorHandler = [](std::shared_ptr<mrousavy::multithreading::MultithreadingScheduler> scheduler) -> std::shared_ptr<reanimated::ErrorHandler> {
        return std::make_shared<reanimated::REAIOSErrorHandler>(scheduler);
    };
    
    mrousavy::multithreading::install(*(jsi::Runtime *)cxxBridge.runtime,makeRuntime,makeScheduler,makeErrorHandler);
    return @true;
}

@end
