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

#ifndef SERVER_COMMUNICATION_H_
#define SERVER_COMMUNICATION_H_

#include <cstdint>
#include <optional>

/// @brief Get the data for one frame from the server
/// @param buffer [out] Pointer to the memory where the data from the server will be written to
/// @param max_length Size of the provided buffer
/// @return
/// - `std::nullopt` if the communication with the server is still on-going
/// - 0 if the communication with the server failed
/// - the length of the data read from the server if the communication was successful
std::optional<uint32_t> ServerCom_GetData(uint8_t* const buffer, const uint32_t max_length);

/// @brief Get the frame history from the server
/// @param buffer [out] Pointer to the memory where the data from the server will be written to
/// @param max_length Size of the provided buffer
/// @return
/// - `std::nullopt` if the communication with the server is still on-going
/// - 0 if the communication with the server failed
/// - the length of the data read from the server if the communication was successful
std::optional<uint32_t> ServerCom_GetHistoryData(uint8_t* const buffer, const uint32_t max_length);

/// @brief Check if there is a firmware update available and if so, update.
/// @return `false` if no update is available, `true` otherwise. The device may be reset in this function.
bool ServerCom_UpdateOta();

/// @brief Ping the server to check if it responds
/// @return
/// - `std::nullopt` if the communication with the server is still on-going
/// - `false` if the communication with the server failed
/// - `true` if the communication was successful
std::optional<bool> ServerCom_Ping();

#endif  // SERVER_COMMUNICATION_H_
