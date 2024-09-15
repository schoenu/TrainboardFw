#include "OsWrapper.h"

void OswTaskCreate(TaskFunc, const char* const, void*, uint32_t, uint32_t) {}
void OswTaskDelay(uint32_t) {}
uint32_t OswTaskGetHighWaterMark() { return 0; }

struct MockMutex
{
    uint32_t dummy;
};

void* OswMutexCreate()
{
    return static_cast<void*>(new MockMutex{0});
}
bool OswMutexGet(void*, uint32_t)
{
    return true;
}
void OswMutexRelease(void*) {}
void OswMutexDelete(void* mutex)
{
    if (nullptr != mutex)
    {
        delete mutex;
    }
}

// Never call these function from an ISR!!
void* OswQueueCreate(uint32_t, uint32_t) { return nullptr; }
void OswQueuePut(void*, const void*, uint32_t) {}
bool OswQueueGet(void*, void*, uint32_t) { return true; }
