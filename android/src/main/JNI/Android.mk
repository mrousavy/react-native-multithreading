LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := rnmultithreading

PROJECT_FILES := $(wildcard $(LOCAL_PATH)/../cpp/*.cpp)
PROJECT_FILES += $(wildcard $(LOCAL_PATH)/../Common/*.cpp)

PROJECT_FILES := $(PROJECT_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(PROJECT_FILES)

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../Common
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../node_modules/react-native-reanimated/Common/cpp/headers/NativeModules
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../node_modules/react-native-reanimated/Common/cpp/headers/Registries
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../node_modules/react-native-reanimated/Common/cpp/headers/SharedItems
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../node_modules/react-native-reanimated/Common/cpp/headers/SpecTools
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../node_modules/react-native-reanimated/Common/cpp/headers/Tools

LOCAL_CFLAGS += -DONANDROID -fexceptions -frtti

LOCAL_STATIC_LIBRARIES := libjsi callinvokerholder
LOCAL_SHARED_LIBRARIES := libfolly_json libfbjni libreactnativejni

include $(BUILD_SHARED_LIBRARY)

# start | build empty library which is needed by CallInvokerHolderImpl.java
include $(CLEAR_VARS)

# Don't strip debug builds
ifeq ($(NATIVE_DEBUG), true)
    cmd-strip :=
endif

ifeq ($(shell test $(REACT_NATIVE_TARGET_VERSION) -lt 64; echo $$?),0)
	LOCAL_MODULE := turbomodulejsijni
	include $(BUILD_SHARED_LIBRARY)
endif
# end

include $(LOCAL_PATH)/react/Android.mk
