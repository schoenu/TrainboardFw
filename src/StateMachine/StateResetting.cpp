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

#include "StateResetting.h"

#include "FwConfig.h"
#include "LedManager.h"
#include "Logging.h"
#include "Signals.h"
#include "TimerTicker.h"
#include "WifiProvisioning.h"

StateResetting::StateResetting(LedManager& led_manager, EventQueue& event_queue, FsmTransition& delay_done_transition)
    : led_manager_(led_manager),
      event_queue_(event_queue),
      tran_connect_(delay_done_transition),
      timer_(kTickPeriodMilliSeconds) {}

void StateResetting::InitTimers()
{
    timer_.Init();
    (void)TimerTicker_RegisterTimer(&timer_);
}

void StateResetting::Enter()
{
    LOG_DEBUG("TBSM - /e Resetting ");
    led_manager_.SetTestLeds();
    WifiProv_ResetCredentials();
    timer_.StartOneShot(5000);
}

void StateResetting::Exit()
{
    LOG_DEBUG("TBSM - /x Resetting ");
    timer_.Stop();
}

FsmTransition* StateResetting::ProcessEvent(uint16_t event)
{
    FsmTransition* transition = nullptr;
    if (TICK == event)
    {
        if (timer_.HasExpired())
        {
            transition = &tran_connect_;
        }
    }
    else
    {
        // No transition for other events
    }
    return transition;
}
