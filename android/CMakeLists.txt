cmake_minimum_required(VERSION 3.4.1)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 14)

add_library(
        rnmultithreading
        SHARED
        src/main/cpp/cpp-adapter.cpp
        src/main/Common/ThreadPool.cpp
        src/main/Common/RNMultithreadingInstaller.cpp
        src/main/Common/MakeJSIRuntime.cpp
        #../../react-native/ReactCommon/jsi/jsi/jsi.cpp
)

# Specifies a path to native header files.
include_directories(
        ../../react-native/React
        ../../react-native/React/Base
        ../../react-native/ReactCommon
        ../../react-native/ReactCommon/jsi
        ../../react-native/ReactCommon/callinvoker
        ../../hermes-engine/android/include
        ../../react-native-reanimated/Common/cpp/headers/NativeModules
        ../../react-native-reanimated/Common/cpp/headers/Registries
        ../../react-native-reanimated/Common/cpp/headers/SharedItems
        ../../react-native-reanimated/Common/cpp/headers/SpecTools
        ../../react-native-reanimated/Common/cpp/headers/Tools
        ../../react-native-reanimated/Common/cpp/hidden_headers
        ../../react-native-reanimated/android/src/main/cpp/headers
        src/main/cpp
        src/main/Common
)

find_library(
        log-lib
        log
)

find_library(
        rea-lib
        rnreanimated
)

target_link_libraries(rnmultithreading
        ${log-lib}
        android
)
