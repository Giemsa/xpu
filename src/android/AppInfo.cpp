/*
 * Property Utilities
 * Copyright (c) 2016 Giemsa, MithrilWorks All Rights Reserved.
 *
 * this software is released under Boost Software License Ver 1.0
 */

#include "../../include/AppInfo.h"
#include "AppInfo_Android.h"
#include <dlfcn.h>
#include <jni.h>
#include <android/log.h>
#include "JNI.h"

JavaVM  *jvm_ = NULL;

namespace xpu
{
    namespace Internal
    {
        void loadFunc()
        {
            typedef jint (*FuncType)(JavaVM **, jsize, jsize *);

            void *handle = dlopen("libart.so", RTLD_LAZY);
            if(!handle)
            {
                handle = dlopen("libdvm.so", RTLD_LAZY);
            }

            if(!handle)
            {
                __android_log_print(ANDROID_LOG_ERROR, "XPU", "failed to load shared object");
                return;
            }

            FuncType func = reinterpret_cast<FuncType>(dlsym(handle, "JNI_GetCreatedJavaVMs"));
            if(!func)
            {
                __android_log_print(ANDROID_LOG_ERROR, "XPU", "failed to get JNI_GetCreatedJavaVMs");
                dlclose(handle);
                return;
            }

            jsize ct = 0;
            const jint r = (*func)(&jvm_, 1 ,&ct);
            if(r != JNI_OK)
            {
                __android_log_print(ANDROID_LOG_ERROR, "XPU", "failed to get JavaVM");
                dlclose(handle);
                return;
            }

            dlclose(handle);
        }

        std::string getVersionName()
        {
            Android::Env env(jvm_);
            jclass activityThreadClass = env->FindClass("android/app/ActivityThread");
            jmethodID idCurrentApplication = env->GetStaticMethodID(activityThreadClass, "currentApplication", "()Landroid/app/Application;");

            jobject application = env->CallStaticObjectMethod(activityThreadClass, idCurrentApplication);
            jclass applicationClass = env->GetObjectClass(application);

            jmethodID idGetPackageManager = env->GetMethodID(applicationClass, "getPackageManager", "()Landroid/content/pm/PackageManager;");
            jobject packageManager = env->CallObjectMethod(application, idGetPackageManager);
            jclass packageManagerClass = env->GetObjectClass(packageManager);

            jmethodID idGetPackageName = env->GetMethodID(applicationClass, "getPackageName", "()Ljava/lang/String;");
            jobject packageName = env->CallObjectMethod(application, idGetPackageName);

            jmethodID idGetPackageInfo = env->GetMethodID(packageManagerClass, "getPackageInfo", "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
            jobject packageInfo = env->CallObjectMethod(packageManager, idGetPackageInfo, packageName, 0);
            jclass packageInfoClass = env->GetObjectClass(packageInfo);

            jfieldID idVersionName = env->GetFieldID(packageInfoClass, "versionName", "Ljava/lang/String;");
            jstring versionName = static_cast<jstring>(env->GetObjectField(packageInfo, idVersionName));
            const char *str = env->GetStringUTFChars(versionName, NULL);
            const std::string r = str;
            env->ReleaseStringUTFChars(versionName, str);
            env->DeleteLocalRef(versionName);

            env->DeleteLocalRef(packageInfoClass);
            env->DeleteLocalRef(packageInfo);
            env->DeleteLocalRef(packageName);
            env->DeleteLocalRef(packageManagerClass);
            env->DeleteLocalRef(packageManager);
            env->DeleteLocalRef(applicationClass);
            env->DeleteLocalRef(application);
            env->DeleteLocalRef(activityThreadClass);

            return r;
        }
    }

    // AppInfoAndroid
    void AppInfoAndroid::setJavaVM(JavaVM *vm)
    {
        jvm_ = vm;
    }

    AppInfo &AppInfo::createAppInfo()
    {
        static AppInfo instance;
        if(!jvm_)
        {
            Internal::loadFunc();
        }

        return instance;
    }

    std::string AppInfo::getVersionName() const
    {
        static const std::string &versionName = Internal::getVersionName();
        return versionName;
    }
}
