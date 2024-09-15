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

#ifndef SIGNALS_H_
#define SIGNALS_H_

#include <cstdint>

#include "FwConfig.h"
#include "etl/queue.h"

enum TrainboardSignal : uint16_t
{
    TICK,
    DELAY_DONE,
    PING,
    FAKE,
    POLL_SERVER,
    LOAD_HISTORY,
    SHORT_PUSH,
    RECONNECT,
    DATA_OK,
    RETRY,
    LIVE_ANIMATION_DONE,
    HIST_ANIMATION_DONE,
    FAKE_ANIMATION_DONE,
    NETWORK_UP,
    NETWORK_DOWN,
    CONNECTED,
    DISCONNECTED,
    CHECK_UPDATE,
    NO_UPDATE,
    N_SIGNALS,
};

class EventQueue : public etl::queue<uint16_t, kEventQueueSize>
{
  public:
    using etl::queue<uint16_t, kEventQueueSize>::queue;
};
#endif  // SIGNALS_H_
