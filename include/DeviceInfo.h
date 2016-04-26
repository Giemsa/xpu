/*
 * Property Utilities
 * Copyright (c) 2016 Giemsa, MithrilWorks All Rights Reserved.
 *
 * this software is released under Boost Software License Ver 1.0
 */

#ifndef MW_XPU_DEVICEINFO_H
#define MW_XPU_DEVICEINFO_H

#include <string>

namespace xpu
{
    class DeviceInfo
    {
    private:
        static DeviceInfo &createDeviceInfo();
    public:
        static DeviceInfo &getDeviceInfo()
        {
            static DeviceInfo &instance = createDeviceInfo();
            return instance;
        }

        std::string getDeviceName() const;
        std::string getVersion() const;
        std::string getOSName() const;
        std::string getSystemName() const;
    };
}

#endif
