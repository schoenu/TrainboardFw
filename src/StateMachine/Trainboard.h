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

#ifndef TRAINBOARD_H_
#define TRAINBOARD_H_

#include "Fsm.h"

#include "LightSensor.h"
#include "Signals.h"
#include "StateConnecting.h"
#include "StateLive.h"
#include "StateOffline.h"
#include "StatePinging.h"
#include "StatePolling.h"
#include "StateResetting.h"
#include "StateStarting.h"
#include "StateTransitioning.h"
#include "StateUpdating.h"
#include "TransitionFake.h"
#include "TransitionLoadHistory.h"
#include "TransitionPollServer.h"

class Trainboard
{
  public:
    Trainboard(EventQueue& event_queue, LedManager& led_manager) : event_queue_(event_queue), led_manager_(led_manager) {}
    void Init()
    {
        tb_fsm_.Init();
        resetting_.InitTimers();
        live_.InitTimers();
        offline_.InitTimers();
        led_manager_.SetBrightness(kDefaultBrightness);
    }
    void DispatchEvent(const uint16_t event)
    {
        (void)tb_fsm_.Dispatch(event);

        // This applies to all states. Could be more elegant if we were using a hierarchical state machine ;)
        if (TICK == event)
        {
            led_manager_.SetBrightness(LightSensor_GetBrightness());
        }
    }

  private:
    EventQueue& event_queue_;
    LedManager& led_manager_;

    // State machine states
    StateStarting starting_{led_manager_, event_queue_, {tran_connect_, tran_reset_}};
    StateResetting resetting_{led_manager_, event_queue_, tran_connect_};
    StateConnecting connecting_{led_manager_, event_queue_, {tran_ping_, tran_fake_}};
    StatePinging pinging_{led_manager_, event_queue_, {tran_load_history_, tran_fake_}};
    StatePolling polling_{led_manager_, event_queue_, {tran_fake_, tran_data_ok_}};
    StateTransitioning transitioning_{led_manager_, event_queue_, {tran_fake_, tran_load_history_, tran_live_animation_done_, tran_hist_animation_done_, tran_fake_animation_done_}};
    StateLive live_{led_manager_, {tran_load_history_, tran_fake_, tran_poll_server_, tran_update_}};
    StateOffline offline_{led_manager_, {tran_load_history_, tran_refresh_, tran_connect_}};
    StateUpdating updating_{event_queue_, tran_no_update_};

    // State machine transitions
    FsmTransition tran_reset_{&resetting_};
    FsmTransition tran_connect_{&connecting_};
    FsmTransition tran_ping_{&pinging_};
    TransitionPollServer tran_poll_server_{&polling_};
    TransitionLoadHistory tran_load_history_{&polling_};
    TransitionFake tran_fake_{&transitioning_};
    FsmTransition tran_data_ok_{&transitioning_};
    FsmTransition tran_refresh_{&transitioning_};
    FsmTransition tran_live_animation_done_{&live_};
    FsmTransition tran_hist_animation_done_{&transitioning_};
    FsmTransition tran_fake_animation_done_{&offline_};
    FsmTransition tran_update_{&updating_};
    FsmTransition tran_no_update_{&live_};

    // State machine
    Fsm tb_fsm_{starting_};
};

#endif  // TRAINBOARD_H_