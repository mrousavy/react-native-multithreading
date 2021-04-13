package com.example.reactnativemultithreading;

import com.facebook.react.bridge.JSIModuleSpec;
import com.facebook.react.bridge.JavaScriptContextHolder;
import com.facebook.react.bridge.ReactApplicationContext;
import com.reactnativemultithreading.MultithreadingModule;
import com.swmansion.reanimated.ReanimatedJSIModulePackage;

import java.util.List;

public class ExampleJSIModulePackage extends ReanimatedJSIModulePackage {
    @Override
    public List<JSIModuleSpec> getJSIModules(ReactApplicationContext reactApplicationContext, JavaScriptContextHolder jsContext) {
        List<JSIModuleSpec> modules = super.getJSIModules(reactApplicationContext, jsContext);
        MultithreadingModule.install(jsContext.get());
        return modules;
    }
}