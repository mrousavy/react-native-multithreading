#include <jni.h>
#include <jsi/jsi.h>

using namespace facebook;

extern "C"
JNIEXPORT void JNICALL
Java_com_reactnativemultithreading_MultithreadingModule_nativeInstallMultithreading(JNIEnv *env, jobject clazz, jlong jsiPtr) {
    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);

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
    }));
}
