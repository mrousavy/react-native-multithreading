#include <jni.h>
#include <jsi/jsi.h>
#include "RNMultithreadingInstaller.h"

using namespace facebook;

extern "C"
JNIEXPORT void JNICALL
Java_com_reactnativemultithreading_MultithreadingModule_nativeInstallMultithreading(JNIEnv *env, jobject clazz, jlong jsiPtr) {
    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);
    return mrousavy::multithreading::install(*runtime);
}
