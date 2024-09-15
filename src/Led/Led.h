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

#ifndef LED_H_
#define LED_H_

#include <cstdint>

enum class LedColor : uint32_t
{
    kBlack = 0x000000U,
    kWhite = 0xFFFFFFU,
    kRed = 0xFF0000U,
    kBlue = 0x0000FFU,
    kGreen = 0x00FF00U,
    kYellow = 0xF7DC6FU,
    kPurple = 0x8E44ADU,
};

class Led
{
  public:
    Led() : id_(0), color_(0), scale_(0) {}
    Led(uint16_t id, uint32_t color, uint8_t scale = UINT8_MAX - 1) : id_(id), color_(color), scale_(scale) {}

    uint32_t GetId() const
    {
        return id_;
    }

    uint32_t GetStripId() const
    {
        return ((id_ & kLedStripMask) >> kLedStripShift);
    }

    uint32_t GetPosition() const
    {
        return (id_ & kLedPositionMask);
    }

    uint32_t GetColor() const
    {
        // Return the color 1:1 (hex format, only the 24 last bits should be set
        return (color_ & kLedColorMask);
    }

    uint8_t GetScale() const { return scale_; }

    bool operator==(const Led& other_led) const { return (other_led.id_ == id_) && (other_led.color_ == color_); }
    bool operator!=(const Led& other_led) const { return !(other_led == *this); }

  private:
    // clang-format off
    static constexpr uint32_t kLedStripShift    = 8U;
    static constexpr uint32_t kLedStripMask     = 0xFF00U;
    static constexpr uint32_t kLedPositionMask  = 0x00FFU;
    static constexpr uint32_t kLedColorMask     = 0xFFFFFFU;  // HTML Format, 24 bits
    // clang-format on
    uint16_t id_;
    uint32_t color_;
    uint8_t scale_;
};

#endif  // LED_H_
