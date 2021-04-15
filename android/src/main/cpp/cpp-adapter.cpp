#include <jni.h>
#include <jsi/jsi.h>
#include <memory>
#include <fbjni/fbjni.h>
#include <ReactCommon/CallInvokerHolder.h>

#include "RNMultithreadingInstaller.h"

#include "Scheduler.h"
#include "AndroidErrorHandler.h"
#include "AndroidScheduler.h"

using namespace facebook;
using namespace reanimated;

/*
To create the Scheduler/AndroidErrorHandler:
1.:     #include <fbjni/fbjni.h>
2.:     class AndroidScheduler : public jni::HybridClass<AndroidScheduler>;
3.:     jni::alias_ref<AndroidScheduler::javaobject> androidScheduler
4.:     api project(":react-native-reanimated")
*/

extern "C"
JNIEXPORT void JNICALL
Java_com_reactnativemultithreading_MultithreadingModule_nativeInstallMultithreading(
        JNIEnv *env,
        jclass clazz,
        jlong jsiPtr,
        jni::alias_ref<facebook::react::CallInvokerHolder::javaobject> jsCallInvokerHolder,
        jni::alias_ref<AndroidScheduler::javaobject> androidScheduler) {

    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);

    auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
    auto scheduler = androidScheduler->cthis()->getScheduler();
    scheduler->setJSCallInvoker(jsCallInvoker);

    auto makeScheduler = [scheduler]() -> std::shared_ptr<reanimated::Scheduler> {
        return scheduler;
    };
    auto makeErrorHandler = [](const std::shared_ptr<reanimated::Scheduler>& scheduler_) -> std::shared_ptr<reanimated::ErrorHandler> {
        return std::make_shared<reanimated::AndroidErrorHandler>(scheduler_);
    };
    mrousavy::multithreading::install(*runtime, makeScheduler, makeErrorHandler);

}