/*
 * Property Utilities
 * Copyright (c) 2016 Giemsa, MithrilWorks All Rights Reserved.
 *
 * this software is released under Boost Software License Ver 1.0
 */

#ifndef MW_XPU_APPINFO_ANDROID_H
#define MW_XPU_APPINFO_ANDROID_H

#include <jni.h>

namespace xpu
{
    class AppInfoAndroid
    {
    public:
        static void setJavaVM(JavaVM *vm);
    };
}

#endif
