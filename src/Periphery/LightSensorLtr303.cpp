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

#include "LightSensorLtr303.h"

#include "Logging.h"
#include "Signals.h"

#include "Adafruit_LTR329_LTR303.h"
#include "etl/utility.h"

static Adafruit_LTR303 ltr = Adafruit_LTR303();
static uint8_t brightness = kDefaultBrightness;
static uint32_t filtered_brightness = kDefaultBrightness << 8;
static bool is_sensor_available = false;

void LightSensorLtr303_Init()
{
    Wire.setPins(kLightSensorSdaPin, kLightSensorSclPin);
    uint32_t cnt = 0;
    while (cnt < 5 && !is_sensor_available)
    {
        is_sensor_available = ltr.begin();
        cnt++;
        delay(50);
    }
    if (is_sensor_available)
    {
        ltr.setGain(LTR3XX_GAIN_48);
        ltr.setIntegrationTime(LTR3XX_INTEGTIME_50);
        ltr.setMeasurementRate(LTR3XX_MEASRATE_100);
        ltr.enableInterrupt(false);
    }
    else
    {
        LOG_ERROR("Could not detect light sensor");
    }
}
void LightSensorLtr393_Dispatch(const uint16_t event)
{
    if (!is_sensor_available)
    {
        return;
    }

    if (TICK == event)
    {
        constexpr uint32_t kMaxCnt = kBrigthnessUpdateRateMilliSec / kTickPeriodMilliSeconds;
        static uint32_t cnt = 0;
        if (cnt < kMaxCnt)
        {
            cnt++;
        }
        else
        {
            cnt = 0;
            if (ltr.newDataAvailable())
            {
                uint16_t visible_and_ir_light = 0;
                uint16_t ir_light = 0;
                [[maybe_unused]] auto _ = ltr.readBothChannels(visible_and_ir_light, ir_light);
                uint32_t new_brightness = static_cast<uint32_t>(visible_and_ir_light);
                filtered_brightness = ((31 * filtered_brightness) + new_brightness) / 32;  // Filter with fixed point arithmetics
                brightness = static_cast<uint8_t>((filtered_brightness & 0x0000FFFF) >> 8);
#if 0  // debugging light sensor
                Serial.print(" => Filtered brightness : ");
                Serial.print(filtered_brightness);
                Serial.print(" => Brightness : ");
                Serial.println(brightness);
#endif
            }
        }
    }
}

uint8_t LightSensor_GetBrightness()
{
    return etl::clamp<uint8_t>(brightness, kMinimumBrightness, UINT8_MAX);
}
