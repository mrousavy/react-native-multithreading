cmake_minimum_required(VERSION 3.4.1)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 14)

aux_source_directory(../cpp common_src)

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
        static_rnmultithreading
        STATIC
        src/main/cpp/cpp-adapter.cpp
        ${common_src}
)

# dynamic

add_library(
        rnmultithreading
        SHARED
        src/main/cpp/dummy.c
)

find_library(
        log-lib
        log
)

target_link_libraries(
        rnmultithreading
        ${log-lib}
        static_rnmultithreading
        android
)
