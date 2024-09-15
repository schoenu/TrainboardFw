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

#ifndef STATE_CONNECTING_H_
#define STATE_CONNECTING_H_

#include "Fsm.h"

class LedManager;
class EventQueue;

class StateConnecting : public FsmState
{
  public:
    struct Transitions
    {
        FsmTransition& ping;
        FsmTransition& fake;
    };
    StateConnecting(LedManager& led_manager, EventQueue& event_queue, Transitions transitions)
        : led_manager_(led_manager),
          event_queue_(event_queue),
          ping_transition_(transitions.ping),
          fake_transition_(transitions.fake)
    {}

    void Enter() override;
    void Exit() override;
    FsmTransition* ProcessEvent(uint16_t event) override;

  private:
    // Infrastructure
    LedManager& led_manager_;
    EventQueue& event_queue_;

    // Transitions
    FsmTransition& ping_transition_;
    FsmTransition& fake_transition_;

    // Helper functions
    void ConnectToWifi() const;
};

#endif  // STATE_CONNECTING_H_
