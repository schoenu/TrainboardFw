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

#ifndef FAST_LED_PRESENTER_H_
#define FAST_LED_PRESENTER_H_

#include "LedPresenter.h"

#include "FastLED.h"
#include "FwConfig.h"
#include "Logging.h"

#include "etl/to_string.h"

class FastLedPresenter : public LedPresenter
{
    void Show() override
    {
        FastLED.show();
    }
    void SetBrightness(uint8_t brightness) override
    {
        if (FastLED.getBrightness() != brightness)
        {
#if 0  // For debug only
            etl::string<64> msg{"Setting brightness : "};
            etl::to_string(brightness, msg, true);
            LOG_DEBUG(msg.c_str());
#endif
            FastLED.setBrightness(brightness);
            FastLED.show();
        }
    }

  public:
    void Init()
    {
        pinMode(kLedLevelShiftEnPin, OUTPUT);
        digitalWrite(kLedLevelShiftEnPin, HIGH);
        SetBrightness(kDefaultBrightness);
    }
};

#endif  // FAST_LED_PRESENTER_H_