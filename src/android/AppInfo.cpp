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

#define LOG_ERROR(FMT, ...) __android_log_print(ANDROID_LOG_ERROR, "XPU", FMT, ## __VA_ARGS__)

JavaVM  *jvm_ = NULL;

namespace xpu
{
    namespace Internal
    {
        // utils
        template<typename T>
        class LocalRef
        {
        private:
            T ref_;
            Android::Env &env_;
        public:
            LocalRef(T ref, Android::Env &env)
            : ref_(ref)
            , env_(env)
            { }

            ~LocalRef()
            {
                env_->DeleteLocalRef(ref_);
            }

            operator bool() const { return ref_; }
            T get() const { return ref_; }
        };

        class ExceptionClear
        {
        private:
            Android::Env &env_;
        public:
            ExceptionClear(Android::Env &env)
            : env_(env)
            { }

            ~ExceptionClear()
            {
                env_->ExceptionClear();
            }
        };
        // ----

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
                LOG_ERROR("failed to load shared object");
                return;
            }

            FuncType func = reinterpret_cast<FuncType>(dlsym(handle, "JNI_GetCreatedJavaVMs"));
            if(!func)
            {
                LOG_ERROR("failed to get JNI_GetCreatedJavaVMs");
                dlclose(handle);
                return;
            }

            jsize ct = 0;
            const jint r = (*func)(&jvm_, 1 ,&ct);
            if(r != JNI_OK)
            {
                LOG_ERROR("failed to get JavaVM");
                dlclose(handle);
                return;
            }

            dlclose(handle);
        }

        std::string getVersionName()
        {
            Android::Env env(jvm_);
            ExceptionClear ec(env);
            LocalRef<jclass> activityThreadClass(env->FindClass("android/app/ActivityThread"), env);
            if(!activityThreadClass)
            {
                LOG_ERROR("failed to find class 'android/app/ActivityThread'");
                return "";
            }

            jmethodID idCurrentApplication = env->GetStaticMethodID(activityThreadClass.get(), "currentApplication", "()Landroid/app/Application;");
            if(!idCurrentApplication)
            {
                LOG_ERROR("failed to get 'currentApplication'");
                return "";
            }

            LocalRef<jobject> application(env->CallStaticObjectMethod(activityThreadClass.get(), idCurrentApplication), env);
            if(!application)
            {
                LOG_ERROR("failed to call 'currentApplication'");
                return "";
            }

            LocalRef<jclass> applicationClass(env->GetObjectClass(application.get()), env);
            if(!applicationClass)
            {
                LOG_ERROR("failed to get class 'android/app/Application'");
                return "";
            }

            jmethodID idGetPackageManager = env->GetMethodID(applicationClass.get(), "getPackageManager", "()Landroid/content/pm/PackageManager;");
            if(!idGetPackageManager)
            {
                LOG_ERROR("failed to get 'getPackageManager'");
                return "";
            }

            LocalRef<jobject> packageManager(env->CallObjectMethod(application.get(), idGetPackageManager), env);
            if(!packageManager)
            {
                LOG_ERROR("failed to call 'getPackageManager'");
                return "";
            }

            LocalRef<jclass> packageManagerClass(env->GetObjectClass(packageManager.get()), env);
            if(!packageManagerClass)
            {
                LOG_ERROR("failed to get class 'android/content/pm/PackageManager'");
                return "";
            }

            jmethodID idGetPackageName = env->GetMethodID(applicationClass.get(), "getPackageName", "()Ljava/lang/String;");
            if(!idGetPackageName)
            {
                LOG_ERROR("failed to get 'idGetPackageName'");
                return "";
            }

            LocalRef<jobject> packageName(env->CallObjectMethod(application.get(), idGetPackageName), env);
            if(!packageName)
            {
                LOG_ERROR("failed to call 'getPackageName'");
                return "";
            }

            jmethodID idGetPackageInfo = env->GetMethodID(packageManagerClass.get(), "getPackageInfo", "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
            if(!idGetPackageInfo)
            {
                LOG_ERROR("failed to get 'getPackageInfo'");
                return "";
            }

            LocalRef<jobject> packageInfo(env->CallObjectMethod(packageManager.get(), idGetPackageInfo, packageName.get(), 0), env);
            if(!packageInfo)
            {
                LOG_ERROR("failed to call 'getPackageInfo'");
                return "";
            }

            LocalRef<jclass> packageInfoClass(env->GetObjectClass(packageInfo.get()), env);
            if(!packageInfoClass)
            {
                LOG_ERROR("failed to get class 'android/content/pm/PackageInfo'");
                return "";
            }

            jfieldID idVersionName = env->GetFieldID(packageInfoClass.get(), "versionName", "Ljava/lang/String;");
            if(!idVersionName)
            {
                LOG_ERROR("failed to get field 'versionName'");
                return "";
            }

            jstring versionName = static_cast<jstring>(env->GetObjectField(packageInfo.get(), idVersionName));
            if(!versionName)
            {
                LOG_ERROR("failed to get 'versionName'");
                return "";
            }

            const char *str = env->GetStringUTFChars(versionName, NULL);
            const std::string r = str;
            env->ReleaseStringUTFChars(versionName, str);
            env->DeleteLocalRef(versionName);

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
