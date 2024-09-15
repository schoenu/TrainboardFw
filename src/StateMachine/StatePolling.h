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

#ifndef STATE_POLLING_H_
#define STATE_POLLING_H_

#include "Fsm.h"
#include "FwConfig.h"

#include <optional>
#include "etl/array.h"

class EventQueue;
class LedManager;

class StatePolling : public FsmState
{
  public:
    struct Transitions
    {
        FsmTransition& fake;
        FsmTransition& data_ok;
    };
    StatePolling(LedManager& led_manager, EventQueue& event_queue, Transitions transitions)
        : led_manager_(led_manager),
          event_queue_(event_queue),
          fake_transition_(transitions.fake),
          data_ok_transition_(transitions.data_ok) {}

    void Enter() override;
    void Exit() override;
    FsmTransition* ProcessEvent(uint16_t event) override;

  private:
    // Infrastructure
    LedManager& led_manager_;
    EventQueue& event_queue_;

    // Transition
    FsmTransition& fake_transition_;
    FsmTransition& data_ok_transition_;

    // Household
    uint16_t fail_cnt_{0};
    etl::array<uint8_t, kNumberOfHistoryFrames * kBufferSizeInBytes> buffer_{};
    std::optional<uint32_t> (*get_data_delegate_)(uint8_t* const, const uint32_t){nullptr};
    bool (*is_data_valid_delegate_)(const uint8_t* const, const uint32_t){nullptr};
    void AssignDelegates();
    void HandleTickEvent();
    void EvaluateData(const uint32_t data_length, const uint8_t* const buffer_data);
    void HandlePollFail();
};

#endif  // STATE_POLLING_H_
