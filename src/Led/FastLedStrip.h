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

#ifndef FAST_LED_STRIP_H_
#define FAST_LED_STRIP_H_

#include "LedStrip.h"
#include "Logging.h"

// Lib
#include "FastLED.h"
#include "etl/algorithm.h"
#include "etl/array.h"

template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class CHIPSET, size_t N_LED, uint32_t PIN>
class FastLedStrip : public LedStrip
{
    void Init() override
    {
        FastLED.addLeds<CHIPSET, PIN, GRB>(led_data_.data(), N_LED);
    }

    uint32_t GetSize() const override { return N_LED; }

    void ClearAll() override
    {
        etl::array<CRGB, N_LED> all_off{};
        all_off.fill(CRGB::Black);
        led_data_.swap(all_off);
    }

    void Set(uint32_t position, uint32_t html_color, uint8_t scaling) override
    {
        ASSERT(position < N_LED);

        led_data_.at(position) = CRGB(html_color).nscale8(scaling);
    }

    void Test() override
    {
        etl::array<CRGB, N_LED> all_on{};
        all_on.fill(CRGB::White);
        led_data_.swap(all_on);
    }

  private:
    etl::array<CRGB, N_LED> led_data_;
};

#endif  // FAST_LED_STRIP_H_