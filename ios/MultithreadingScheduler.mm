//
//  VisionCameraScheduler.mm
//  VisionCamera
//
//  Created by Marc Rousavy on 23.07.21.
//  Copyright © 2021 mrousavy. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MultithreadingScheduler.h"

#import <React-callinvoker/ReactCommon/CallInvoker.h>

using namespace facebook;

namespace mrousavy {
    
    namespace multithreading {
        
        MultithreadingScheduler::MultithreadingScheduler(std::shared_ptr<react::CallInvoker> jsInvoker,dispatch_queue_t processQueue) {
            this->processQueue_ = processQueue;
            this->jsCallInvoker_ = jsInvoker;
        }
        
        // does not schedule on UI thread but rather on Frame Processor Thread
        void MultithreadingScheduler::scheduleOnUI(std::function<void()> job) {
            dispatch_async(this->processQueue_, ^{
                job();
            });
        }
        
        MultithreadingScheduler::~MultithreadingScheduler(){
        }
        
    } // namespace mrousavy
    
} // namespace vision
