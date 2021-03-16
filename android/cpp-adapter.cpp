#include <jni.h>
#include <jsi/jsi.h>
#include <jsi/JSIDynamic.h>
#include "RNMultithreadingInstaller.h"
#include "AndroidErrorHandler.h"
#include "AndroidScheduler.h"
#include <memory>

using namespace facebook;

extern "C"
JNIEXPORT void JNICALL
Java_com_reactnativemultithreading_MultithreadingModule_nativeInstallMultithreading(JNIEnv *env,
        jobject clazz,
        jlong jsiPtr,
        jni::alias_ref<facebook::react::CallInvokerHolder::javaobject> jsCallInvokerHolder,
        jni::alias_ref<AndroidScheduler::javaobject> androidScheduler) {
    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);
    return mrousavy::multithreading::install(*runtime, []() {
        auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
        auto scheduler = androidScheduler->cthis()->getScheduler();
        scheduler->setJSCallInvoker(jsCallInvoker);
        return scheduler;
    }, [](std::shared_ptr<reanimated::AndroidScheduler> scheduler) {
        return std::make_shared<reanimated::AndroidErrorHandler>(scheduler);
    });
}
