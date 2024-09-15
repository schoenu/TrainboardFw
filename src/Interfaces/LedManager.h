// This file provides the interface for the "LedManager" component, which
// controls the LEDs on the trainboard.
//
// All interactions with regard to LEDs happen through the "LedManager".
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

#ifndef LED_MANAGER_H_
#define LED_MANAGER_H_

#include <cstdint>

enum class LedColor : uint32_t;
class Led;

/// @brief Abstract class representing an object through which the
/// application interacts with the LEDs on the board
class LedManager
{
  public:
    /// @brief Set LEDs on the board to give some information about the board's state
    ///
    /// @return `false` if there is an on-going LED transition, `true` otherwise
    virtual bool SetStatusLed(LedColor color) = 0;

    /// @brief Set LEDs as a board test (e.g. all LEDs 'on', white)
    ///
    /// @return `false` if there is an on-going LED transition, `true` otherwise
    virtual bool SetTestLeds() = 0;

    /// @brief
    /// Set new LEDs to be displayed at the next transition
    ///
    /// @details
    /// The leds provided here are supposed to be valid and the length right.
    /// No return value is provided to indicate if the data is valid or not.
    virtual void SetLeds(const Led* const leds, uint32_t leds_length) = 0;

    /// @brief Immediately clears all LEDs on the board, interrupting any
    /// on-going transition.
    virtual void ClearAllLeds() = 0;

    /// @brief Update routine for the LED transition algorithm. Must be called
    /// continuously at a fixed rate until the transition is finished.
    ///
    /// @return `true` if transition is finished, `false` otherwise.
    virtual bool RefreshTransition() = 0;

    /// @brief Update the brightness of the LEDs.
    /// @param `brightness` : 0 (off) to 255 (full scale)
    virtual void SetBrightness(uint8_t brightness) = 0;

    virtual ~LedManager() = default;
};

#endif  // LED_MANAGER_H_
