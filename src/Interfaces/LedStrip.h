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

#ifndef LED_STRIP_H_
#define LED_STRIP_H_

#include <cstdint>

/// @brief Represents an LED strip
class LedStrip
{
  public:
    /// @brief Do everything needed before LEDs can be set.
    virtual void Init() = 0;

    /// @brief Set an LED on the strip
    /// @param position 0-based index of the LED on the strip
    /// @param html_color 24-bit color in HTML format
    /// @param scaling 0-255 use this for fading effects
    virtual void Set(uint32_t position, uint32_t html_color, uint8_t scaling) = 0;

    /// @brief Get the length of the strip
    /// @return Number of LEDs on the strip
    virtual uint32_t GetSize() const = 0;

    /// @brief Clear all LEDs on the strip
    virtual void ClearAll() = 0;

    /// @brief Test the LEDs (e.g. all LEDs white)
    virtual void Test() = 0;

    virtual ~LedStrip() = default;
};

#endif  // LED_STRIP_H_
