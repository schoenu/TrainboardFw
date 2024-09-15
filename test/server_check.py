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

from urllib.request import Request, urlopen
import sys

# Ping server
ping = Request('https://api.trainboard.ch/ping')
try:
    ping_content = urlopen(ping);
    ping_data = list(ping_content.read())
    print("Ping returned data:")
    print(ping_data)
except Exception:
    sys.exit("Server not reachable")

# Poll server
try:
    req = Request('https://api.trainboard.ch/tb1_1')
    req.add_header('fwv', '0.2.3')
    req.add_header('hwv', 'v1.2')
    req.add_header('mac', '74:4D:BD:8B:6D:94') # MAC address of any existing board
    content = urlopen(req)
except Exception:
    sys.exit("Could not read out server data")

# Get bytes
data = list(content.read())

# Extract LED information
BYTES_PER_LED = 5
HEADER_BYTES = 2
n_leds = data[1]
print("Data Length: ")
print(len(data))
print("Received information for {} LEDs.".format(n_leds))
print("Received data:")
print(data)

data_length_expected = (BYTES_PER_LED * n_leds + HEADER_BYTES);
data_length = len(data)
if ( data_length_expected != data_length):
    print("Error data format!!")
    print("Expected data length of {}, got {}".format(data_length_expected, data_length))
    sys.exit()


for i in range(n_leds) :
    pos = i * BYTES_PER_LED + HEADER_BYTES

    strip_id = data[pos]
    led_id = data[pos + 1]

    red = data[pos + 2]
    green = data[pos + 3]
    blue = data[pos + 4]

    id = strip_id << 8 | led_id
    color = (red << 16) | (green << 8) | blue

    print("LED #{}: strip {}, pos {}, ({},{},{})".format(i, strip_id, led_id, red, green, blue))

print("};")