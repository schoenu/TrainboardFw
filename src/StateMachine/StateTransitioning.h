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

#ifndef STATE_TRANSITIONING_H_
#define STATE_TRANSITIONING_H_

#include "Fsm.h"
#include "FwConfig.h"
#include "Led.h"

#include <optional>
#include "etl/array.h"

class EventQueue;
class LedManager;

class StateTransitioning : public FsmState
{
  public:
    struct Transitions
    {
        FsmTransition& fake;
        FsmTransition& load_history;
        FsmTransition& live_done;
        FsmTransition& hist_done;
        FsmTransition& fake_done;
    };
    StateTransitioning(LedManager& led_manager, EventQueue& event_queue, Transitions transitions)
        : event_queue_(event_queue),
          led_manager_(led_manager),
          load_hist_transition_(transitions.load_history),
          fake_transition_(transitions.fake),
          live_done_transition_(transitions.live_done),
          hist_done_transition_(transitions.hist_done),
          fake_done_transition_(transitions.fake_done)
    {}

    void Enter() override;
    void Exit() override;
    FsmTransition* ProcessEvent(uint16_t event) override;

  private:
    // Infrastructure
    EventQueue& event_queue_;
    LedManager& led_manager_;

    // Transitions
    FsmTransition& load_hist_transition_;
    FsmTransition& fake_transition_;
    FsmTransition& live_done_transition_;
    FsmTransition& hist_done_transition_;
    FsmTransition& fake_done_transition_;

    // Housekeeping
    uint8_t conversion_fail_cnt_{0U};
    etl::array<uint8_t, kBufferSizeInBytes> buffer_{};
    etl::array<Led, kMaxLedsOn> leds_{};
    uint32_t ReadDataToBeDisplayed(uint8_t* const data, uint32_t size) const;
    void SetNewLedsToLedManager(const Led* const leds, std::optional<uint32_t> nr_of_leds);
    FsmTransition* HandleShortPush() const;
    FsmTransition* HandleTick() const;
};
#endif  // STATE_TRANSITIONING_H_
