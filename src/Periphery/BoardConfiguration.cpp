// Trainboard.ch
// Copyright (C) 2024 Emile Décosterd
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "BoardConfiguration.h"
#include "BuildInfo.h"
#include "FwConfig.h"

#include <Arduino.h>

BoardConfig& BoardConfig::Get()
{
    static BoardConfig instance;
    return instance;
}

void BoardConfig::ReadHwVersion()
{
    ReadHwVersionAnalogPin();
    StringifyHwVersion();

    Serial.print("HW version detected: ");
    Serial.println(hw_version_string_.c_str());
}

void BoardConfig::ReadHwVersionAnalogPin()
{
    const uint32_t analog_reading = analogReadMilliVolts(kHwVersionPin);

    if ((1550 < analog_reading) && (analog_reading < 1750))
    {
        hw_version_ = HardwareVersion::kV1;
    }
    else if ((910 < analog_reading) && (analog_reading < 1110))
    {
        hw_version_ = HardwareVersion::kV1_1;
    }
    else if ((2080 < analog_reading) && (analog_reading < 2280))
    {
        hw_version_ = HardwareVersion::kV1_2;
    }
    else
    {
        hw_version_ = HardwareVersion::kUnknown;
    }
}

void BoardConfig::StringifyHwVersion()
{
    switch (hw_version_)
    {
        case HardwareVersion::kV1:
            hw_version_string_ = "v1";
            break;
        case HardwareVersion::kV1_1:
            hw_version_string_ = "v1.1";
            break;
        case HardwareVersion::kV1_2:
            hw_version_string_ = "v1.2";
            break;
        default:
            break;
    }
}
