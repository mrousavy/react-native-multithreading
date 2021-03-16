#include <jni.h>
#include <jsi/jsi.h>
#include "RNMultithreadingInstaller.h"
#include "AndroidErrorHandler.h"
#include "AndroidScheduler.h"
#include <memory>

using namespace facebook;

extern "C"
JNIEXPORT void JNICALL
Java_com_reactnativemultithreading_MultithreadingModule_nativeInstallMultithreading(JNIEnv *env, jobject clazz, jlong jsiPtr, reanimated::AndroidScheduler scheduler) {
    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);
    return mrousavy::multithreading::install(*runtime, []() {
        return std::make_shared<reanimated::AndroidScheduler>(nullptr);
    }, [](std::shared_ptr<reanimated::AndroidScheduler> scheduler) {
        return std::make_shared<reanimated::AndroidErrorHandler>(scheduler);
    });
}
