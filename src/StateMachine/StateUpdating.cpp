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

#include "StateUpdating.h"

#include "Logging.h"
#include "ServerCommunication.h"
#include "Signals.h"
#include "WifiProvisioning.h"

void StateUpdating::Enter()
{
    LOG_DEBUG("TBSM - /e Updating ");
}

void StateUpdating::Exit()
{
    LOG_DEBUG("TBSM - /x Updating ");
}

FsmTransition* StateUpdating::ProcessEvent(uint16_t event)
{
    FsmTransition* transition = nullptr;
    if (TICK == event)
    {
        HandleTickEvent();
    }
    else if (NO_UPDATE == event)
    {
        transition = &no_update_transition_;
    }
    else
    {
        // Ignore other events
    }
    return transition;
}

void StateUpdating::HandleTickEvent()
{
    const auto is_connected_to_wifi = WifiProv_IsConnectedToWifi();
    if (!is_connected_to_wifi)
    {
        event_queue_.push(NO_UPDATE);
    }
    else
    {
        UpdateOta();
    }
}

void StateUpdating::UpdateOta()
{
    const auto could_update = ServerCom_UpdateOta();
    if (!could_update)
    {
        event_queue_.push(NO_UPDATE);
    }
    else
    {
        // Nothing to do
    }
}
