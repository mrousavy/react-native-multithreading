//
// Created by Marc Rousavy on 25.07.21.
//

#include "MultithreadingScheduler.h"
#include <fbjni/fbjni.h>

namespace mrousavy {


namespace multithreading {

using namespace facebook;
using TSelf = jni::local_ref<MultithreadingScheduler::jhybriddata>;

TSelf MultithreadingScheduler::initHybrid(jni::alias_ref<jhybridobject> jThis) {
    return makeCxxInstance(jThis);
}

void MultithreadingScheduler::scheduleOnUI(std::function<void()> job) {
    // 1. add job to queue
    uiJobs.push(job);
    scheduleTrigger();
}

void MultithreadingScheduler::scheduleTrigger() {
    // 2. schedule `triggerUI` to be called on the java thread
    static auto method = javaPart_->getClass()->getMethod<void()>("scheduleTrigger");
    method(javaPart_.get());
}

void MultithreadingScheduler::triggerUI() {
    // 3. call job we enqueued in step 1.
    auto job = uiJobs.pop();
    job();
}

void MultithreadingScheduler::registerNatives() {
    registerHybrid({
                       makeNativeMethod("initHybrid", MultithreadingScheduler::initHybrid),
                       makeNativeMethod("triggerUI", MultithreadingScheduler::triggerUI),
                   });
}

}// namespace multithreading
} // namespace mrousavy