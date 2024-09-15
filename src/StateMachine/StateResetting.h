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

#ifndef STATE_RESETTING_H_
#define STATE_RESETTING_H_

#include "Fsm.h"
#include "Timer.h"

class LedManager;
class EventQueue;

class StateResetting : public FsmState
{
  public:
    StateResetting(LedManager& led_manager, EventQueue& event_queue, FsmTransition& delay_done_transition);

    void InitTimers();
    void Enter() override;
    void Exit() override;
    FsmTransition* ProcessEvent(uint16_t event) override;

  private:
    // Infrastructure
    LedManager& led_manager_;
    EventQueue& event_queue_;

    // Transitions
    FsmTransition& tran_connect_;

    // Household
    Timer timer_;
};

#endif  // STATE_RESETTING_H_
