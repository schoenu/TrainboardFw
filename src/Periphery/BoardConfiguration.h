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

#ifndef BOARD_CONFIGURATION_H_
#define BOARD_CONFIGURATION_H_

#include <cstdint>

#include "etl/string.h"

// WARNING: not thread-safe!

class BoardConfig
{
  public:
    enum class HardwareVersion
    {
        kV1,
        kV1_1,
        kV1_2,
        kUnknown,
        // Add more versions as needed
    };

    static BoardConfig& Get();

    static constexpr size_t kVersionStringMaxLength = 32U;
    void ReadHwVersion();
    HardwareVersion GetHwVersion() const { return hw_version_; }
    etl::string<kVersionStringMaxLength> GetHwVersionString() const { return hw_version_string_; }

  private:
    BoardConfig() = default;
    BoardConfig(const BoardConfig&) = delete;
    BoardConfig& operator=(const BoardConfig&) = delete;

    HardwareVersion hw_version_{HardwareVersion::kUnknown};
    etl::string<kVersionStringMaxLength> hw_version_string_{""};
    void ReadHwVersionAnalogPin();
    void StringifyHwVersion();
};

#endif  // BOARD_CONFIGURATION_H_
