package com.reactnativemultithreading;

import com.facebook.react.bridge.JSIModuleSpec;
import com.facebook.react.bridge.JavaScriptContextHolder;
import com.facebook.react.bridge.ReactApplicationContext;
import com.swmansion.reanimated.ReanimatedJSIModulePackage;

import java.util.List;

public class MultithreadingJSIModulePackage extends ReanimatedJSIModulePackage {
    @Override
    public List<JSIModuleSpec> getJSIModules(ReactApplicationContext reactApplicationContext, JavaScriptContextHolder jsContext) {
        try {
            return super.getJSIModules(reactApplicationContext, jsContext);
        } finally {
            MultithreadingModule.install(reactApplicationContext, jsContext);
        }
    }
}
