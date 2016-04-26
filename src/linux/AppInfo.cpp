/*
 * Property Utilities
 * Copyright (c) 2016 Giemsa, MithrilWorks All Rights Reserved.
 *
 * this software is released under Boost Software License Ver 1.0
 */

#include "../../include/AppInfo.h"

namespace xpu
{
    AppInfo &AppInfo::createAppInfo()
    {
        static AppInfo instance;
        return instance;
    }

    std::string AppInfo::getVersionName() const
    {
        return "";
    }
}
