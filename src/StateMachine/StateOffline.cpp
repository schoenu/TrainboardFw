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

#include "StateOffline.h"

#include "DataManager.h"
#include "LedManager.h"
#include "Logging.h"
#include "Signals.h"
#include "TimerTicker.h"
#include "WifiProvisioning.h"

StateOffline::StateOffline(LedManager& led_manager, Transitions transitions)
    : led_manager_(led_manager),
      load_hist_transition_(transitions.hist),
      refresh_transition_(transitions.refresh),
      connect_transition_(transitions.connect),
      timer_1min_(kTickPeriodMilliSeconds)
{}

void StateOffline::InitTimers()
{
    timer_1min_.Init();
    (void)TimerTicker_RegisterTimer(&timer_1min_);
}

void StateOffline::Enter()
{
    LOG_DEBUG("TBSM - /e Offline ");
    (void)timer_1min_.StartOneShot(60 * 1000);
}

void StateOffline::Exit()
{
    LOG_DEBUG("TBSM - /x Offline ");
    timer_1min_.Stop();
    timer_1min_.Reset();
}

FsmTransition* StateOffline::ProcessEvent(uint16_t event)
{
    FsmTransition* transition = nullptr;
    if (TICK == event)
    {
        if (!WifiProv_IsConnectedToWifi() && WifiProv_HasCredentials())
        {
            constexpr uint32_t kFakeConnectTimeoutInSeconds = 10;
            (void)WifiProv_Connect(kFakeConnectTimeoutInSeconds);
            // Return value ignored because the connection listener handles changes in connection status.
        }
        if (timer_1min_.HasExpired())
        {
            transition = &refresh_transition_;
        }
    }
    else if (CONNECTED == event)
    {
        led_manager_.ClearAllLeds();
        DataMgr_SetReaderMode(DataReaderMode::kLive);
        transition = &load_hist_transition_;
    }
    else if (SHORT_PUSH == event)
    {
        led_manager_.ClearAllLeds();
        transition = &connect_transition_;
    }
    else
    {
        // Signal ignored
    }
    return transition;
}
