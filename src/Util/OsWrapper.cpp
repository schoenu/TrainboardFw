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

#include "OsWrapper.h"

#include "Logging.h"
#include "semphr.h"

// OS wrapper port for ESP32 based on RTOS

void OswTaskCreate(TaskFunc func, const char* const name, void* context, uint32_t stack_size, uint32_t priority)
{
    // Stack size, see https://www.freertos.org/a00125.html

    BaseType_t result = xTaskCreate(func, name, stack_size, context, priority, nullptr);

    if (result != pdPASS)
    {
        LOG_ERROR("Could not create task: ");
    }
    else
    {
        LOG_INFO("Task successfullly created: ");
    }
    LOG_INFO(name);

    ASSERT(pdPASS == result);  // No point continuing if we could not create the task...
}

void OswTaskDelay(uint32_t delay_ms)
{
    const uint32_t ticks = delay_ms / portTICK_PERIOD_MS;
    vTaskDelay(ticks);
}

uint32_t OswTaskGetHighWaterMark() { return uxTaskGetStackHighWaterMark(NULL); }

void* OswMutexCreate()
{
    auto mutex = xSemaphoreCreateMutex();
    ASSERT(nullptr != mutex);
    return static_cast<void*>(mutex);
}
bool OswMutexGet(void* mutex, uint32_t ticks_to_wait)
{
    ASSERT(nullptr != mutex);
    auto result = xSemaphoreTake(static_cast<QueueHandle_t>(mutex), static_cast<TickType_t>(ticks_to_wait));
    return (pdTRUE == result);
}
void OswMutexRelease(void* mutex)
{
    ASSERT(nullptr != mutex);
    auto result = xSemaphoreGive(mutex);
    ASSERT(pdTRUE == result);  // If it fails, we did not get it first. This would be  a programmer error to catch during development.
}
void OswMutexDelete(void* mutex)
{
    if (nullptr != mutex)
    {
        vSemaphoreDelete(mutex);
    }
}

void* OswQueueCreate(uint32_t length, uint32_t item_size)
{
    auto handle = xQueueCreate(length, item_size);
    ASSERT(nullptr != handle);  // No point continuing if we could not create the message queue...

    return static_cast<void*>(handle);
}
void OswQueuePut(void* handle, const void* item, uint32_t timeout)
{
    ASSERT(nullptr != handle);

    auto result = xQueueSend(static_cast<QueueHandle_t>(handle), item, timeout);
    ASSERT(pdPASS == result);
}
bool OswQueueGet(void* handle, void* item, uint32_t timeout)
{
    ASSERT(nullptr != handle);
    auto result = xQueueReceive(static_cast<QueueHandle_t>(handle), item, timeout);
    return (pdPASS == result);
}