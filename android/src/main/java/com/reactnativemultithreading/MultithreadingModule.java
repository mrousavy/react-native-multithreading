package com.reactnativemultithreading;

import androidx.annotation.NonNull;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;

public class MultithreadingModule extends ReactContextBaseJavaModule {
  static {
    System.loadLibrary("rnmultithreading");
  }

  private static native void nativeInstallMultithreading(long jsiPtr);

  public static void install(long jsiRuntimePointer) {
    nativeInstallMultithreading(jsiRuntimePointer);
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
