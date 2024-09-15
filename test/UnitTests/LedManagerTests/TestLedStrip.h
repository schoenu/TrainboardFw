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

#pragma once

#include <array>

template<size_t N>
class TestLedStrip : public LedStrip, public LedPresenter
{
  public:
    void Init() override
    {
        leds.fill(0);
    }
    void Set(uint32_t pos, uint32_t html_color, uint8_t scaling) override
    {
        if (pos < N)
        {
            if (scaling > 0)
            {
                leds[pos] = html_color;
            }
            else
            {
                leds[pos] = 0U;
            }
        }
        else
        {
            FAIL() << "Led position out of bounds";
        }
    }
    std::array<uint32_t, N>& GetData()
    {
        return leds;
    }
    void SetLeds(const std::array<uint32_t, N>& leds_in)
    {
        leds = leds_in;
    }
    uint32_t GetSize() const override { return N; }
    void ClearAll() override
    {
        std::array<uint32_t, N> leds_off{};
        leds.swap(leds_off);
    }
    void Test() override
    {
        for (size_t i = 0; i < N; i++)
        {
            leds.at(i) = 0xFFFFFF;
        }
    }
    void SetBrightness(uint8_t brightness) override { brightness_ = brightness; }
    uint8_t GetBrightness() const { return brightness_; }
    void Show() override { did_show_ = true; }
    void ResetDidShow() { did_show_ = false; }
    bool GetDidShow() const { return did_show_; }

  private:
    std::array<uint32_t, N> leds{};
    uint8_t brightness_{UINT8_MAX};
    bool did_show_{false};
};
