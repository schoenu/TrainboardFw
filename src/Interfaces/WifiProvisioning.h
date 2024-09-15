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

#ifndef WIFI_PROVISIONING_H_
#define WIFI_PROVISIONING_H_

#include <optional>

/// @brief Connect to saved networks, start config portal if no network is saved
/// @param timeout_s If the connection is unsuccessful after this time, returns `false`
/// @return
/// - `std::nullopt` if the connection process is not finished
/// - `true` if the device could connect to a known network
/// - `false` if no connection to known networks could be established
/// @note This function may also be implemented as a blocking call
std::optional<bool> WifiProv_Connect(uint32_t timeout_s);

/// @brief Check if the device is connected to a Wi-Fi network
/// @return `true` if connected, `false` otherwise
bool WifiProv_IsConnectedToWifi();

/// @brief Clear saved networks.
/// @details After calling this function, the device must be provisioned again.
void WifiProv_ResetCredentials();

/// @brief Disconnect wifi, so it does not try to reconnect
void WifiProv_Disconnect();

/// @brief Check if wifi credentials are saved on the device
bool WifiProv_HasCredentials();

#endif  // WIFI_PROVISIONING_H_
