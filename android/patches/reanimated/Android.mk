
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE:= reanimated
LOCAL_SRC_FILES := jni/$(TARGET_ARCH_ABI)/libreanimated.so
include $(PREBUILT_SHARED_LIBRARY)
