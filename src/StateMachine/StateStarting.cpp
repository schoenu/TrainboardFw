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

#include "StateStarting.h"

#include "FwConfig.h"
#include "Led.h"
#include "LedManager.h"
#include "LightSensor.h"
#include "Logging.h"
#include "Signals.h"
#include "TimerTicker.h"

StateStarting::StateStarting(LedManager& led_manager, EventQueue& event_queue, Transitions transitions)
    : led_manager_(led_manager),
      event_queue_(event_queue),
      tran_delay_done_(transitions.delay_done),
      tran_reset_(transitions.reset),
      timer_(kTickPeriodMilliSeconds)
{}

void StateStarting::Init()
{
    LOG_DEBUG("TBSM - /i Starting ");
    timer_.Init();
    (void)TimerTicker_RegisterTimer(&timer_);
}

void StateStarting::Enter()
{
    LOG_DEBUG("TBSM - /e Starting ");

    const auto did_set_led = led_manager_.SetStatusLed(LedColor::kWhite);
    if (!did_set_led)
    {
        LOG_DEBUG("TBSM(Starting) - Could not set white LEDs!");
    }

    StartTimer();
}

void StateStarting::Exit()
{
    LOG_DEBUG("TBSM - /x Starting ");
    timer_.Stop();
}

FsmTransition* StateStarting::ProcessEvent(uint16_t event)
{
    FsmTransition* transition = nullptr;
    if (TICK == event)
    {
        if (timer_.HasExpired())
        {
            event_queue_.push(DELAY_DONE);
        }
    }
    else if (DELAY_DONE == event)
    {
        transition = &tran_delay_done_;
    }
    else if (SHORT_PUSH == event)
    {
        transition = &tran_reset_;
    }
    else
    {
        // Ignore other events
    }
    return transition;
}

void StateStarting::StartTimer()
{
    constexpr uint32_t kStartupDurationMilliSeconds = 2U * 1000U;
    const auto could_start = timer_.StartOneShot(kStartupDurationMilliSeconds);
    if (!could_start)
    {
        LOG_INFO("TBSM(Polling) - Could not start timer.");
    }
}
