/*
 * Property Utilities
 * Copyright (c) 2016 Giemsa, MithrilWorks All Rights Reserved.
 *
 * this software is released under Boost Software License Ver 1.0
 */

#ifndef MW_XPU_ANDROID_JNI_H
#define MW_XPU_ANDROID_JNI_H

#include <jni.h>

namespace xpu
{
    namespace Android
    {
        class Env
        {
        private:
            bool attached_;
            JNIEnv *env_;
            JavaVM *jvm_;

        public:
            Env(JavaVM *vw);
            ~Env();

            JNIEnv *operator->() const { return env_; }
            JNIEnv *getEnv() const { return env_; }
        };
    }
}

#endif
