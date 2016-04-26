/*
 * Property Utilities
 * Copyright (c) 2016 Giemsa, MithrilWorks All Rights Reserved.
 *
 * this software is released under Boost Software License Ver 1.0
 */

#include "../../include/DeviceInfo.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/utsname.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

namespace xpu
{
    namespace Internal
    {
        bool exec(std::string &ret, const char *cmd)
        {
            static const int BUF_SIZE = 256;
            ret.clear();

            FILE *fp = ::popen(cmd, "r");
            if(fp == NULL)
            {
                return false;
            }

            char buf[BUF_SIZE];
            while(fgets(buf, BUF_SIZE, fp) != NULL)
            {
                ret.append(buf);
            }

            ret = ret.substr(0, ret.find_last_not_of(" \t\r\n") + 1);

            return ::pclose(fp) == 0;
        }

        bool readProperty(std::string &ret, const char *key, std::ifstream &ifs)
        {
            std::string s;
            while(std::getline(ifs, s))
            {
                if(s.find(key) != std::string::npos)
                {
                    const std::string::const_iterator &it = s.begin() + strlen(key);
                    const size_t n = *it == '"' ? 1 : 0;
                    ret.assign<std::string::const_iterator>(it + n, s.end() - n);
                    return true;
                }
            }

            return false;
        }

        bool getValue(std::string &ret, const char *key, const char *file)
        {
            std::ifstream ifs(file);
            if(ifs)
            {
                return readProperty(ret, key, ifs);
            }

            return false;
        }

        bool readFirstLine(std::string &ret, const char *file)
        {
            std::ifstream ifs(file);
            if(ifs)
            {
                if(std::getline(ifs, ret) && !ret.empty())
                {
                    return true;
                }
            }

            return false;
        }

        std::string getVersion()
        {
            utsname r;
            if(uname(&r) != 0)
            {
                return "";
            }

            return r.release;
        }

        std::string getComputerName()
        {
            utsname r;
            if(uname(&r) != 0)
            {
                return "";
            }

            return r.nodename;
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

        std::string getOSName()
        {
            std::string s;
            if(exec(s, "/usr/bin/lsb_release -ds 2> /dev/null"))
            {
                return s;
            }

            {
                std::ifstream ifs("/etc/lsb-release");
                if(ifs)
                {
                    if(readProperty(s, "DISTRIB_DESCRIPTION=", ifs))
                    {
                        return s;
                    }

                    ifs.clear();
                    ifs.seekg(0, std::ios::beg);

                    if(readProperty(s, "DISTRIB_ID=", ifs))
                    {
                        return s;
                    }
                }
            }

            if(getValue(s, "PRETTY_NAME=", "/etc/os-release"))
            {
                return s;
            }

            if(getValue(s, "PRETTY_NAME=", "/usr/lib/os-release"))
            {
                return s;
            }

            if(readFirstLine(s, "/etc/redhat-release"))
            {
                return s;
            }

            if(readFirstLine(s, "/etc/SuSE-release"))
            {
                return s;
            }

            if(exec(s, "uname -o 2> /dev/null"))
            {
                return s;
            }

            return getSystemName();
        }

    }

    DeviceInfo &DeviceInfo::createDeviceInfo()
    {
        static DeviceInfo instance;
        return instance;
    }

    std::string DeviceInfo::getDeviceName() const
    {
        static const std::string &device = Internal::getComputerName();
        return device;
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
