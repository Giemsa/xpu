/*
 * Property Utilities
 * Copyright (c) 2016 Giemsa, MithrilWorks All Rights Reserved.
 *
 * this software is released under Boost Software License Ver 1.0
 */

#include "../../include/DeviceInfo.h"
#include <sstream>
#include <sys/sysctl.h>
#include <alloca.h>
#include <objc/message.h>
#import <Foundation/Foundation.h>
#import <CoreServices/CoreServices.h>
#include <sys/utsname.h>

namespace xpu
{
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

        void getOSVersion(int &major, int &minor, int &patch)
        {
            if([[NSProcessInfo processInfo] respondsToSelector:@selector(operatingSystemVersion)])
            {
                NSOperatingSystemVersion version = ((NSOperatingSystemVersion (*)(id, SEL))objc_msgSend_stret)([NSProcessInfo processInfo], @selector(operatingSystemVersion));
                major = static_cast<int>(version.majorVersion);
                minor = static_cast<int>(version.minorVersion);
                patch = static_cast<int>(version.patchVersion);
            }
            else
            {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
                Gestalt(gestaltSystemVersionMajor, &major);
                Gestalt(gestaltSystemVersionMinor, &minor);
                Gestalt(gestaltSystemVersionBugFix, &patch);
#pragma GCC diagnostic pop
            }
        }

        std::string getVersion()
        {
            int major;
            int minor;
            int patch;

            getOSVersion(major, minor, patch);

            std::stringstream ss;
            ss << major << "." << minor << "." << patch;
            return ss.str();
        }

        std::string getOSName()
        {
            static const char *osNames[] = {
                "",
                "Puma",
                "Jaguar",
                "Panther",
                "Tiger",
                "Leopard",
                "Snow Leopard",
                "Lion",
                "Mountain Lion",
                "Mavericks",
                "Yosemite",
                "El Capitan"
            };

            int major;
            int minor;
            int patch;

            getOSVersion(major, minor, patch);
            if(0 <= minor && minor < sizeof(osNames) / sizeof(osNames[0]))
            {
                char buf[32];
                sprintf(buf, "Mac OS X %s", osNames[minor]);
                return buf;
            }

            return "Mac OS X";
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

    DeviceInfo &DeviceInfo::createDeviceInfo()
    {
        static DeviceInfo instance;
        return instance;
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
        static const std::string &name = Internal::getOSName();
        return name;
    }

    std::string DeviceInfo::getSystemName() const
    {
        static const std::string &name = Internal::getSystemName();
        return name;
    }
}
