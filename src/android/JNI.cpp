/*
 * Property Utilities
 * Copyright (c) 2016 Giemsa, MithrilWorks All Rights Reserved.
 *
 * this software is released under Boost Software License Ver 1.0
 */

#include "JNI.h"
#include <cstdio>
#include <android/log.h>

namespace xpu
{
    namespace Android
    {
        Env::Env(JavaVM *vm)
        : attached_(false)
        , env_(NULL)
        , jvm_(vm)
        {
            if(attached_)
            {
                return;
            }

            const jint result = vm->GetEnv((void **)&env_, JNI_VERSION_1_6);
            if(result == JNI_EDETACHED)
            {
                const jint result = vm->AttachCurrentThread(&env_, NULL);
                if(result == JNI_OK)
                {
                    attached_ = true;
                }
                else
                {
                    __android_log_print(ANDROID_LOG_ERROR, "XPU", "Can't attach JavaVM to current thread.");
                    return;
                }
            }
            else if(result != JNI_OK)
            {
                __android_log_print(ANDROID_LOG_ERROR, "XPU", "failed to GetEnv. %d", result);
                return;
            }
        }

        Env::~Env()
        {
            if(attached_)
            {
                jvm_->DetachCurrentThread();
            }
        }
    }
}

