//
//  VisionCameraScheduler.h
//  VisionCamera
//
//  Created by Marc Rousavy on 23.07.21.
//  Copyright © 2021 mrousavy. All rights reserved.
//

#pragma once

#include <functional>
#import <React-callinvoker/ReactCommon/CallInvoker.h>
#import <RNReanimated/Scheduler.h>

namespace mrousavy {

namespace multithreading {

class MultithreadingScheduler : public reanimated::Scheduler {
public:
    MultithreadingScheduler(std::shared_ptr<facebook::react::CallInvoker> jsInvoker,dispatch_queue_t processQueue);
    virtual ~MultithreadingScheduler();
    
    void scheduleOnUI(std::function<void()> job) override;
private:
    dispatch_queue_t processQueue_;
};

} // namespace multithreading

} // namespace mrousavy
