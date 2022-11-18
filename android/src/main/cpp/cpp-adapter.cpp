#include <jni.h>
#include <jsi/jsi.h>
#include <jsi/decorator.h>
#include <memory>
#include <fbjni/fbjni.h>
#include <ReactCommon/CallInvokerHolder.h>
#include <react/jni/JavaScriptExecutorHolder.h>
#include <android/log.h>
#include <string>
#include "RNMultithreadingInstaller.h"

#include "Scheduler.h"
#include "AndroidScheduler.h"
#include "AndroidErrorHandler.h"
#include "MultithreadingScheduler.h"

using namespace facebook;
using namespace reanimated;
using namespace mrousavy;
using namespace multithreading;

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

 static void installNative(jni::alias_ref<JClass> clazz,
                           jlong jsiRuntimePointer,
                           jni::alias_ref<facebook::react::CallInvokerHolder::javaobject> jsCallInvokerHolder,
                           jni::alias_ref<MultithreadingScheduler::javaobject> androidScheduler) {


  auto runtimeT = reinterpret_cast<jsi::Runtime *>(jsiRuntimePointer);
  auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
  auto scheduler = std::shared_ptr<MultithreadingScheduler>(androidScheduler->cthis());
  scheduler->setJSCallInvoker(jsCallInvoker);

  auto makeScheduler = [scheduler]() -> std::shared_ptr<MultithreadingScheduler> {
   return scheduler;
  };

  auto makeErrorHandler = [](const std::shared_ptr<MultithreadingScheduler> &scheduler_) -> std::shared_ptr<reanimated::ErrorHandler> {
   return std::make_shared<reanimated::AndroidErrorHandler>(scheduler_);
  };

  auto makeRuntime = []() -> std::unique_ptr<jsi::Runtime> {
   __android_log_write(ANDROID_LOG_INFO, TAG, "Creating JSExecutorFactory..");
   try {
    std::shared_ptr<react::ExecutorDelegate> delegate = std::shared_ptr<react::ExecutorDelegate>();
    std::shared_ptr<react::MessageQueueThread> jsQueue = std::shared_ptr<react::MessageQueueThread>();

    auto jsExecutorFactory = makeJSExecutorFactory();
    __android_log_write(ANDROID_LOG_INFO, TAG, "Creating JSExecutor..");
    auto executor = jsExecutorFactory->createJSExecutor(delegate,
                                                        jsQueue);



    auto runtimePointer = static_cast<jsi::Runtime *>(executor->getJavaScriptContext());
    __android_log_write(ANDROID_LOG_INFO, TAG, "JSExecutor created!");
    // I need to release the local shared_ptr because otherwise the returned jsi::Runtime will be destroyed immediately.
    auto _ = executor.release();

    return std::unique_ptr<jsi::Runtime>(runtimePointer);
   } catch (std::exception &exc) {
    // Fatal error - the runtime can't be created at all.
    __android_log_write(ANDROID_LOG_ERROR, TAG, "Failed to create JSExecutor!");
    __android_log_write(ANDROID_LOG_ERROR, TAG, exc.what());
    abort();
   }
  };



  mrousavy::multithreading::install(*runtimeT, makeRuntime, makeScheduler, makeErrorHandler);
 }
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
 return facebook::jni::initialize(vm, [] {
  MultithreadingScheduler::registerNatives();
  MultithreadingModule::registerNatives();
 });
}



