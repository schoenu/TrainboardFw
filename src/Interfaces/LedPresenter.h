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

#ifndef LED_PRESENTER_H_
#define LED_PRESENTER_H_

#include <cstdint>

/// @brief Represents an object to present the LEDs on the board.
class LedPresenter
{
  public:
    /// @brief Update the brightness of the LEDs.
    ///@param brightness 0 (off) to 255 (full scale)
    virtual void SetBrightness(uint8_t brightness) = 0;

    /// @brief Shifts out the data to the LEDs.
    virtual void Show() = 0;
    virtual ~LedPresenter() = default;
};

#endif  // LED_STRIP_H_
