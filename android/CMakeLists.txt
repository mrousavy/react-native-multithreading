cmake_minimum_required(VERSION 3.4.1)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 14)
set (PACKAGE_NAME "rnmultithreading")
set (build_DIR ${CMAKE_SOURCE_DIR}/build)

# fbjni

file (GLOB libfbjni_link_DIRS "${build_DIR}/fbjni*.aar/jni/${ANDROID_ABI}")
file (GLOB libfbjni_include_DIRS "${build_DIR}/fbjni-*-headers.jar/")

# includes

include_directories(
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native/React"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native/React/Base"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native/ReactCommon"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native/ReactCommon/callinvoker"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native/ReactCommon/jsi"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native/ReactAndroid/src/main/java/com/facebook/react/turbomodule/core/jni"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native/ReactAndroid/src/main/jni"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native-reanimated/Common/cpp/headers/Tools"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native-reanimated/Common/cpp/headers/SpecTools"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native-reanimated/Common/cpp/headers/SharedItems"
        "${RN_ANDROID_ROOT_DIR}/../node_modules/react-native-reanimated/Common/cpp/headers/Registries"
        "../cpp"
)

# rnmultithreading shared

add_library(
        ${PACKAGE_NAME}
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
        ${CMAKE_SOURCE_DIR}/ndk-deps/reanimated/jni/${ANDROID_ABI}/libreanimated.so
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
        ${CMAKE_SOURCE_DIR}/ndk-deps//jsc/jni/${ANDROID_ABI}/libjsc.so
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
        ${CMAKE_SOURCE_DIR}/ndk-deps/rn/jni/${ANDROID_ABI}/libjscexecutor.so
)

# find packages

find_library(
        log-lib
        log
)

find_library(
        FBJNI_LIBRARY fbjni
        PATHS ${libfbjni_link_DIRS}
        NO_CMAKE_FIND_ROOT_PATH
)

# build shared lib

target_include_directories(
        ${PACKAGE_NAME} PRIVATE
        ${libfbjni_include_DIRS}
)

target_link_libraries(
        ${PACKAGE_NAME}
        reanimated
        jsc
        rn
        ${FBJNI_LIBRARY}
        ${log-lib}
        android
)
