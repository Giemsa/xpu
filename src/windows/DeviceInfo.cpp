/*
 * Property Utilities
 * Copyright (c) 2016 Giemsa, MithrilWorks All Rights Reserved.
 *
 * this software is released under Boost Software License Ver 1.0
 */

#include "../../include/DeviceInfo.h"
#include <windows.h>
#include <string>
#include <sstream>

namespace xpu
{
    namespace Internal
    {
        std::string getProductName(const OSVERSIONINFOEXW &osvi)
        {
            DWORD type = 0;
            if(!::GetProductInfo(osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.wServicePackMajor, osvi.wServicePackMinor, &type))
            {
                return "";
            }

            switch(type)
            {
            case PRODUCT_PROFESSIONAL:   return "Professional";
            case PRODUCT_ULTIMATE:       return "Ultimate";
            case PRODUCT_HOME_BASIC:     return "Home Basic";
            case PRODUCT_HOME_PREMIUM:   return "Home Premium";
            case PRODUCT_ENTERPRISE:     return "Enterprise";
            case PRODUCT_BUSINESS:       return "Basiness";
            case PRODUCT_STARTER:        return "Startar";
            case PRODUCT_WEB_SERVER:     return "Web Server";
            case PRODUCT_CLUSTER_SERVER: return "Cluster Server";
            case PRODUCT_HOME_SERVER:    return "Home Server";
            case PRODUCT_UNLICENSED:     return "Unlicensed";
            default:
                {
                    std::stringstream ss;
                    ss << "Unknown Type(" << type << ")";
                    return ss.str();
                }
            }
        }

        bool getOSVersion(OSVERSIONINFOEXW &osvi)
        {
            HMODULE module = LoadLibrary(TEXT("ntdll.dll"));
            if(!module)
            {
                return false;
            }

            typedef void (WINAPI *RtlGetVersionType)(OSVERSIONINFOEXW *);

            RtlGetVersionType func = reinterpret_cast<RtlGetVersionType>(GetProcAddress(module, "RtlGetVersion"));
            if(!func)
            {
                FreeLibrary(module);
                return false;
            }

            ZeroMemory(&osvi, sizeof(osvi));
            osvi.dwOSVersionInfoSize = sizeof(osvi);
            func(&osvi);

            FreeLibrary(module);
            return true;
        }

        std::string getOSName()
        {
            OSVERSIONINFOEXW osvi;
            if(!getOSVersion(osvi))
            {
                return "";
            }

            std::string verName = "Windows ";
            SYSTEM_INFO sysInfo;

            ::GetSystemInfo(&sysInfo);

            switch(osvi.dwMajorVersion)
            {
            case 10:
                if(osvi.dwMinorVersion >= 0)
                {
                    verName += "10";
                    if(osvi.wProductType != VER_NT_WORKSTATION)
                    {
                        verName += " Server";
                    }
                }

                verName += getProductName(osvi);
                break;
            case 6:
                if(osvi.wProductType == VER_NT_WORKSTATION)
                {
                    switch(osvi.dwMinorVersion)
                    {
                    case 0:
                        verName += "Vista ";
                        break;
                    case 1:
                        verName += "7 ";
                        break;
                    case 2:
                        verName += "8 ";
                        break;
                    case 3:
                        verName += "8.1 ";
                        break;
                    }
                }
                else
                {
                    verName += "Server ";
                    switch(osvi.dwMinorVersion)
                    {
                    case 0:
                        verName += "2008 ";
                        break;
                    case 1:
                        verName += "2008 R2 ";
                        break;
                    case 2:
                        verName += "2012 ";
                        break;
                    case 3:
                        verName += "2012 R2 ";
                        break;
                    }
                }

                verName += getProductName(osvi);
                break;
            case 5:
                switch(osvi.dwMinorVersion)
                {
                case 0:
                    verName += "2000";
                    if((osvi.wSuiteMask & VER_SUITE_DATACENTER) == VER_SUITE_DATACENTER)
                    {
                        verName += " Datacenter Server";
                    }
                    else if((osvi.wSuiteMask & VER_SUITE_ENTERPRISE) == VER_SUITE_ENTERPRISE)
                    {
                        verName += " Advanced Server";
                    }
                    else
                    {
                        verName += " Server";
                    }
                    break;
                case 1:
                    verName += "XP";
                    if((osvi.wSuiteMask & VER_SUITE_PERSONAL) == VER_SUITE_PERSONAL)
                    {
                        verName += " Home Edition";
                    }
                    else
                    {
                        verName += " Professional";
                    }
                    break;
                case 2:
                    if(sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
                    {
                        verName += "XP x64";
                    }
                    else
                    {
                        verName += "Server 2003";

                        if((osvi.wSuiteMask & VER_SUITE_DATACENTER) == VER_SUITE_DATACENTER)
                        {
                            verName += " Datacenter Edition";
                        }
                        else if((osvi.wSuiteMask & VER_SUITE_ENTERPRISE) == VER_SUITE_ENTERPRISE)
                        {
                            verName += " Enterprise Edition";
                        }
                        else if(osvi.wSuiteMask == VER_SUITE_BLADE)
                        {
                            verName += " Web Edition";
                        }
                        else
                        {
                            verName += " Standard Edition";
                        }
                    }
                    break;
                }
                break;
            default:
                verName = "unknown";
                break;
            }

            if(osvi.wServicePackMajor != 0)
            {
                char buf[24] = {};
                sprintf_s(buf, sizeof(buf), " Service Pack %hd", osvi.wServicePackMajor);
                verName += buf;
            }

            return verName;
        }

        std::string getVersion()
        {
            OSVERSIONINFOEXW osvi;
            if(!getOSVersion(osvi))
            {
                return "0.0.0";
            }

            std::stringstream ss;
            ss << osvi.dwMajorVersion << "." << osvi.dwMinorVersion << "." << osvi.dwBuildNumber;
            return ss.str();
        }

        std::string getComputerName()
        {
            char buf[MAX_COMPUTERNAME_LENGTH + 1];
            DWORD len;
            if(!::GetComputerNameA(buf, &len))
            {
                return "";
            }
            else
            {
                return buf;
            }
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
        return "Microsoft Windows";
    }
}
