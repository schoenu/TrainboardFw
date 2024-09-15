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

#include "StateLive.h"

#include "DataManager.h"
#include "LedManager.h"
#include "Logging.h"
#include "Signals.h"
#include "TimerTicker.h"
#include "WifiProvisioning.h"

StateLive::StateLive(LedManager& led_manager, Transitions transitions)
    : led_manager_(led_manager),
      load_hist_transition_(transitions.hist),
      fake_transition_(transitions.fake),
      poll_transition_(transitions.poll),
      update_transition_(transitions.update),
      timer_1min_(kTickPeriodMilliSeconds),
      timer_15min_(kTickPeriodMilliSeconds)
{}

void StateLive::InitTimers()
{
    timer_1min_.Init();
    timer_15min_.Init();
    (void)TimerTicker_RegisterTimer(&timer_1min_);
    (void)TimerTicker_RegisterTimer(&timer_15min_);
}

void StateLive::Enter()
{
    LOG_DEBUG("TBSM - /e Live ");
    (void)timer_1min_.StartOneShot(60 * 1000);
    if (!timer_15min_.IsRunning())
    {
        (void)timer_15min_.StartOneShot(15 * 60 * 1000);
    }
}

void StateLive::Exit()
{
    LOG_DEBUG("TBSM - /x Live ");
    timer_1min_.Stop();
    timer_1min_.Reset();
}

FsmTransition* StateLive::ProcessEvent(uint16_t event)
{
    FsmTransition* transition = nullptr;
    if (TICK == event)
    {
        if (timer_1min_.HasExpired())
        {
            transition = &poll_transition_;
        }
        else if (timer_15min_.HasExpired())
        {
            timer_15min_.Reset();
            transition = &update_transition_;
        }
        else
        {
            // Nothing to do
        }
    }
    else if ((DISCONNECTED == event) || (NETWORK_DOWN == event))
    {
        led_manager_.ClearAllLeds();
        transition = &fake_transition_;
    }
    else if (SHORT_PUSH == event)
    {
        led_manager_.ClearAllLeds();
        DataMgr_SetReaderMode(DataReaderMode::kHistory);
        transition = &load_hist_transition_;
    }
    else
    {
        // Signal ignored
    }
    return transition;
}
