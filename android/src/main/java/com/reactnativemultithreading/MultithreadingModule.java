package com.reactnativemultithreading;

import androidx.annotation.NonNull;

import com.facebook.react.bridge.JavaScriptContextHolder;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;
import com.facebook.react.turbomodule.core.interfaces.CallInvokerHolder;
import com.swmansion.reanimated.Scheduler;

public class MultithreadingModule extends ReactContextBaseJavaModule {
  static {
    System.loadLibrary("reanimated");
    System.loadLibrary("rnmultithreading");
  }

  private static native void installNative(long jsiRuntimePointer,
                                           CallInvokerHolderImpl jsCallInvokerHolder,
                                           Scheduler scheduler);

  public static void install(ReactApplicationContext context, JavaScriptContextHolder jsContext) {
    CallInvokerHolderImpl holder = (CallInvokerHolderImpl) context.getCatalystInstance().getJSCallInvokerHolder();
    installNative(jsContext.get(), holder, new Scheduler(context));
  }

  // Dummy so react native adds it to the Gradle Module System
  public MultithreadingModule(ReactApplicationContext context) {
    super(context);
  }

  @NonNull
  @Override
  public String getName() {
    return "RNMultithreading";
  }
}
