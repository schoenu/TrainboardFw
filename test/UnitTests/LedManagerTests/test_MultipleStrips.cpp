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

#include "gtest/gtest.h"

#include "Led.h"
#include "LedManager_Trainboard.h"
#include "LedStrip.h"

#include <array>

// Mocks
#include "TestLedStrip.h"

class MultipleStripsTest : public ::testing::Test
{
  protected:
    static constexpr uint32_t kTransitionDurationInTicks = 20;
    static constexpr size_t kStripLength1 = 8U;
    static constexpr size_t kStripLength2 = 7U;
    static constexpr size_t kStripLength3 = 9U;
    TestLedStrip<kStripLength1> strip1;
    TestLedStrip<kStripLength2> strip2;
    TestLedStrip<kStripLength3> strip3;
    static constexpr size_t kNumberOfStrips = 3U;
    using Manager = TrainboardLedManager<kNumberOfStrips>;
    etl::array<Manager::Strip, kNumberOfStrips> strips{strip1, strip2, strip3};
    Manager led_manager{strips, strip1, kTransitionDurationInTicks};
    void ExecuteWholeTransition()
    {
        for (auto i = 0; i < kTransitionDurationInTicks; i++)
        {
            (void)led_manager.RefreshTransition();
        }
    }
    void ExecuteHalfTransition()
    {
        for (auto i = 0; i < kTransitionDurationInTicks / 2; i++)
        {
            (void)led_manager.RefreshTransition();
        }
    }
};

TEST_F(MultipleStripsTest, AtInit_NoLedsSet)
{
}

TEST_F(MultipleStripsTest, NoLedsSet_NewLeds_LedsSet)
{
    constexpr uint32_t kIdStrip1 = 0 << 8U;
    constexpr uint32_t kIdStrip2 = 1 << 8U;
    constexpr uint32_t kIdStrip3 = 2 << 8U;
    const std::array<Led, 12> new_leds{
        Led(kIdStrip1 | 0, 42),
        Led(kIdStrip2 | 1, 1),
        Led(kIdStrip3 | 2, 2),
        Led(kIdStrip3 | 3, 3),
        Led(kIdStrip3 | 4, 4),
        Led(kIdStrip2 | 5, 5),
        Led(kIdStrip3 | 6, 6),
        Led(kIdStrip1 | 2, 7),
        Led(kIdStrip2 | 0, 8),
        Led(kIdStrip2 | 6, 9),
        Led(kIdStrip1 | 7, 10),
        Led(kIdStrip3 | 7, 11),
    };
    led_manager.SetLeds(new_leds.data(), new_leds.size());
    ExecuteWholeTransition();

    // THEN
    const auto strip1_data = strip1.GetData();
    const auto strip2_data = strip2.GetData();
    const auto strip3_data = strip3.GetData();
    const std::array<uint32_t, kStripLength1> strip1_data_expected{42, 0, 7, 0, 0, 0, 0, 10};
    const std::array<uint32_t, kStripLength2> strip2_data_expected{8, 1, 0, 0, 0, 5, 9};
    const std::array<uint32_t, kStripLength3> strip3_data_expected{0, 0, 2, 3, 4, 0, 6, 11, 0};
    EXPECT_EQ(strip1_data, strip1_data_expected);
    EXPECT_EQ(strip2_data, strip2_data_expected);
    EXPECT_EQ(strip3_data, strip3_data_expected);
}

TEST_F(MultipleStripsTest, RightStripIsTaken)
{
    constexpr uint32_t kIdStrip1 = 0 << 8U;
    constexpr uint32_t kIdStrip2 = 1 << 8U;
    const std::array<Led, 2> active_leds{
        Led(kIdStrip1 | 1, 42),
        Led(kIdStrip2 | 1, 42),
    };
    led_manager.SetLeds(active_leds.data(), active_leds.size());
    ExecuteWholeTransition();

    // WHEN
    const std::array<Led, 3> new_leds{
        Led(kIdStrip1 | 1, 42),
        Led(kIdStrip2 | 1, 3),
        Led(kIdStrip2 | 2, 42),
    };
    led_manager.SetLeds(new_leds.data(), new_leds.size());
    ExecuteHalfTransition();

    // THEN
    const auto strip1_data = strip1.GetData();
    const auto strip2_data = strip2.GetData();
    const std::array<uint32_t, kStripLength1> strip1_data_expected{0, 42, 0, 0, 0, 0, 0, 0};
    const std::array<uint32_t, kStripLength2> strip2_data_expected{0, 0, 0, 0, 0, 0, 0};
    EXPECT_EQ(strip1_data, strip1_data_expected);
    EXPECT_EQ(strip2_data, strip2_data_expected);
}

TEST_F(MultipleStripsTest, ImpossibleLedId_Ignore)
{
    constexpr uint32_t kIdStrip1 = 0 << 8U;
    constexpr uint32_t kIdStrip2 = 1 << 8U;
    constexpr uint32_t kIdStrip3 = 2 << 8U;
    const std::array<Led, 15> new_leds{
        Led(kIdStrip1 | 0, 42),
        Led(kIdStrip1 | kStripLength1, 42),
        Led(kIdStrip2 | 1, 1),
        Led(kIdStrip3 | 2, 2),
        Led(kIdStrip3 | 3, 3),
        Led(kIdStrip3 | 4, 4),
        Led(kIdStrip2 | 5, 5),
        Led(kIdStrip2 | kStripLength2, 5),
        Led(kIdStrip3 | 6, 6),
        Led(kIdStrip1 | 2, 7),
        Led(kIdStrip2 | 0, 8),
        Led(kIdStrip2 | 6, 9),
        Led(kIdStrip1 | 7, 10),
        Led(kIdStrip3 | kStripLength3, 11),
        Led(kIdStrip3 | 7, 11),
    };
    led_manager.SetLeds(new_leds.data(), new_leds.size());
    ExecuteWholeTransition();

    // THEN
    const auto strip1_data = strip1.GetData();
    const auto strip2_data = strip2.GetData();
    const auto strip3_data = strip3.GetData();
    const std::array<uint32_t, kStripLength1> strip1_data_expected{42, 0, 7, 0, 0, 0, 0, 10};
    const std::array<uint32_t, kStripLength2> strip2_data_expected{8, 1, 0, 0, 0, 5, 9};
    const std::array<uint32_t, kStripLength3> strip3_data_expected{0, 0, 2, 3, 4, 0, 6, 11, 0};
    EXPECT_EQ(strip1_data, strip1_data_expected);
    EXPECT_EQ(strip2_data, strip2_data_expected);
    EXPECT_EQ(strip3_data, strip3_data_expected);
}
