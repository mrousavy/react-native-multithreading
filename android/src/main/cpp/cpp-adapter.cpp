#include <jni.h>
#include <jsi/jsi.h>
#include <memory>
#include <fbjni/fbjni.h>
#include <ReactCommon/CallInvokerHolder.h>
#include <react/jni/JavaScriptExecutorHolder.h>
#include <android/log.h>

#include "RNMultithreadingInstaller.h"

#include "Scheduler.h"
#include "AndroidErrorHandler.h"
#include "AndroidScheduler.h"

using namespace facebook;
using namespace reanimated;


struct MultithreadingModule : jni::JavaClass<MultithreadingModule> {
public:
  __unused static constexpr auto kJavaDescriptor = "Lcom/reactnativemultithreading/MultithreadingModule;";

  static constexpr auto TAG = "RNMultithreading";

  static void registerNatives() {
    javaClassStatic()->registerNatives({
                                           makeNativeMethod("installNative",
                                                            MultithreadingModule::installNative)
                                       });
  }

private:
  static std::shared_ptr<react::JSExecutorFactory> makeJSExecutorFactory() {
    __android_log_write(ANDROID_LOG_INFO, TAG, "Calling Java method MultithreadingModule.makeJSExecutor()...");
    static const auto cls = javaClassStatic();
    static const auto method = cls->getStaticMethod<react::JavaScriptExecutorHolder()>("makeJSExecutor");
    auto result = method(cls);
    __android_log_write(ANDROID_LOG_INFO, TAG, "JavaScriptExecutor created! Getting factory...");
    return result->cthis()->getExecutorFactory();
  }

  static void installNative(jni::alias_ref<JClass>,
                            jlong jsiRuntimePointer,
                            jni::alias_ref<facebook::react::CallInvokerHolder::javaobject> jsCallInvokerHolder,
                            jni::alias_ref<AndroidScheduler::javaobject> androidScheduler) {

    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiRuntimePointer);

    auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
    auto scheduler = androidScheduler->cthis()->getScheduler();
    scheduler->setJSCallInvoker(jsCallInvoker);

    auto makeScheduler = [scheduler]() -> std::shared_ptr<reanimated::Scheduler> {
      return scheduler;
    };
    auto makeErrorHandler = [](const std::shared_ptr<reanimated::Scheduler>& scheduler_) -> std::shared_ptr<reanimated::ErrorHandler> {
      return std::make_shared<reanimated::AndroidErrorHandler>(scheduler_);
    };
    auto makeJsExecutor = []() -> std::unique_ptr<jsi::Runtime> {
      __android_log_write(ANDROID_LOG_INFO, TAG, "Creating JSExecutorFactory..");
      try {
        std::shared_ptr<react::ExecutorDelegate> delegate = std::shared_ptr<react::ExecutorDelegate>();
        std::shared_ptr<react::MessageQueueThread> jsQueue = std::shared_ptr<react::MessageQueueThread>();

        auto jsExecutorFactory = makeJSExecutorFactory();
        __android_log_write(ANDROID_LOG_INFO, TAG, "Creating JSExecutor..");
        auto executor = jsExecutorFactory->createJSExecutor(delegate,
                                                            jsQueue);
        auto runtimePointer = static_cast<jsi::Runtime*>(executor->getJavaScriptContext());
        __android_log_write(ANDROID_LOG_INFO, TAG, "JSExecutor created!");

        // I need to release the local shared_ptr because otherwise the returned jsi::Runtime will be destroyed immediately.
        auto _ = executor.release();

        return std::unique_ptr<jsi::Runtime>(runtimePointer);
      } catch (std::exception& exc) {
        // Fatal error - the runtime can't be created at all.
        __android_log_write(ANDROID_LOG_ERROR, TAG, "Failed to create JSExecutor!");
        __android_log_write(ANDROID_LOG_ERROR, TAG, exc.what());
        abort();
      }
    };
    mrousavy::multithreading::install(*runtime, makeJsExecutor, makeScheduler, makeErrorHandler);

  }
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
  return facebook::jni::initialize(vm, [] {
    MultithreadingModule::registerNatives();
  });
}
/*
To create the Scheduler/AndroidErrorHandler:
1.:     #include <fbjni/fbjni.h>
2.:     class AndroidScheduler : public jni::HybridClass<AndroidScheduler>;
3.:     jni::alias_ref<AndroidScheduler::javaobject> androidScheduler
4.:     api project(":react-native-reanimated")
*/