// Copyright (C) 2024 Emile Decosterd
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

#ifndef TIMER_H_
#define TIMER_H_

#include "Mutex.h"

#include <cstdint>

class Timer
{
  public:
    explicit Timer(uint32_t tick_period_ms);

    // Getters
    bool IsRunning();
    bool HasExpired();
    uint32_t GetElapsedMilliSeconds();
    uint32_t GetTickPeriodMilliSeconds();

    // Commands
    void Init();
    bool StartOneShot(uint32_t duration_ms);
    bool Restart();
    bool Reset();
    bool Halt();
    bool Continue();
    bool Stop();
    void Tick();

  private:
    enum class State
    {
        kStopped,
        kRunning,
        kHalted,
        kExpired
    };
    const uint32_t tick_period_ms_;
    uint32_t duration_in_ticks_{0};
    uint32_t ticks_{0};
    State state_{State::kStopped};
    Mutex mutex_{};
};

#endif  // TIMER_H_
