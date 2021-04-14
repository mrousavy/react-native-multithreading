#include <jni.h>
#include <jsi/jsi.h>
#include <memory>
#include <fbjni/fbjni.h>

#include "RNMultithreadingInstaller.h"

#include "Scheduler.h"
#include "ErrorHandler.h"

using namespace facebook;

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
        jlong jsiPtr) {
    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);

    auto makeScheduler = []() -> std::shared_ptr<reanimated::Scheduler> {
        return std::shared_ptr<reanimated::Scheduler>();
    };
    auto makeErrorHandler = [](const std::shared_ptr<reanimated::Scheduler>& scheduler) -> std::shared_ptr<reanimated::ErrorHandler> {
        return std::shared_ptr<reanimated::ErrorHandler>();
    };
    mrousavy::multithreading::install(*runtime, makeScheduler, makeErrorHandler);
/*

    // TODO: Implement multithreading for Android.
    //  The only problem I have with this is that I cannot really import/include the Reanimated library since that is a prebuilt .aar.
    //  That means, I cannot import it's headers, I cannot link it, and I cannot create instances of ShareableValue, AndroidErrorHandler, AndroidScheduler, ...
    runtime->global().setProperty(*runtime,
            "spawnThread",
            jsi::Function::createFromHostFunction(*runtime,
                    jsi::PropNameID::forAscii(*runtime, "spawnThread"),
                    1,
                    [](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
                auto promise = runtime.global().getProperty(runtime, "Promise").asObject(runtime);
                auto rejecter = promise.getProperty(runtime, "reject");
                return rejecter.asObject(runtime).asFunction(runtime).call(runtime, jsi::JSError(runtime, "Multithreading is not yet supported on Android.").value());
    }));*/
}
