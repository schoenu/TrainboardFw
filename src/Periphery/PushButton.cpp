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

#include "PushButton.h"

#include "FwConfig.h"
#include "Logging.h"
#include "Signals.h"

#include "Arduino.h"

void PushButton::Init()
{
    pinMode(pin_nr_, INPUT_PULLUP);
}
void PushButton::Dispatch(const uint16_t event)
{
    if (TICK == event)
    {
        if (digitalRead(pin_nr_) == LOW)
        {
            debounce_cnt_++;
            if (debounce_cnt_ > kDebounceDurationMilliSeconds / kTickPeriodMilliSeconds)
            {
                is_pushed_ = true;
            }
        }
        else
        {
            if (debounce_cnt_ > kShortPushMinDurationMilliSeconds / kTickPeriodMilliSeconds)
            {
                event_queue_.push(SHORT_PUSH);
                LOG_DEBUG("PushButton - Short push");
            }
            else
            {
                // No push detected
            }
            debounce_cnt_ = 0;
            is_pushed_ = false;
        }
    }
}