#import <React/RCTBridgeModule.h>
#import <jsi/jsi.h>

typedef id (^MultithreadingPlugin) (facebook::jsi::Runtime &runtime);

@interface RNMultithreading : NSObject <RCTBridgeModule>

@property (nonatomic, assign) BOOL setBridgeOnMainQueue;

+(NSMutableDictionary*)getPlugins;
+(void)registerPlugin:(NSString *)name registerWithRuntime:(MultithreadingPlugin)registerWithRuntime;

@end
