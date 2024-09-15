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

#ifndef FSM_H_
#define FSM_H_

#include <atomic>
#include <cstdint>

class FsmState;

class FsmTransition
{
  public:
    explicit FsmTransition(FsmState* const destination) : destination_(destination) {}
    virtual ~FsmTransition() = default;
    virtual void Execute(){
        // By default, there is no action associated with a transition.
    };
    FsmState* GetDestinationState() const { return destination_; }

  private:
    FsmState* const destination_;
};

class FsmState
{
  public:
    virtual FsmTransition* ProcessEvent(const uint16_t event) = 0;
    virtual void Enter() = 0;
    virtual void Exit() = 0;
    virtual ~FsmState() = default;
};

class FsmInitialState : public FsmState
{
  public:
    virtual void Init() = 0;
};

class Fsm
{
  public:
    enum class Status
    {
        kOk,
        kBusy,
        kError,
    };
    explicit Fsm(FsmInitialState& initial_state) : initial_state_(initial_state) {}
    void Init();
    Status Dispatch(const uint16_t event);
    bool IsInState(const FsmState* const state) const { return state == actual_state_; };

  private:
    FsmInitialState& initial_state_;
    FsmState* actual_state_ = nullptr;
    std::atomic<bool> is_transitioning_{false};
    Status Execute(FsmTransition* const transition);
};

#endif  // FSM_H_
