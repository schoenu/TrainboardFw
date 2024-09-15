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

#ifndef TIMER_TICKER_H_
#define TIMER_TICKER_H_

#include <cstdint>

class Timer;

bool TimerTicker_Start(uint32_t tick_period_ms = 0);
bool TimerTicker_RegisterTimer(Timer* const timer);

#ifdef UNIT_TEST
void TimerTicker_Tick();
#endif

#endif  // TIMER_TICKER_H_
