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

#include "RNMultithreading.h"
#include "MultithreadingScheduler.h"

using namespace facebook;

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
                NSURL *bundleUrl = [[NSBundle mainBundle] URLForResource:[NSString stringWithUTF8String:_moduleFile.c_str()] withExtension:@"js"];
                NSString *string = [NSString stringWithContentsOfURL:bundleUrl encoding:NSUTF8StringEncoding error:nil];
                runtime.evaluateJavaScript(std::make_unique<jsi::StringBuffer>(string.UTF8String), "");
                auto defineGlobal = ("global." + _moduleName + " = " + "globalThis." + _moduleName);
                eval(runtime, defineGlobal.c_str());
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
            
            auto &nextRuntime = *manager.get()->runtime;
            auto _pluginName = pluginName->getValue(nextRuntime).asString(nextRuntime).utf8(nextRuntime);
            NSString *__pluginName = [NSString stringWithUTF8String:_pluginName.c_str()];
            NSMutableDictionary *plugins =  [RNMultithreading getPlugins];
            bool exists = [[plugins allKeys] containsObject:__pluginName];
            if (exists) {
                MultithreadingPlugin plugin = [plugins valueForKey:__pluginName];
                plugin(nextRuntime);
            }
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
        
        auto runtime = makeRuntime();
        reanimated::RuntimeDecorator::decorateRuntime(*runtime,
                                                      "CUSTOM_THREAD_1");
        auto scheduler = makeScheduler();
        auto errorHandler = makeErrorHandler(scheduler);
        manager = std::make_unique<reanimated::RuntimeManager>(
                                                               std::move(runtime),
                                                               errorHandler,
                                                               scheduler);
        
    });
    auto setupFuture = std::make_shared<std::future<void>>(std::move(setupFutureSingle));
    
    registerLoadPlugin(runtime, setupFuture, pool);
    registerSetLoadModulesFromAssets(runtime, setupFuture, pool);
    registerDoWork(runtime, setupFuture, pool);
    registerSpawnThread(runtime, setupFuture, pool);
}

} // namespace multithreading
} // namespace mrousavy

