#include <jni.h>
#include <jsi/jsi.h>
#include <memory>

#include "RNMultithreadingInstaller.h"

#include "Tools/Scheduler.h"
#include "SpecTools/ErrorHandler.h"

using namespace facebook;

extern "C"
JNIEXPORT void JNICALL
Java_com_reactnativemultithreading_MultithreadingModule_nativeInstallMultithreading(JNIEnv *env, jobject clazz, jlong jsiPtr) {
    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);

    auto makeScheduler = []() -> std::shared_ptr<reanimated::Scheduler> {
        // TODO: Actually create valid AndroidScheduler
        return std::shared_ptr<reanimated::Scheduler>();
    };
    auto makeErrorHandler = []() -> std::shared_ptr<reanimated::ErrorHandler> {
        // TODO: Actually create valid AndroidErrorHandler
        return std::shared_ptr<reanimated::ErrorHandler>();
    };
    mrousavy::multithreading::install(*runtime, makeScheduler, makeErrorHandler);
}
