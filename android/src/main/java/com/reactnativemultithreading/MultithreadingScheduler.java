package com.reactnativemultithreading;

import com.facebook.jni.HybridData;
import com.facebook.proguard.annotations.DoNotStrip;
import java.util.concurrent.ExecutorService;

@SuppressWarnings("JavaJniMissingFunction")
public class MultithreadingScheduler {
    @SuppressWarnings({"unused", "FieldCanBeLocal"})
    @DoNotStrip
    private final HybridData mHybridData;
    private final ExecutorService mExecutorThread;

    public MultithreadingScheduler(ExecutorService executorThread) {
        this.mExecutorThread = executorThread;
        mHybridData = initHybrid();
    }

    private native HybridData initHybrid();
    private native void triggerUI();

    @SuppressWarnings("unused")
    @DoNotStrip
    private void scheduleTrigger() {
        mExecutorThread.submit(this::triggerUI);
    }
}

