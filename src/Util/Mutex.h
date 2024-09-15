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

#ifndef MUTEX_H_
#define MUTEX_H_

#include <cstdint>

class Mutex
{
  public:
    Mutex() = default;
    ~Mutex();
    void Init();  // Only call this after the kernel was initialized
    void Lock(uint32_t timeout = 0U);
    void Unlock();

  private:
    using MutexId = void*;
    MutexId mutex_{nullptr};
};

class MutexLock
{
  public:
    MutexLock(Mutex& mutex, uint32_t timeout_ms = kDefaultLockTimeoutMilliSeconds) : mutex_(mutex)
    {
        mutex_.Lock(timeout_ms);
    }
    ~MutexLock()
    {
        mutex_.Unlock();
    }

    // Prevent copying
    MutexLock(const MutexLock&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;

    // Prevent moving
    MutexLock(MutexLock&&) = delete;
    MutexLock& operator=(MutexLock&&) = delete;

  private:
    static constexpr uint32_t kDefaultLockTimeoutMilliSeconds = 10;
    Mutex& mutex_;
};

#endif  // MUTEX_H_
