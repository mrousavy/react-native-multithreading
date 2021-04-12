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
        src/main/cpp/
        ../../react-native/React
        ../../react-native/React/Base
        ../../react-native/ReactCommon
        ../../react-native/ReactCommon/jsi
        ../../react-native/ReactCommon/callinvoker
        ../../hermes-engine/android/include
        ../../react-native-reanimated/android/src/main/cpp/headers
        ../../react-native-reanimated/Common/cpp/headers
        ../../react-native-reanimated/Common/cpp/hidden_headers
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
