// This file provides the interface for the "DataConverter" component.
// The "DataConverter" component interprets data previously downloaded
// from the Trainboard server.
//      - Checks that a given data stream can be converter to LEDs
//      - Converts a valid data stream to an array of LEDs
//
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

#ifndef DATA_CONVERTER_H_
#define DATA_CONVERTER_H_

#include <cstdint>
#include <optional>

class Led;

/// @brief
/// Checks that data containing one frame is valid.
///
/// @param data Buffer containing the data received from the server
/// @param data_length Number of bytes in the data buffer
///
/// @return `true` if the data is valid, `false` otherwise.
///
/// @details
/// The following conditions are checked:
///      - The provided buffer is valid
///      - The length of the data fits in the maximum frame size
///      - The length of the data corresponds to the length specified in the data
bool DataConv_IsDataValid(const uint8_t* const data, uint32_t data_length);

/// @brief
/// Checks that data containing all history frames is valid.
///
/// @param data Buffer containing the data received from the server
///  @param data_length Number of bytes in the data buffer
///
/// @return `true` if the data is valid, `false` otherwise.
///
/// @details
/// The following conditions are checked:
///      - The provided buffer is valid
///      - The length of the data fits in the maximum frame size times the history length
///      - Each frame in the data is valid
///      - The data stream contains the right amount of frames
bool DataConv_IsHistoryDataValid(const uint8_t* const data, uint32_t data_length);

/// @brief
/// Converts a data stream containing one frame to an array of LEDs.
///
/// @param data_in Buffer containing the data received from the server
/// @param data_length Number of bytes in the data buffer
/// @param leds_out [out] Buffer in which the LEDs will be written
/// @param max_leds_out Maximum number of expected LEDs
///
/// @return
///  `std::nullopt` when an input parameter pointer is invalid, 0 if the data
/// is invalid, or the number of converted LEDs when the data is valid.
std::optional<uint32_t> DataConv_DataToLeds(const uint8_t* const data_in,
                                            const uint32_t data_length,
                                            Led* const leds_out,
                                            const uint32_t max_leds_out);

#endif  // DATA_CONVERTER_H_
