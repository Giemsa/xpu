/*
 * Property Utilities
 * Copyright (c) 2016 Giemsa, MithrilWorks All Rights Reserved.
 *
 * this software is released under Boost Software License Ver 1.0
 */

#include "../../include/DeviceInfo.h"
#include <sys/system_properties.h>
#include <sys/utsname.h>

namespace xpu
{
    DeviceInfo &DeviceInfo::createDeviceInfo()
    {
        static DeviceInfo instance;
        return instance;
    }

    namespace Internal
    {
        std::string getDeviceName()
        {
            char model[PROP_VALUE_MAX + 1];
            const int len = __system_property_get("ro.product.model", model);
            if(len > 0)
            {
                return model;
            }

            return "";
        }

        std::string getVersion()
        {
            char version[PROP_VALUE_MAX + 1];
            const int len = __system_property_get("ro.build.version.release", version);
            if(len > 0)
            {
                return version;
            }

            return "";
        }

        std::string getSystemName()
        {
            utsname r;
            if(uname(&r) != 0)
            {
                return "Unknown";
            }

            return r.sysname;
        }
    }

    std::string DeviceInfo::getDeviceName() const
    {
        static const std::string &name = Internal::getDeviceName();
        return name;
    }

    std::string DeviceInfo::getVersion() const
    {
        static const std::string &version = Internal::getVersion();
        return version;
    }

    std::string DeviceInfo::getOSName() const
    {
        return "Android";
    }

    std::string DeviceInfo::getSystemName() const
    {
        static const std::string &name = Internal::getSystemName();
        return name;
    }
}
