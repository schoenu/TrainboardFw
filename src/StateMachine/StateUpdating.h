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

#ifndef STATE_UPDATING_H_
#define STATE_UPDATING_H_

#include "Fsm.h"

class EventQueue;

class StateUpdating : public FsmState
{
  public:
    StateUpdating(EventQueue& event_queue, FsmTransition& no_update_transition)
        : event_queue_(event_queue),
          no_update_transition_(no_update_transition) {}

    void Enter() override;
    void Exit() override;
    FsmTransition* ProcessEvent(uint16_t event) override;

  private:
    // Infrastructure
    EventQueue& event_queue_;

    // Transition
    FsmTransition& no_update_transition_;

    // Household
    void HandleTickEvent();
    void UpdateOta();
};

#endif  // STATE_UPDATING_H_
