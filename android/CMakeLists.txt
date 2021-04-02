cmake_minimum_required(VERSION 3.4.1)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 14)

add_library(rnmultithreading
            SHARED
#            ../cpp/RNMultithreadingInstaller.cpp
            cpp-adapter.cpp
            ../cpp/RNMultithreadingInstaller.cpp
            ../cpp/ThreadPool.cpp
            ../../react-native/ReactCommon/jsi/jsi/jsi.cpp
#            ../../react-native-reanimated/android/src/main/cpp/AndroidErrorHandler.cpp
#            ../../react-native-reanimated/android/src/main/cpp/AndroidScheduler.cpp
)

# Specifies a path to native header files.
include_directories(
        ../cpp
        ../../react-native/React
        ../../react-native/React/Base
        ../../react-native/ReactCommon/jsi
        ../../react-native-reanimated/android/src/main/cpp/headers
        ../../react-native-reanimated/Common/cpp/headers
        ../../react-native-reanimated/Common/cpp/hidden_headers
)

find_library(
        log-lib
        log
)

target_link_libraries(rnmultithreading
        ${log-lib}
        android
)
