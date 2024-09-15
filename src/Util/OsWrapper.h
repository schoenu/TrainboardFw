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
#ifndef OS_WRAPPER_H_
#define OS_WRAPPER_H_

#include <cstdint>

using TaskFunc = void (*)(void*);

constexpr uint32_t kOsMaxDelayQueuePut = 20U;
constexpr uint32_t kOsMaxDelayQueueGet = 0x7FFFFFFFU;

void OswTaskCreate(TaskFunc func, const char* const name, void* context, uint32_t stack_size, uint32_t priority = 1U);
void OswTaskDelay(uint32_t delay_ms);
uint32_t OswTaskGetHighWaterMark();

void* OswMutexCreate();
bool OswMutexGet(void* mutex, uint32_t ticks_to_wait);
void OswMutexRelease(void* mutex);
void OswMutexDelete(void* mutex);

// Never call these function from an ISR!!
void* OswQueueCreate(uint32_t length, uint32_t item_size);
void OswQueuePut(void* handle, const void* item, uint32_t timeout = kOsMaxDelayQueuePut);
bool OswQueueGet(void* handle, void* item, uint32_t timeout = kOsMaxDelayQueueGet);

#endif
