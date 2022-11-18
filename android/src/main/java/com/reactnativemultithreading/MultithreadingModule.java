package com.reactnativemultithreading;

import android.content.res.AssetManager;
import android.util.Log;

import androidx.annotation.Keep;
import androidx.annotation.NonNull;

import com.facebook.jni.HybridData;
import com.facebook.react.bridge.NativeModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.hermes.reactexecutor.HermesExecutorFactory;
import com.facebook.proguard.annotations.DoNotStrip;
import com.facebook.react.bridge.JavaScriptContextHolder;
import com.facebook.react.bridge.JavaScriptExecutor;
import com.facebook.react.bridge.JavaScriptExecutorFactory;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.jscexecutor.JSCExecutorFactory;
import com.facebook.react.module.annotations.ReactModule;
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;
import com.facebook.react.turbomodule.core.interfaces.CallInvokerHolder;
import com.facebook.soloader.SoLoader;
import com.swmansion.reanimated.Scheduler;

import java.util.HashMap;
import java.util.Set;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

@Keep
@DoNotStrip
@ReactModule(name = MultithreadingModule.NAME)
public class MultithreadingModule extends ReactContextBaseJavaModule {
  public static HashMap<String,MultithreadingPlugin> Plugins = new HashMap<String,MultithreadingPlugin>();
  ExecutorService executorService = Executors.newSingleThreadExecutor();
  MultithreadingScheduler multithreadingScheduler;

  static {
    System.loadLibrary("reanimated");
    System.loadLibrary("rnmultithreading");
  }

  public static final String NAME =  "RNMultithreading";
  static String TAG = "RNMultithreading";
  private final ReactApplicationContext reactContext;

  // Dummy so react native adds it to the Gradle Module System
  public MultithreadingModule(ReactApplicationContext context) {
    super(context);
    this.reactContext = context;
  }

  @NonNull
  @Override
  public String getName() {
    return TAG;
  }

  private native void setupAssetManager(AssetManager assetManager);

  private static native void installNative(long jsiRuntimePointer,
                                           CallInvokerHolderImpl jsCallInvokerHolder,
                                           MultithreadingScheduler scheduler);


  @ReactMethod(isBlockingSynchronousMethod = true)
  public boolean install() {
    multithreadingScheduler = new MultithreadingScheduler(executorService);
    JavaScriptContextHolder jsContext = getReactApplicationContext().getJavaScriptContextHolder();
    CallInvokerHolderImpl holder = (CallInvokerHolderImpl) reactContext.getCatalystInstance().getJSCallInvokerHolder();
    SoLoader.init(reactContext, false); // <-- required for makeJSExecutorFactory later on
    setupAssetManager(getReactApplicationContext().getAssets());
    installNative(jsContext.get(), holder, multithreadingScheduler);
    return true;
  }

  public boolean installPlugin(String name,long runtime) {
    if (Plugins.containsKey(name)) {
      MultithreadingPlugin plugin = Plugins.get(name);
      return plugin.registerWithRuntime(runtime);
    }
    return false;

  }



  // Called from the C++ code
  @SuppressWarnings({"unused", "RedundantSuppression"})
  public static JavaScriptExecutor makeJSExecutor() {
    Log.i(TAG, "Creating JavaScriptExecutorFactory...");
    JavaScriptExecutorFactory factory = makeJSExecutorFactory();
    try {
      Log.i(TAG, "Factory created! Creating JavaScriptExecutor...");
      return factory.create();
    } catch (Exception e) {
      Log.e(TAG, "Failed to create JavaScriptExecutor!");
      e.printStackTrace();
      return null;
    }
  }

  // method from React native
  public static JavaScriptExecutorFactory makeJSExecutorFactory() {
    try {
      Log.i(TAG, "Trying to create JSC Factory...");
      SoLoader.loadLibrary("jscexecutor");
      return new JSCExecutorFactory("Multithreading", "Multithreading");
    } catch (UnsatisfiedLinkError jscE) {
      // https://github.com/facebook/hermes/issues/78 shows that
      // people who aren't trying to use Hermes are having issues.
      // https://github.com/facebook/react-native/issues/25923#issuecomment-554295179
      // includes the actual JSC error in at least one case.
      //
      // So, if "__cxa_bad_typeid" shows up in the jscE exception
      // message, then we will assume that's the failure and just
      // throw now.

      if (jscE.getMessage().contains("__cxa_bad_typeid")) {
        throw jscE;
      }

      // Otherwise use Hermes
      try {
        Log.i(TAG, "Trying to create Hermes Factory...");
        return new HermesExecutorFactory();
      } catch (UnsatisfiedLinkError hermesE) {
        // If we get here, either this is a JSC build, and of course
        // Hermes failed (since it's not in the APK), or it's a Hermes
        // build, and Hermes had a problem.

        // We suspect this is a JSC issue (it's the default), so we
        // will throw that exception, but we will print hermesE first,
        // since it could be a Hermes issue and we don't want to
        // swallow that.
        hermesE.printStackTrace();
        throw jscE;
      }
    }
  }
}
