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

#include "StateConnecting.h"

#include "Led.h"
#include "LedManager.h"
#include "Logging.h"
#include "Signals.h"
#include "WifiProvisioning.h"

void StateConnecting::Enter()
{
    LOG_DEBUG("TBSM - /e Connecting ");
    led_manager_.SetStatusLed(LedColor::kBlue);
}

void StateConnecting::Exit()
{
    LOG_DEBUG("TBSM - /x Connecting ");
}

FsmTransition* StateConnecting::ProcessEvent(uint16_t event)
{
    FsmTransition* transition = nullptr;
    if (TICK == event)
    {
        ConnectToWifi();
    }
    else if (SHORT_PUSH == event)
    {
        transition = &fake_transition_;
    }
    else if (PING == event)
    {
        transition = &ping_transition_;
    }
    else
    {
        // Signal ignored
    }
    return transition;
}

void StateConnecting::ConnectToWifi() const
{
    const auto connect_result = WifiProv_Connect(60 * 5);
    if (connect_result.has_value())
    {
        const auto is_connected = connect_result.value();
        if (is_connected)
        {
            event_queue_.push(PING);
        }
        else
        {
            event_queue_.push(FAKE);
        }
    }
    else
    {
        // Try again next tick
    }
}
