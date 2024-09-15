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

#include <Arduino.h>
#include "esp_task_wdt.h"

#include "Application.h"
#include "Logging.h"

void setup()
{
    static uint32_t restart_cnt = 0;

    Serial.begin(115200);
    delay(500);

    const auto is_app_initialized = Application_Init();
    if (!is_app_initialized)
    {
        if (restart_cnt < 3)
        {
            LOG_INFO("Could not initialized, restarting device");
            restart_cnt++;
            delay(500);  // Let the UART send the information
            esp_restart();
        }
        else
        {
            // After too many restarts, just send out a message periodically on the console
            while (true)
            {
                delay(500);
                LOG_INFO("Could not be initialized, restarted 3 times. Now just waiting for power cycle...");
                esp_task_wdt_reset();
            }
        }
    }
}

void loop()
{
    static uint32_t last_time_stamp = 0U;
    static auto event_queue = Application_GetEventQueue();
    ASSERT(event_queue != nullptr);

    if (event_queue->empty())
    {
        uint32_t actual_time_stamp = millis();
        if (actual_time_stamp > (last_time_stamp + kTickPeriodMilliSeconds))
        {
            last_time_stamp = actual_time_stamp;
            event_queue->push(TICK);
        }
        else
        {
            delay(1U);
        }
    }
    else
    {
        const auto event = event_queue->front();
        Application_Dispatch(event);
        event_queue->pop();
    }

    esp_task_wdt_reset();
}