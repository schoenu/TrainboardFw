# Trainboard Firmware
# Copyright (C) 2024 Emile Décosterd
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from pickle import FRAME
from urllib.request import Request, urlopen
import sys

NUMBER_OF_FRAMES = 45

# Poll server
req = Request('https://api.trainboard.ch/tb1_1')
req.add_header('com', 'history_' + str(NUMBER_OF_FRAMES))
req.add_header('fwv', '0.2.3')
req.add_header('hwv', 'v1.2')
req.add_header('mac', '74:4D:BD:8B:6D:94') # MAC address of any existing board
content = urlopen(req)

# Get bytes
data = list(content.read())
data_length =len(data)

# Extract LED information
BYTES_PER_LED = 5
HEADER_BYTES = 2
print("Data Length: ")
print(len(data))

# Open file for writing

code = """
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

#include "FakeData.h"
#include "DataManagerTypes.h"

#include <etl/array.h>
#include <etl/circular_buffer.h>

// clang-format off
static const Frame _fake_frames[kNumberOfHistoryFrames + 1] = {
"""

success = True
frame_cnt = 0
frame_start_index = 0
byte_cnt = 0
is_frame_length_info_in_valid_memory = lambda: frame_start_index + HEADER_BYTES < data_length
while frame_cnt < NUMBER_OF_FRAMES and is_frame_length_info_in_valid_memory() :
    n_leds = data[frame_start_index + 1]
    frame_length = HEADER_BYTES + n_leds*BYTES_PER_LED
    data[frame_start_index + 1] = n_leds + 1 # add fake LED
    print("Frame #{}:  length = {}, number of LEDs = {}".format(frame_cnt, frame_length, n_leds))
    if (frame_start_index + frame_length > data_length):
        print("Frame parsing error")
        success = False
        break
    code += "    {{{}, {{".format(frame_length + BYTES_PER_LED)
    for i in range(frame_length - 1):
        byte_cnt += 1
        code += "{}, ".format(data[frame_start_index + i])
    code += "{}, ".format(data[frame_start_index + frame_length - 1])
    code += "3, 56, 245, 110, 27, }},\n" # Additional orange led
    byte_cnt += 1
    frame_cnt += 1
    frame_start_index += frame_length
code += "    {},\n};\n"
code += "// clang-format off\n"

code += """
// Doing a const cast here should be ok as long as we do not push, pop, or overwrite the circular buffer.
// This should hold as the fake data circular buffer is defined with the `const` keyword.
const RealDataCircularBuffer g_fake_data{const_cast<void*>(static_cast<const void*>(&_fake_frames[0])), kNumberOfHistoryFrames};
"""

print("Data length with fake LEDs : ")
print(len(data) + BYTES_PER_LED*frame_cnt )

# Write fake data
if success:
    with open("FakeData.cpp", "w") as file:
        file.write(code)