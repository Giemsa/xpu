/*
 * Property Utilities
 * Copyright (c) 2016 Giemsa, MithrilWorks All Rights Reserved.
 *
 * this software is released under Boost Software License Ver 1.0
 */

#include "../../include/DeviceInfo.h"
#include <vector>
#include <sys/sysctl.h>
#include <alloca.h>
#import <UIKit/UIKit.h>
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
            std::size_t size;
            sysctlbyname("hw.machine", NULL, &size, NULL, 0);

            char *name = static_cast<char *>(alloca(size * sizeof(char)));

            sysctlbyname("hw.machine", name, &size, NULL, 0);
            return name;
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
        UIDevice *device = [UIDevice currentDevice];
        return device.systemVersion.UTF8String;
    }

    std::string DeviceInfo::getOSName() const
    {
        return "iOS";
    }

    std::string DeviceInfo::getSystemName() const
    {
        static const std::string &name = Internal::getSystemName();
        return name;
    }
}
