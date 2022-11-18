//
//  FrameProcessorPlugin.h
//  VisionCamera
//
//  Created by Marc Rousavy on 01.05.21.
//  Copyright © 2021 mrousavy. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <jsi/jsi.h>

@protocol MultiProcessorPluginBase
+ (void) registerWithRuntime:(facebook::jsi::Runtime *)runtime;
@end
