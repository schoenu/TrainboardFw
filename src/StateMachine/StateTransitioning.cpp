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

#include "StateTransitioning.h"

#include "DataConverter.h"
#include "DataManager.h"
#include "Led.h"
#include "LedManager.h"
#include "Logging.h"
#include "Signals.h"
#include "WifiProvisioning.h"

void StateTransitioning::Enter()
{
    LOG_DEBUG("TBSM - /e Transitioning ");

    auto data_length = ReadDataToBeDisplayed(buffer_.data(), kBufferSizeInBytes);
    auto conversion_result = DataConv_DataToLeds(buffer_.data(), data_length, leds_.data(), kMaxLedsOn);

    LOG_DEBUG("Converted Leds");

    SetNewLedsToLedManager(leds_.data(), conversion_result);
}

void StateTransitioning::Exit()
{
    LOG_DEBUG("TBSM - /x Transitioning ");
}

FsmTransition* StateTransitioning::ProcessEvent(uint16_t event)
{
    FsmTransition* transition = nullptr;
    if (TICK == event)
    {
        transition = HandleTick();
    }
    else if (SHORT_PUSH == event)
    {
        transition = HandleShortPush();
    }
    else if ((DISCONNECTED == event) || (NETWORK_DOWN == event))
    {
        transition = &fake_done_transition_;
    }
    else if (FAKE == event)
    {
        transition = &fake_transition_;
    }
    else
    {
        // Other events ignored
    }
    return transition;
}

uint32_t StateTransitioning::ReadDataToBeDisplayed(uint8_t* const data, uint32_t size) const
{
    auto reader = DataMgr_GetReader();
    ASSERT(nullptr != reader);
    auto data_length = reader->ReadData(data, size);
    return data_length;
}

void StateTransitioning::SetNewLedsToLedManager(const Led* const leds, std::optional<uint32_t> nr_of_leds)
{
    if (nr_of_leds.has_value())
    {
        conversion_fail_cnt_ = 0U;
        auto leds_size = nr_of_leds.value();
        led_manager_.SetLeds(leds, leds_size);
    }
    else
    {
        conversion_fail_cnt_++;
        auto mode = DataMgr_GetReaderMode();
        if (DataReaderMode::kOffline == mode)
        {
            // In fake mode, the conversion should always be successful
            ASSERT(false);
        }
        else if (conversion_fail_cnt_ >= 3U)
        {
            conversion_fail_cnt_ = 0U;
            event_queue_.push(FAKE);
        }
        else
        {
            // No action yet, retrying
        }
    }
}

FsmTransition* StateTransitioning::HandleShortPush() const
{
    FsmTransition* transition = nullptr;

    const auto read_mode = DataMgr_GetReaderMode();
    if (read_mode != DataReaderMode::kOffline)
    {
        led_manager_.ClearAllLeds();
        if (read_mode == DataReaderMode::kLive)
        {
            LOG_INFO("TBSM - Setting history mode");
            DataMgr_SetReaderMode(DataReaderMode::kHistory);
        }
        else
        {
            LOG_INFO("TBSM - Setting live mode");
            DataMgr_SetReaderMode(DataReaderMode::kLive);
        }
        transition = &load_hist_transition_;
    }

    return transition;
}

FsmTransition* StateTransitioning::HandleTick() const
{
    FsmTransition* transition = nullptr;
    auto transition_done = led_manager_.RefreshTransition();
    if (transition_done)
    {
        LOG_INFO("TBSM - Transition done");
        switch (DataMgr_GetReaderMode())
        {
            case DataReaderMode::kLive:
                transition = &live_done_transition_;
                break;
            case DataReaderMode::kHistory:
                transition = &hist_done_transition_;
                break;
            case DataReaderMode::kOffline:
                transition = &fake_done_transition_;
                break;
        }
    }
    return transition;
}
