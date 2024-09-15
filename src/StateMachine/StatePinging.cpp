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

#include "StatePinging.h"

#include "Led.h"
#include "LedManager.h"
#include "Logging.h"
#include "ServerCommunication.h"
#include "Signals.h"

void StatePinging::Enter()
{
    LOG_DEBUG("TBSM - /e Pinging ");
    led_manager_.SetStatusLed(LedColor::kPurple);
}

void StatePinging::Exit()
{
    LOG_DEBUG("TBSM - /x Pinging ");
}

FsmTransition* StatePinging::ProcessEvent(uint16_t event)
{
    FsmTransition* transition = nullptr;
    if (TICK == event)
    {
        PingServer();
    }
    else if (FAKE == event)
    {
        transition = &fake_transition_;
    }
    else if (LOAD_HISTORY == event)
    {
        transition = &load_hist_transition_;
    }
    else
    {
        // Signal ignored
    }
    return transition;
}

void StatePinging::PingServer() const
{
    const auto ping_result = ServerCom_Ping();
    if (ping_result.has_value())
    {
        if (true == ping_result.value())
        {
            event_queue_.push(LOAD_HISTORY);
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
