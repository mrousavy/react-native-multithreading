#include "RNMultithreadingInstaller.h"
#include <memory>
#include "ThreadPool.h"
#include <exception>

#if __has_include(<RNReanimated/Scheduler.h>)
#include <RNReanimated/Scheduler.h>
#include <RNReanimated/RuntimeManager.h>
#include <RNReanimated/ShareableValue.h>
#include <RNReanimated/RuntimeDecorator.h>
#include <RNReanimated/ErrorHandler.h>
#else

#include "Scheduler.h"
#include "RuntimeManager.h"
#include "ShareableValue.h"
#include "RuntimeDecorator.h"
#include "ErrorHandler.h"

#endif

#ifdef ON_ANDROID

#include "android/asset_manager.h"
#include <fbjni/fbjni.h>
#include  "MultithreadingScheduler.h"

AAssetManager *assetManager;

static JavaVM *vm;

static jobject multithreading_object;

static jclass multithreading_class;

/**
 * A simple callback function that allows us to detach current JNI Environment
 * when the thread
 * See https://stackoverflow.com/a/30026231 for detailed explanation
 */

void DeferThreadDetach(JNIEnv *env)
{
    static pthread_key_t thread_key;

    // Set up a Thread Specific Data key, and a callback that
    // will be executed when a thread is destroyed.
    // This is only done once, across all threads, and the value
    // associated with the key for any given thread will initially
    // be NULL.
    static auto run_once = []
    {
        const auto err = pthread_key_create(&thread_key, [](void *ts_env)
        {
            if (ts_env) {
                vm->DetachCurrentThread();
            }
        });
        if (err) {
            // Failed to create TSD key. Throw an exception if you want to.
        }
        return 0;
    }();

    // For the callback to actually be executed when a thread exits
    // we need to associate a non-NULL value with the key on that thread.
    // We can use the JNIEnv* as that value.
    const auto ts_env = pthread_getspecific(thread_key);
    if (!ts_env) {
        if (pthread_setspecific(thread_key, env)) {
            // Failed to set thread-specific value for key. Throw an exception if you want to.
        }
    }
}

/**
 * Get a JNIEnv* valid for this thread, regardless of whether
 * we're on a native thread or a Java thread.
 * If the calling thread is not currently attached to the JVM
 * it will be attached, and then automatically detached when the
 * thread is destroyed.
 *
 * See https://stackoverflow.com/a/30026231 for detailed explanation
 */
JNIEnv *GetJniEnv()
{
    JNIEnv *env = nullptr;
    // We still call GetEnv first to detect if the thread already
    // is attached. This is done to avoid setting up a DetachCurrentThread
    // call on a Java thread.

    // g_vm is a global.
    auto get_env_result = vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (get_env_result == JNI_EDETACHED) {
        if (vm->AttachCurrentThread(&env, NULL) == JNI_OK) {
            DeferThreadDetach(env);
        }
        else {
            // Failed to attach thread. Throw an exception if you want to.
        }
    }
    else if (get_env_result == JNI_EVERSION) {
        // Unsupported JNI version. Throw an exception if you want to.
    }
    return env;
}

#endif

jsi::Value eval(jsi::Runtime &rt, const char *code)
{
    return rt.global().getPropertyAsFunction(rt, "eval").call(rt, code);
}

namespace mrousavy
{
namespace multithreading
{

static std::unique_ptr<reanimated::RuntimeManager> manager;

static auto getResolver(jsi::Runtime &runtime, const jsi::Value &arg)
{
    auto resolverValue = std::make_shared<jsi::Value>(
        (arg.asObject(runtime)));

    auto resolver = [&runtime, resolverValue](
        const std::shared_ptr<reanimated::ShareableValue> &shareableValue)
    {
        manager->scheduler->scheduleOnJS(
            [&runtime, resolverValue, shareableValue]()
            {
                resolverValue->asObject(
                    runtime).asFunction(
                    runtime).call(
                    runtime,
                    shareableValue->getValue(
                        runtime));
            });
    };

    return resolver;
}

static auto getRejecter(jsi::Runtime &runtime, const jsi::Value &arg)
{
    auto rejecterValue = std::make_shared<jsi::Value>(
        (arg.asObject(runtime)));
    auto rejecter = [&runtime, rejecterValue](
        const std::string &message)
    {
        manager->scheduler->scheduleOnJS(
            [&runtime, rejecterValue, message]()
            {
                rejecterValue->asObject(
                    runtime).asFunction(
                    runtime).call(
                    runtime,
                    jsi::JSError(
                        runtime,
                        message).value());
            });
    };
    return rejecter;
}

template<typename Func>
static jsi::Value registerPromiseFunc(jsi::Runtime &runtime, const std::shared_ptr<ThreadPool> &pool, int paramCount, Func &&func)
{
    auto promiseFunc = [func, pool](
        jsi::Runtime &runtime,
        const jsi::Value &thisValue,
        const jsi::Value *arguments,
        size_t count) -> jsi::Value
    {

        auto resolver = getResolver(runtime, arguments[0]);

        auto rejecter = getRejecter(runtime, arguments[1]);

        auto task = [func, resolver, rejecter]
        {
            try {
                auto res = func();
                resolver(res);

            }
            catch (const std::exception &e) {
                rejecter(e.what());
            }
        };

        pool->enqueue(task);

        return jsi::Value::undefined();
    };

    auto promiseCallback = jsi::Function::createFromHostFunction(
        runtime,
        jsi::PropNameID::forAscii(
            runtime,
            "promiseCallback"),
        paramCount,
        promiseFunc);

    auto newPromise = runtime.global().getProperty(
        runtime, "Promise");
    auto promise = newPromise
        .asObject(runtime)
        .asFunction(runtime)
        .callAsConstructor(
            runtime,
            promiseCallback);

    return promise;
}

template<typename Func>
static void setGlobalRuntimeProperty(jsi::Runtime &runtime, const char *name, int paramCount, Func &&func)
{

    __android_log_print(ANDROID_LOG_ERROR, "setGlobalRuntimeProperty", "RUNTIME");
    auto f = jsi::Function::createFromHostFunction(runtime, jsi::PropNameID::forAscii(runtime, name), paramCount, func);
    runtime.global().setProperty(runtime, name, std::move(f));
}

static void registerSetLoadModulesFromAssets(jsi::Runtime &runtime, const std::shared_ptr<std::future<void>> &setupFuture, const std::shared_ptr<ThreadPool> &pool)
{
    auto setLoadModuleFromAssets = [setupFuture, pool](
        jsi::Runtime &runtime,
        const jsi::Value &thisValue,
        const jsi::Value *arguments,
        size_t count) -> jsi::Value
    {

        if (setupFuture->valid())
            setupFuture->get(); // clears future, makes invalid

        auto moduleFile = reanimated::ShareableValue::adapt(
            runtime, arguments[0],
            manager.get());
        auto moduleName = reanimated::ShareableValue::adapt(runtime, arguments[1], manager.get());

        auto task = [moduleName, moduleFile]
        {
            auto &runtime = *manager->runtime;
            std::string _moduleName = moduleName->getValue(runtime).asString(runtime).utf8(runtime);
            auto moduleExists = ("typeof globalThis." + _moduleName + " === 'object'");
            if (eval(runtime, moduleExists.c_str()).getBool()) {
                return reanimated::ShareableValue::adapt(runtime, true, manager.get());
            }
            else {
                std::string _moduleFile = moduleFile->getValue(runtime).asString(runtime).utf8(runtime);
                AAsset *file = AAssetManager_open(assetManager, _moduleFile.c_str(), AASSET_MODE_BUFFER);
                size_t fileLength = AAsset_getLength(file);
                char *fileContent = new char[fileLength + 1];
                AAsset_read(file, fileContent, fileLength);
                fileContent[fileLength] = '\0';
                runtime.evaluateJavaScript(std::make_unique<jsi::StringBuffer>(fileContent), "");
                auto defineGlobal = ("global." + _moduleName + " = " + "globalThis." + _moduleName);
                eval(runtime, defineGlobal.c_str());
                delete[] fileContent;
                return reanimated::ShareableValue::adapt(runtime, true, manager.get());
            }
        };

        return registerPromiseFunc(runtime, pool, 2, task);
    };

    setGlobalRuntimeProperty(runtime, "loadModuleFromAssets", 2, std::move(setLoadModuleFromAssets));
}

static void registerDoWork(jsi::Runtime &runtime, const std::shared_ptr<std::future<void>> &setupFuture, const std::shared_ptr<ThreadPool> &pool)
{
    auto setDoWork = [setupFuture, pool](
        jsi::Runtime &runtime,
        const jsi::Value &thisValue,
        const jsi::Value *arguments,
        size_t count) -> jsi::Value
    {

        if (setupFuture->valid())
            setupFuture->get(); // clears future, makes invalid

        auto code = reanimated::ShareableValue::adapt(
            runtime, arguments[0],
            manager.get());

        auto task = [code]
        {
            auto &runtime = *manager->runtime;
            std::string c = code->getValue(
                runtime).asString(
                runtime).utf8(
                runtime);
            auto result = eval(
                runtime,
                ("(" +
                    c +
                    ")").c_str()).asObject(
                runtime).asFunction(
                runtime).call(
                runtime);
            return reanimated::ShareableValue::adapt(
                runtime,
                result,
                manager.get());
        };

        return registerPromiseFunc(runtime, pool, 2, task);
    };
    setGlobalRuntimeProperty(runtime, "doWork", 1, setDoWork);
}

static void registerSpawnThread(jsi::Runtime &runtime, const std::shared_ptr<std::future<void>> &setupFuture, const std::shared_ptr<ThreadPool> &pool)
{

    auto setSpawnThread = [setupFuture, pool](
        jsi::Runtime &runtime,
        const jsi::Value &thisValue,
        const jsi::Value *arguments,
        size_t count) -> jsi::Value
    {


        if (!arguments[0].isObject())
            throw jsi::JSError(
                runtime,
                "spawnThread: First argument has to be a function!");

        if (setupFuture->valid())
            setupFuture->get(); // clears future, makes invalid

        auto worklet = reanimated::ShareableValue::adapt(
            runtime,
            arguments[0],
            manager.get());

        auto task = [worklet]
        {
            auto &runtime = *manager->runtime;
            auto function = worklet->getValue(
                runtime).asObject(
                runtime).asFunction(
                runtime);
            auto result = function.getFunction(
                runtime).callWithThis(
                runtime,
                function);
            return reanimated::ShareableValue::adapt(
                runtime,
                result,
                manager.get());
        };
        return registerPromiseFunc(runtime, pool, 2, task);
    };

    setGlobalRuntimeProperty(runtime, "spawnThread", 1, setSpawnThread);
}

static void registerLoadPlugin(jsi::Runtime &runtime, const std::shared_ptr<std::future<void>> &setupFuture, const std::shared_ptr<ThreadPool> &pool)
{

    auto setLoadPlugin = [setupFuture, pool](
        jsi::Runtime &runtime,
        const jsi::Value &thisValue,
        const jsi::Value *arguments,
        size_t count) -> jsi::Value
    {
        if (setupFuture->valid())
            setupFuture->get(); // clears future, makes invalid

        auto pluginName = reanimated::ShareableValue::adapt(runtime, arguments[0], manager.get());

        pool->enqueue([pluginName]()
                      {
                          JNIEnv *jniEnv = GetJniEnv();
                          auto &nextRuntime = *manager.get()->runtime;
                          auto _pluginName = pluginName->getValue(nextRuntime).asString(nextRuntime).utf8(nextRuntime);
                          multithreading_class = jniEnv->GetObjectClass(multithreading_object);
                          jmethodID installPlugin = jniEnv->GetMethodID(multithreading_class, "installPlugin", "(Ljava/lang/String;J)Z");
                          long long runtimeAddr = reinterpret_cast<long>(&nextRuntime);
                          bool result = jniEnv->CallBooleanMethod(multithreading_object, installPlugin, jniEnv->NewStringUTF(_pluginName.c_str()), runtimeAddr);
                          eval(nextRuntime, "console.log('second runtime is working')");
                      });

        return jsi::Value(true);
    };

    setGlobalRuntimeProperty(runtime, "loadPlugin", 1, setLoadPlugin);
}

void install(jsi::Runtime &runtime,
             const std::function<std::unique_ptr<jsi::Runtime>()> &makeRuntime,
             const std::function<std::shared_ptr<mrousavy::multithreading::MultithreadingScheduler>()> &makeScheduler,
             const std::function<std::shared_ptr<reanimated::ErrorHandler>(
                 std::shared_ptr<mrousavy::multithreading::MultithreadingScheduler>)> &makeErrorHandler)
{
    auto pool = std::make_shared<ThreadPool>(1);

    // Quickly setup the runtime - this is executed in parallel so we have to join this on the JS thread if spawnThread is called before this finishes.
    auto setupFutureSingle = pool->enqueue(
        [makeScheduler, makeRuntime, makeErrorHandler]()
        {
#ifdef ON_ANDROID
            // We need to attach this Thread to JNI because the Runtime is a HybridClass.
            jni::ThreadScope::WithClassLoader(
                [makeRuntime, makeScheduler, makeErrorHandler]()
                {
                    __unused jni::ThreadScope scope;
#endif
                    auto runtime = makeRuntime();
                    reanimated::RuntimeDecorator::decorateRuntime(*runtime,
                                                                  "CUSTOM_THREAD_1");
                    auto scheduler = makeScheduler();
                    auto errorHandler = makeErrorHandler(scheduler);
                    manager = std::make_unique<reanimated::RuntimeManager>(
                        std::move(runtime),
                        errorHandler,
                        scheduler);
#ifdef ON_ANDROID
                });
#endif
        });
    auto setupFuture = std::make_shared<std::future<void>>(std::move(setupFutureSingle));

    registerLoadPlugin(runtime, setupFuture, pool);
    registerSetLoadModulesFromAssets(runtime, setupFuture, pool);
    registerDoWork(runtime, setupFuture, pool);
    registerSpawnThread(runtime, setupFuture, pool);
}

} // namespace multithreading
} // namespace mrousavy


#ifdef ON_ANDROID
extern "C"
JNIEXPORT void JNICALL
Java_com_reactnativemultithreading_MultithreadingModule_setupAssetManager(JNIEnv *env, jobject thiz, jobject asset_manager)
{
    assetManager = AAssetManager_fromJava(env, asset_manager);
    env->GetJavaVM(&vm);
    multithreading_object = env->NewGlobalRef(thiz);

    if (assetManager == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, "NDK_ASSET_MANAGER_EXAMPLE", "Could not load assetManager");
        return;
    }
    __android_log_print(ANDROID_LOG_INFO, "NDK_ASSET_MANAGER_EXAMPLE", "Successfully set asset manager in native module");
}
#endif
