package com.reactnativemultithreading;

import androidx.annotation.NonNull;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;

public class MultithreadingModule extends ReactContextBaseJavaModule {
  static {
    System.loadLibrary("rnmultithreading");
  }

  private native void nativeInstallMultithreading(long jsiPtr);

  public MultithreadingModule(ReactApplicationContext context) {
    super(context);
  }

  @NonNull
  @Override
  public String getName() {
    return "RNMultithreading";
  }

  @Override
  public void initialize() {
    super.initialize();

    getReactApplicationContext().runOnJSQueueThread(new Runnable() {
      @Override
      public void run() {
        nativeInstallMultithreading(getReactApplicationContext().getJavaScriptContextHolder().get());
      }
    });

  }
}
