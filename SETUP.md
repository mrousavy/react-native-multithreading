# Setup Guide

## iOS

Run:

```sh
cd ios
pod install
```

## Android

Since pure JSI Modules cannot be autolinked yet, you have to manually initialize them.

### Without react-native-mmkv (or other JSI libs)

1. Open your app's `MainApplication.java`
2. Add the following code:
  ```diff
  package com.example.reactnativemultithreading;

  import android.app.Application;
  import android.content.Context;
  import com.facebook.react.PackageList;
  import com.facebook.react.ReactApplication;
  import com.facebook.react.ReactNativeHost;
  import com.facebook.react.ReactPackage;
  import com.facebook.react.ReactInstanceManager;
  import com.facebook.soloader.SoLoader;
  import java.lang.reflect.InvocationTargetException;
  import java.util.List;

  +import com.reactnativemultithreading.MultithreadingJSIModulePackage;
  +import com.facebook.react.bridge.JSIModulePackage;

  public class MainApplication extends Application implements ReactApplication {

    private final ReactNativeHost mReactNativeHost =
        new ReactNativeHost(this) {
          @Override
          public boolean getUseDeveloperSupport() {
            return BuildConfig.DEBUG;
          }

          @Override
          protected List<ReactPackage> getPackages() {
            @SuppressWarnings("UnnecessaryLocalVariable")
            List<ReactPackage> packages = new PackageList(this).getPackages();
            // Packages that cannot be autolinked yet can be added manually here, for MultithreadingExample:
            // packages.add(new MyReactNativePackage());
            packages.add(new MultithreadingPackage());
            return packages;
          }

          @Override
          protected String getJSMainModuleName() {
            return "index";
          }

  +       // TODO: Remove this when JSI Modules can be autoinstalled (maybe RN 0.65)
  +       @Override
  +       protected JSIModulePackage getJSIModulePackage() {
  +         return new MultithreadingJSIModulePackage();
  +       }
        };
  ```

### With react-native-mmkv (or other JSI libs)

1. Open your project in Android Studio
2. Open the folder where `MainApplication.java` lives (`src/main/java/...`)
3. Right click the folder, **New** > **Java class**
4. Call it whatever you prefer, in my case it's `ExampleJSIPackage` because my app is called "Example"
5. Add the following code:

  ```java
  package com.example;

  import com.facebook.react.bridge.JSIModuleSpec;
  import com.facebook.react.bridge.JavaScriptContextHolder;
  import com.facebook.react.bridge.ReactApplicationContext;
  import com.swmansion.reanimated.ReanimatedJSIModulePackage;
  import com.reactnativemmkv.MultithreadingModule;

  import java.util.Collections;
  import java.util.List;

  // TODO: Remove all of this when JSI Modules can be autoinstalled (maybe RN 0.65)
  public class ExampleJSIPackage extends ReanimatedJSIModulePackage {
      @Override
      public List<JSIModuleSpec> getJSIModules(ReactApplicationContext reactApplicationContext, JavaScriptContextHolder jsContext) {
          super.getJSIModules(reactApplicationContext, jsContext);
          MultithreadingModule.install(reactApplicationContext, jsContext);
          return Collections.emptyList();
      }
  }
  ```
6. Replace `com.example` (first line) with the correct package name
7. Replace `ExampleJSIPackage` with the file name you chose in step 4.
8. Open `MainApplication.java` and find the location where the `ReactNativeHost` is initialized. You have to override it's `getJSIModulePackage` method:
  ```diff
  package com.example.reactnativemultithreading;

  import android.app.Application;
  import android.content.Context;
  import com.facebook.react.PackageList;
  import com.facebook.react.ReactApplication;
  import com.facebook.react.ReactNativeHost;
  import com.facebook.react.ReactPackage;
  import com.facebook.react.ReactInstanceManager;
  import com.facebook.soloader.SoLoader;
  import java.lang.reflect.InvocationTargetException;
  import java.util.List;

  +import com.example.ExampleJSIPackage;
  +import com.facebook.react.bridge.JSIModulePackage;

  public class MainApplication extends Application implements ReactApplication {

    private final ReactNativeHost mReactNativeHost =
        new ReactNativeHost(this) {
          @Override
          public boolean getUseDeveloperSupport() {
            return BuildConfig.DEBUG;
          }

          @Override
          protected List<ReactPackage> getPackages() {
            @SuppressWarnings("UnnecessaryLocalVariable")
            List<ReactPackage> packages = new PackageList(this).getPackages();
            // Packages that cannot be autolinked yet can be added manually here, for MultithreadingExample:
            // packages.add(new MyReactNativePackage());
            packages.add(new MultithreadingPackage());
            return packages;
          }

          @Override
          protected String getJSMainModuleName() {
            return "index";
          }

  +       // TODO: Remove this when JSI Modules can be autoinstalled (maybe RN 0.65)
  +       @Override
  +       protected JSIModulePackage getJSIModulePackage() {
  +         return new ExampleJSIModulePackage();
  +       }
        };
  ```
