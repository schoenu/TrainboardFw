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

#include "StatePolling.h"

#include "DataConverter.h"
#include "DataManager.h"
#include "FwConfig.h"
#include "LedManager.h"
#include "Logging.h"
#include "ServerCommunication.h"
#include "Signals.h"
#include "WifiProvisioning.h"

void StatePolling::Enter()
{
    LOG_DEBUG("TBSM - /e Polling ");
    fail_cnt_ = 0U;
    AssignDelegates();
}

void StatePolling::Exit()
{
    LOG_DEBUG("TBSM - /x Polling ");
}

FsmTransition* StatePolling::ProcessEvent(uint16_t event)
{
    FsmTransition* transition = nullptr;
    if (TICK == event)
    {
        HandleTickEvent();
    }
    else if (FAKE == event)
    {
        transition = &fake_transition_;
    }
    else if (DATA_OK == event)
    {
        transition = &data_ok_transition_;
    }
    else if ((DISCONNECTED == event) || (NETWORK_DOWN == event))
    {
        transition = &fake_transition_;
    }
    else
    {
        // Event not supported
    }
    return transition;
}

void StatePolling::AssignDelegates()
{
    const auto mode = DataMgr_GetWriterMode();
    switch (mode)
    {
        case DataWriterMode::kSingle:
            get_data_delegate_ = ServerCom_GetData;
            is_data_valid_delegate_ = DataConv_IsDataValid;
            break;
        case DataWriterMode::kMultiple:
            get_data_delegate_ = ServerCom_GetHistoryData;
            is_data_valid_delegate_ = DataConv_IsHistoryDataValid;
            break;
    }
    ASSERT(nullptr != get_data_delegate_);
    ASSERT(nullptr != is_data_valid_delegate_);
}

void StatePolling::HandleTickEvent()
{
    LOG_DEBUG("TBSM(Polling) - Poll server...");
    const auto server_response = get_data_delegate_(buffer_.data(), static_cast<uint32_t>(buffer_.size()));
    if (server_response.has_value())
    {
        LOG_DEBUG("TBSM(Polling) - Got server response");
        const auto data_length = server_response.value();
        EvaluateData(data_length, buffer_.data());
    }
    else
    {
        LOG_DEBUG("TBSM(Polling) - Could not get server response");
    }
}

void StatePolling::EvaluateData(const uint32_t data_length, const uint8_t* const buffer_data)
{
    LOG_DEBUG("TBSM(Polling) - Evaluating data");
    if (0U == data_length)
    {
        LOG_DEBUG("TBSM(Polling) - Zero length data");
        HandlePollFail();
    }
    else
    {
        const auto is_data_valid = is_data_valid_delegate_(buffer_data, data_length);
        if (!is_data_valid)
        {
            LOG_DEBUG("TBSM(Polling) - Invalid data");
            HandlePollFail();
        }
        else
        {
            LOG_DEBUG("TBSM(Polling) - Saving data");
            auto writer = DataMgr_GetWriter();
            ASSERT(nullptr != writer);
            writer->SaveData(buffer_data, data_length);
            event_queue_.push(DATA_OK);
        }
    }
}

void StatePolling::HandlePollFail()
{
    LOG_DEBUG("TBSM(Polling) - Handling poll fail");

    constexpr uint16_t kMaxFails = 5U;

    fail_cnt_++;
    if (fail_cnt_ >= kMaxFails)
    {
        LOG_DEBUG("TBSM(Polling) - Too many fails, changing to fake mode");
        event_queue_.push(FAKE);
    }
    else
    {
        // Nothing to do yet
    }
}
