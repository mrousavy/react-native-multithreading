cmake_minimum_required(VERSION 3.4.1)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 14)

include_directories(
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native/React"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native/React/Base"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native/ReactCommon"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native/ReactCommon/callinvoker"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native/ReactCommon/jsi"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native-reanimated/Common/cpp/headers/Tools"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native-reanimated/Common/cpp/headers/SpecTools"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native-reanimated/Common/cpp/headers/SharedItems"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native-reanimated/Common/cpp/headers/Registries"
        "../cpp"
)

add_library(
        rnmultithreading
        SHARED
        src/main/cpp/cpp-adapter.cpp
        ../cpp/MakeJSIRuntime.cpp
        ../cpp/RNMultithreadingInstaller.cpp
        ../cpp/ThreadPool.cpp
)

# reanimated shared

add_library(
        reanimated
        SHARED
        IMPORTED
)
set_target_properties(
        reanimated
        PROPERTIES IMPORTED_LOCATION
        ${PREBUILT_DIR}/reanimated/jni/${ANDROID_ABI}/libreanimated.so
)

# jsc shared

add_library(
        jsc
        SHARED
        IMPORTED
)
set_target_properties(
        jsc
        PROPERTIES IMPORTED_LOCATION
        ${PREBUILT_DIR}/jsc/jni/${ANDROID_ABI}/libjsc.so
)

# rn shared

add_library(
        rn
        SHARED
        IMPORTED
)
set_target_properties(
        rn
        PROPERTIES IMPORTED_LOCATION
        ${PREBUILT_DIR}/rn/jni/${ANDROID_ABI}/libjscexecutor.so
)

# build shared lib

find_library(
        log-lib
        log
)

target_link_libraries(
        rnmultithreading
        reanimated
        jsc
        rn
        ${log-lib}
        android
)