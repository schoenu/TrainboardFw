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

#include "Fsm.h"
#include "Logging.h"

#include <iostream>
#include <string>

class S1;
class S2;
class S3;

extern std::string sequence;
extern S1 state_1;
extern S2 state_2;
extern S3 state_3;

enum
{
    kA,
    kB,
    kC,
    kD,
    kFaulty,
};

class EventWithAction : public FsmTransition
{
  public:
    EventWithAction(FsmState* destination, const std::string& text) : FsmTransition(destination), text_(text) {}
    void Execute() override
    {
        sequence.append(text_);
    }

  private:
    std::string text_;
};

extern EventWithAction transition_a;
extern EventWithAction transition_b;
extern EventWithAction transition_c12;
extern EventWithAction transition_c21;
extern EventWithAction transition_d13;
extern EventWithAction transition_d31;
extern EventWithAction fault_transition;

class B : public FsmTransition
{
    using FsmTransition::FsmTransition;
    void Execute() override
    {
        sequence.append("A; ");
    }
};

class S1 : public FsmInitialState
{
    void Init() override
    {
        sequence.append("init; ");
    }
    void Enter() override
    {
        sequence.append("s1-e; ");
    }
    void Exit() override
    {
        sequence.append("s1-x; ");
    }
    FsmTransition* ProcessEvent(const uint16_t event) override
    {
        FsmTransition* transition = nullptr;
        switch (event)
        {
            case kA:
                transition = &transition_a;
                break;
            case kB:
                transition = &transition_b;
                break;
            case kC:
                transition = &transition_c12;
                break;
            case kD:
                transition = &transition_d13;
                break;
            default:
                break;
        }
        return transition;
    }
};
class S2 : public FsmState
{
    void Enter() override
    {
        sequence.append("s2-e; ");
    }
    void Exit() override
    {
        sequence.append("s2-x; ");
    }
    FsmTransition* ProcessEvent(const uint16_t event) override
    {
        FsmTransition* transition = nullptr;
        switch (event)
        {
            case kC:
                transition = &transition_c21;
                break;
            default:
                break;
        }
        return transition;
    }
};

class S3 : public FsmState
{
  public:
    S3(Fsm& sm, const uint16_t event) : sm_(sm), event_(event) {}
    void Enter() override
    {
        sequence.append("s3-e; ");
        status_ = sm_.Dispatch(event_);
    }
    void Exit() override
    {
        sequence.append("s3-x; ");
        status_ = sm_.Dispatch(event_);
    }
    FsmTransition* ProcessEvent(const uint16_t event) override
    {
        FsmTransition* transition = nullptr;
        switch (event)
        {
            case kFaulty:
                status_ = sm_.Dispatch(event);
                transition = &fault_transition;
                break;
            case kD:
                transition = &transition_d31;
            default:
                break;
        }
        return transition;
    }
    void ResetStatus() { status_ = Fsm::Status::kOk; }
    Fsm::Status GetStatus() const { return status_; }

  private:
    Fsm& sm_;
    const uint16_t event_;
    Fsm::Status status_ = Fsm::Status::kOk;
};

extern Fsm my_fsm;
