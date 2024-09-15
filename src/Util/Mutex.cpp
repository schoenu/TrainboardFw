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

#include "Mutex.h"

#include "Logging.h"
#include "OsWrapper.h"

Mutex::~Mutex()
{
    OswMutexDelete(mutex_);
}

void Mutex::Init()
{
    mutex_ = OswMutexCreate();
}

void Mutex::Lock(uint32_t timeout)
{
    [[maybe_unused]] const auto did_get_mutex = OswMutexGet(mutex_, timeout);
    ASSERT(did_get_mutex);
}

void Mutex::Unlock()
{
    OswMutexRelease(mutex_);
}
