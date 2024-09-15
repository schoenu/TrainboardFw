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

#include "Timer.h"

#include "Logging.h"
#include "OsWrapper.h"

Timer::Timer(uint32_t tick_period_ms) : tick_period_ms_(tick_period_ms)
{
    ASSERT(0U != tick_period_ms);
}

void Timer::Init()
{
    mutex_.Init();
}

bool Timer::IsRunning()
{
    MutexLock lock{mutex_};
    return state_ == State::kRunning;
}

bool Timer::HasExpired()
{
    MutexLock lock{mutex_};
    return state_ == State::kExpired;
}

bool Timer::StartOneShot(uint32_t duration_ms)
{
    MutexLock lock{mutex_};
    bool could_start{false};
    if (state_ == State::kStopped)
    {
        if (duration_ms > tick_period_ms_)
        {
            duration_in_ticks_ = duration_ms / tick_period_ms_;
            state_ = State::kRunning;
            could_start = true;
        }
        else if (duration_ms == 0U)
        {
            state_ = State::kExpired;
            could_start = true;
        }
        else
        {
            // Duration cannot be resolved with actual tick period
        }
    }
    else
    {
        // Nothing to do
    }
    return could_start;
}

void Timer::Tick()
{
    MutexLock lock{mutex_};
    if (state_ == State::kRunning)
    {
        ticks_++;
        if (ticks_ == duration_in_ticks_)
        {
            state_ = State::kExpired;
        }
        else
        {
            // Still running
        }
    }
    else
    {
        // Nothing to tick
    }
}

uint32_t Timer::GetElapsedMilliSeconds()
{
    MutexLock lock{mutex_};
    return ticks_ * tick_period_ms_;
}

uint32_t Timer::GetTickPeriodMilliSeconds()
{
    MutexLock lock{mutex_};
    return tick_period_ms_;
}

bool Timer::Reset()
{
    MutexLock lock{mutex_};
    bool could_reset{false};
    if (state_ == State::kExpired || state_ == State::kStopped)
    {
        ticks_ = 0U;
        state_ = State::kStopped;
        could_reset = true;
    }
    return could_reset;
}

bool Timer::Halt()
{
    MutexLock lock{mutex_};
    bool could_halt{false};
    if (state_ == State::kRunning)
    {
        state_ = State::kHalted;
        could_halt = true;
    }
    return could_halt;
}

bool Timer::Restart()
{
    MutexLock lock{mutex_};
    bool could_restart{false};
    if (state_ != State::kStopped)
    {
        state_ = State::kRunning;
        ticks_ = 0U;
        could_restart = true;
    }
    else
    {
        // Not allowed, because duration is not specified
    }
    return could_restart;
}

bool Timer::Stop()
{
    MutexLock lock{mutex_};
    bool could_stop{false};
    if (state_ != State::kExpired)
    {
        ticks_ = 0U;
        state_ = State::kStopped;
        could_stop = true;
    }
    else
    {
        // When the timer is expired, a reset or restart is possible, not a stop
    }
    return could_stop;
}

bool Timer::Continue()
{
    MutexLock lock{mutex_};
    bool could_continue{false};
    if (state_ == State::kHalted)
    {
        state_ = State::kRunning;
        could_continue = true;
    }
    else
    {
        // continuing from other state makes no sense
    }
    return could_continue;
}
