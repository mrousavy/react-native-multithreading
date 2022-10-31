package com.reactnativemultithreading;

import com.facebook.react.bridge.JavaScriptContextHolder;

public abstract class MultithreadingPlugin {
    public abstract String getName();

    protected MultithreadingPlugin() {}

    public static void register( MultithreadingPlugin plugin) {
        MultithreadingModule.Plugins.put(plugin.getName(),plugin);
    }

    public abstract boolean registerWithRuntime(long runtime);
}
