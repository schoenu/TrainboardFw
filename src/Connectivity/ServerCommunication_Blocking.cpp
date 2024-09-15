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

#include "ServerCommunication.h"

// Project headers
#include "BoardConfiguration.h"
#include "BuildInfo.h"
#include "Certificates.h"
#include "FwConfig.h"
#include "Logging.h"

// Libraries
#include <Arduino.h>
#include <HTTPUpdate.h>
#include "HTTPClient.h"
#include "WiFi.h"
#include "etl/algorithm.h"
#include "etl/to_string.h"

static std::optional<uint32_t> GetData(uint8_t* const buffer, const uint32_t max_length, bool is_history_mode)
{
    uint32_t data_length = 0U;

    const String kServerUrl = "https://api.trainboard.ch";
    const String kProductUrl = "/tb1_1";

    WiFiClientSecure secure;
    secure.setCACert(kServerRootCACertificate);
    HTTPClient client;

    // Prepare request
    client.begin(secure, kServerUrl + kProductUrl);
    client.addHeader("fwv", String(FW_VERSION_FULL));
    client.addHeader("hwv", BoardConfig::Get().GetHwVersionString().c_str());
    client.addHeader("mac", String(WiFi.macAddress()));
    if (is_history_mode)
    {
        client.addHeader("com", "history_" + String(kNumberOfHistoryFrames));
    }
    client.setReuse(false);

    // Send it
    const int response = client.GET();

    // Evaluate result
    if (response > 0)
    {
        LOG_DEBUG("Success getting data from server!");

        // Make sure the buffer contains no data
        for (auto i = 0; i < max_length; i++)
        {
            buffer[i] = 0;
        }

        // Write data to buffer
        const auto received_data_length = static_cast<uint32_t>(client.getSize());
        etl::string<64> msg{"Received data length : "};
        etl::to_string(received_data_length, msg, true);
        msg.append("bytes");
        LOG_DEBUG(msg.c_str());

        const auto payload = client.getString();
        const auto length = etl::min(received_data_length, max_length);

        msg.clear();
        msg.append("Effective data length: ");
        etl::to_string(length, msg, true);
        msg.append("bytes");
        LOG_DEBUG(msg.c_str());

        for (auto i = 0; i < length; i++)
        {
            buffer[i] = payload[i];
        }
        data_length = length;
    }
    else
    {
        etl::string<64> error_message{"Error getting data from server, HTTP response: "};
        etl::to_string(response, error_message, true);
        LOG_WARN(error_message.c_str());
    }

    return data_length;
}

std::optional<uint32_t> ServerCom_GetData(uint8_t* const buffer, const uint32_t max_length)
{
    constexpr auto kLiveMode = false;
    return GetData(buffer, max_length, kLiveMode);
}

std::optional<uint32_t> ServerCom_GetHistoryData(uint8_t* const buffer, const uint32_t max_length)
{
    constexpr auto kHistoryMode = true;
    return GetData(buffer, max_length, kHistoryMode);
}

bool ServerCom_UpdateOta()
{
    bool did_succeed_updating = true;

    WiFiClientSecure client;
    client.setCACert(kServerRootCACertificate);
    auto ret = httpUpdate.update(client, "https://api.trainboard.ch/ota", FW_VERSION);

    switch (ret)
    {
        case HTTP_UPDATE_FAILED:
        {
            etl::string<128> error_message{"OTA update failed. Error "};
            etl::to_string(httpUpdate.getLastError(), error_message, true);
            etl::to_string(httpUpdate.getLastErrorString().c_str(), error_message, true);
            LOG_WARN(error_message.c_str());
            did_succeed_updating = false;
            break;
        }
        case HTTP_UPDATE_NO_UPDATES:
            LOG_INFO("No OTA update available");
            did_succeed_updating = false;
            break;

        case HTTP_UPDATE_OK:
            LOG_INFO("OTA update ok");
            break;
        default:
            break;
    }

    return did_succeed_updating;
}

std::optional<bool> ServerCom_Ping()
{
    const String kPingUrl = "https://api.trainboard.ch/ping";
    constexpr uint16_t kPingTimeoutMilliSeconds = 5000;

    bool is_ping_successful = false;

    WiFiClientSecure secure;
    secure.setCACert(kServerRootCACertificate);
    HTTPClient client;

    // Ping the server
    client.begin(secure, kPingUrl);
    client.setReuse(false);
    client.setTimeout(kPingTimeoutMilliSeconds);
    const int response = client.GET();

    // Evaluate response
    if (response > 0)
    {
        const auto received_data_length = static_cast<uint32_t>(client.getSize());
        if (received_data_length == 2)
        {
            const auto payload = client.getString();
            const uint8_t byte0 = payload[0];
            const uint8_t byte1 = payload[1];
            if ((byte0 == 0xBE) && (byte1 == 0xEF))
            {
                is_ping_successful = true;
            }
        }
    }
    return is_ping_successful;
}