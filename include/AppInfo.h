/*
 * Property Utilities
 * Copyright (c) 2016 Giemsa, MithrilWorks All Rights Reserved.
 *
 * this software is released under Boost Software License Ver 1.0
 */

#ifndef MW_XPU_APPINFO_H
#define MW_XPU_APPINFO_H

#include <string>

namespace xpu
{
    class AppInfo
    {
    private:
        static AppInfo &createAppInfo();
    public:
        static AppInfo &getAppInfo()
        {
            static AppInfo &instance = createAppInfo();
            return instance;
        }

        std::string getVersionName() const;
    };
}

#ifdef ANDROID
#   include "src/android/AppInfo_Android.h"
#endif

#endif
