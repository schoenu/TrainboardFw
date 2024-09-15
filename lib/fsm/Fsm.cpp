// Copyright (C) 2024 Emile Decosterd
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

#include "Fsm.h"

void Fsm::Init()
{
    initial_state_.Init();
    initial_state_.Enter();
    actual_state_ = &initial_state_;
}

Fsm::Status Fsm::Dispatch(const uint16_t event)
{
    auto status = Status::kOk;

    if (nullptr == actual_state_)
    {
        status = Status::kError;
    }
    else
    {
        if (is_transitioning_)
        {
            status = Status::kBusy;
        }
        else
        {
            is_transitioning_ = true;
            auto transition = actual_state_->ProcessEvent(event);
            status = Execute(transition);
            is_transitioning_ = false;
        }
    }

    return status;
}

Fsm::Status Fsm::Execute(FsmTransition* const transition)
{
    if (nullptr == actual_state_)
    {
        return Status::kError;
    }

    if (transition == nullptr)
    {
        // There is no transition associated to this event
    }
    else
    {
        auto destination = transition->GetDestinationState();
        if (destination == nullptr)
        {
            // We stay in the same state, but execute the transition
            transition->Execute();
        }
        else
        {
            actual_state_->Exit();
            transition->Execute();
            destination->Enter();
            actual_state_ = destination;
        }
    }
    return Status::kOk;
}
