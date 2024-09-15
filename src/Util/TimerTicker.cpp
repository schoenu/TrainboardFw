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

#include "TimerTicker.h"

#include "Logging.h"
#include "OsWrapper.h"
#include "Timer.h"

#include "etl/algorithm.h"
#include "etl/vector.h"

// clang-format off
constexpr uint32_t kMaxTimers                       = 10;
constexpr uint32_t kDefaultTickPeriodMilliseconds   = 10;
// clang-format on

static uint32_t _tick_period_ms{kDefaultTickPeriodMilliseconds};
static etl::vector<Timer*, kMaxTimers> _timers{};

static void TimerTickerThread(void*);
static void Tick();

bool TimerTicker_Start(uint32_t tick_period_ms)
{
    static bool _is_started{false};
    if (!_is_started)
    {
        if (0 != tick_period_ms)
        {
            _tick_period_ms = tick_period_ms;
        }
        OswTaskCreate(TimerTickerThread, "Timer Ticker Thread", nullptr, 1024);
    }
    else
    {
        LOG_ERROR("Already started TimerTicker!!!");
    }
    return !_is_started;
}

bool TimerTicker_RegisterTimer(Timer* const timer)
{
    ASSERT(nullptr != timer);
    if (timer->GetTickPeriodMilliSeconds() != _tick_period_ms)
    {
        LOG_ERROR("Timer does not have the right tick period!");
        return false;
    }
    if (etl::find(_timers.begin(), _timers.end(), timer) != _timers.end())
    {
        LOG_ERROR("Timer already registered!");
        return false;
    }
    if (_timers.full())
    {
        LOG_ERROR("No more timer slots available!");
        return false;
    }
    _timers.push_back(timer);
    return true;
}

#ifdef UNIT_TEST
void TimerTicker_Tick()
{
    Tick();
}
#endif

static void TimerTickerThread(void*)
{
    while (true)
    {
        OswTaskDelay(_tick_period_ms);
        Tick();
    }
}

static void Tick()
{
    for (auto timer : _timers)
    {
        if (nullptr != timer)
        {
            timer->Tick();
        }
        else
        {
            LOG_ERROR("Nullptr timer registered!!");
            ASSERT(false);
        }
    }
}
