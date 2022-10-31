//
// Created by Marc Rousavy on 25.07.21.
//

#pragma once

#include "Scheduler.h"
#include <ReactCommon/CallInvokerHolder.h>
#include <jni.h>
#include <fbjni/fbjni.h>

namespace mrousavy
{

namespace multithreading
{

using namespace facebook;

class MultithreadingScheduler: public reanimated::Scheduler, public jni::HybridClass<MultithreadingScheduler>
{
public:
    static auto constexpr kJavaDescriptor = "Lcom/reactnativemultithreading/MultithreadingScheduler;";
    static jni::local_ref<jhybriddata> initHybrid(jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();

    // schedules the given job to be run on the VisionCamera FP Thread at some future point in time
    void scheduleOnUI(std::function<void()> job) override;

private:
    friend HybridBase;
    jni::global_ref<MultithreadingScheduler::javaobject> javaPart_;

    explicit MultithreadingScheduler(jni::alias_ref<MultithreadingScheduler::jhybridobject> jThis)
        :
        javaPart_(jni::make_global(jThis))
    {}

    // Schedules a call to `triggerUI` on the VisionCamera FP Thread
    void scheduleTrigger();
    // Calls the latest job in the job queue
    void triggerUI() override;
};

} // namespace multithreading
} // namespace mrousavy