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
#include <functional>
#include "etl/array.h"

// Mocks
#include "TestLedStrip.h"

class SmoothTransitionTest : public ::testing::Test
{
  protected:
    static constexpr uint32_t kTransitionDurationInTicks = 20;
    static constexpr size_t kStripLength = 8U;
    static constexpr size_t kNumberOfStrips = 1U;
    using Manager = TrainboardLedManager<kNumberOfStrips>;
    TestLedStrip<kStripLength> strip;
    etl::array<std::reference_wrapper<LedStrip>, kNumberOfStrips> strips{strip};
    Manager led_manager{strips, strip, kTransitionDurationInTicks};
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
    void SetInitialLeds(const std::array<uint32_t, kStripLength>& initial_led_data)
    {
        const std::array<Led, kStripLength> initial_leds{
            Led(0, initial_led_data[0]),
            Led(1, initial_led_data[1]),
            Led(2, initial_led_data[2]),
            Led(3, initial_led_data[3]),
            Led(4, initial_led_data[4]),
            Led(5, initial_led_data[5]),
            Led(6, initial_led_data[6]),
            Led(7, initial_led_data[7]),
        };
        strip.SetLeds(initial_led_data);
        led_manager.SetLeds(initial_leds.data(), initial_leds.size());
        ExecuteWholeTransition();
    }
};

TEST_F(SmoothTransitionTest, DurationOfTransition)
{
    for (auto i = 0; i < kTransitionDurationInTicks - 1; i++)
    {
        const auto did_finish_transition = led_manager.RefreshTransition();
        ASSERT_FALSE(did_finish_transition);
    }
    const auto is_finished = led_manager.RefreshTransition();
    EXPECT_TRUE(is_finished);
}

TEST_F(SmoothTransitionTest, TransitionFinished_SetLeds_ResetTransitionDuration)
{
    // GIVEN
    ExecuteWholeTransition();

    // WHEN
    constexpr size_t kNFakeLeds = 8U;
    std::array<Led, kNFakeLeds> leds{};
    led_manager.SetLeds(leds.data(), leds.size());

    // THEN
    const auto is_finished_transitioning = led_manager.RefreshTransition();
    EXPECT_FALSE(is_finished_transitioning);
}

TEST_F(SmoothTransitionTest, LedDoesNotChange_StillThere)
{
    // GIVEN
    const std::array<uint32_t, kStripLength> initial_led_data{1, 2, 3, 4, 5, 6, 7, 8};
    SetInitialLeds(initial_led_data);

    // WHEN
    const std::array<Led, kStripLength> new_leds{
        Led(0, initial_led_data[0]),
        Led(1, initial_led_data[1]),
        Led(2, initial_led_data[2]),
        Led(3, initial_led_data[3]),
        Led(4, initial_led_data[4]),
        Led(5, initial_led_data[5]),
        Led(6, initial_led_data[6]),
        Led(7, initial_led_data[7]),
    };
    led_manager.SetLeds(new_leds.data(), new_leds.size());
    ExecuteWholeTransition();

    // THEN
    const auto new_led_data = strip.GetData();
    EXPECT_EQ(new_led_data, initial_led_data);
}
TEST_F(SmoothTransitionTest, LedIn_ThereAfterTransition)
{
    // GIVEN
    const std::array<uint32_t, kStripLength> initial_led_data{};
    const std::array<Led, kStripLength> new_leds{
        Led(0, 1),
        Led(1, 2),
        Led(2, 3),
        Led(3, 4),
        Led(4, 5),
        Led(5, 6),
        Led(6, 7),
        Led(7, 8),
    };
    strip.SetLeds(initial_led_data);

    // WHEN
    led_manager.SetLeds(new_leds.data(), new_leds.size());
    ExecuteWholeTransition();

    // THEN
    const auto new_led_data = strip.GetData();
    const std::array<uint32_t, kStripLength> new_led_data_expected = {1, 2, 3, 4, 5, 6, 7, 8};
    EXPECT_EQ(new_led_data, new_led_data_expected);
}
TEST_F(SmoothTransitionTest, LedOut_NotThereAfterTransition)
{
    // GIVEN
    const std::array<uint32_t, kStripLength> initial_led_data{1, 2, 3, 4, 5, 6, 7, 8};
    SetInitialLeds(initial_led_data);

    // WHEN
    const std::array<Led, kStripLength - 1> new_leds{
        Led(0, 1),
        Led(1, 2),
        Led(2, 3),
        Led(3, 4),
        // Turn off Led(4,5)
        Led(5, 6),
        Led(6, 7),
        Led(7, 8),
    };
    led_manager.SetLeds(new_leds.data(), new_leds.size());
    ExecuteWholeTransition();

    // THEN
    const auto new_led_data = strip.GetData();
    const std::array<uint32_t, kStripLength> new_led_data_expected = {1, 2, 3, 4, 0, 6, 7, 8};
    EXPECT_EQ(new_led_data, new_led_data_expected);
}

TEST_F(SmoothTransitionTest, LedSwap_OffAtHalfTransition)
{
    // GIVEN
    const std::array<uint32_t, kStripLength> initial_led_data{1, 2, 3, 4, 5, 6, 7, 8};
    SetInitialLeds(initial_led_data);

    // WHEN
    const std::array<Led, kStripLength> new_leds{
        Led(0, 1),
        Led(1, 2),
        Led(2, 3),
        Led(3, 5),  // swap
        Led(4, 4),  // swap
        Led(5, 6),
        Led(6, 7),
        Led(7, 8),
    };
    led_manager.SetLeds(new_leds.data(), new_leds.size());
    ExecuteHalfTransition();

    // THEN
    const auto new_led_data = strip.GetData();
    const std::array<uint32_t, kStripLength> new_led_data_expected = {1, 2, 3, 0, 0, 6, 7, 8};
    EXPECT_EQ(new_led_data, new_led_data_expected);
}

TEST_F(SmoothTransitionTest, LedSwap_SwapedAfterTransition)
{
    // GIVEN
    const std::array<uint32_t, kStripLength> initial_led_data{1, 2, 3, 4, 5, 6, 7, 8};
    SetInitialLeds(initial_led_data);

    // WHEN
    const std::array<Led, kStripLength> new_leds{
        Led(0, 1),
        Led(1, 2),
        Led(2, 3),
        Led(3, 5),  // swap
        Led(4, 4),  // swap
        Led(5, 6),
        Led(6, 7),
        Led(7, 8),
    };
    led_manager.SetLeds(new_leds.data(), new_leds.size());
    ExecuteWholeTransition();

    // THEN
    const auto new_led_data = strip.GetData();
    const std::array<uint32_t, kStripLength> new_led_data_expected = {1, 2, 3, 5, 4, 6, 7, 8};
    EXPECT_EQ(new_led_data, new_led_data_expected);
}
TEST_F(SmoothTransitionTest, LedNullptr_Assert)
{
    ASSERT_DEATH({
        led_manager.SetLeds(nullptr, 42U);

        // THEN
        // dies
    },
                 "ASSERT");
}

TEST_F(SmoothTransitionTest, AllLedsOff)
{
    // GIVEN
    const std::array<uint32_t, kStripLength> initial_led_data{1, 2, 3, 4, 5, 6, 7, 8};
    SetInitialLeds(initial_led_data);

    // WHEN
    const std::array<Led, kStripLength> random_leds{
        Led(0, 4321),
        Led(1, 9877),
        Led(2, 2222),
        Led(3, 1111),  // swap
        Led(4, 6666),  // swap
        Led(5, 3333),
        Led(6, 10000),
        Led(7, 5555),
    };
    led_manager.SetLeds(random_leds.data(), 0U);
    ExecuteWholeTransition();

    // THEN
    const auto new_led_data = strip.GetData();
    const std::array<uint32_t, kStripLength> new_led_data_expected = {0, 0, 0, 0, 0, 0, 0, 0};
    EXPECT_EQ(new_led_data, new_led_data_expected);
}

TEST_F(SmoothTransitionTest, SwapAllLeds)
{
    // GIVEN
    const std::array<uint32_t, kStripLength> initial_led_data{1, 2, 3, 4, 5, 6, 7, 8};
    SetInitialLeds(initial_led_data);

    // WHEN
    const std::array<Led, kStripLength> new_leds{
        Led(6, 7),
        Led(1, 8),
        Led(2, 3),
        Led(3, 5),
        Led(4, 4),
        Led(0, 6),
        Led(5, 1),
        Led(7, 2),
    };
    led_manager.SetLeds(new_leds.data(), new_leds.size());
    ExecuteWholeTransition();

    // THEN
    const auto new_led_data = strip.GetData();
    const std::array<uint32_t, kStripLength> new_led_data_expected = {6, 8, 3, 5, 4, 1, 7, 2};
    EXPECT_EQ(new_led_data, new_led_data_expected);
}

TEST_F(SmoothTransitionTest, MultipleTimesSameLed)
{
    // GIVEN
    const std::array<uint32_t, kStripLength> initial_led_data{1, 2, 3, 4, 5, 6, 7, 8};
    SetInitialLeds(initial_led_data);

    // WHEN
    const std::array<Led, kStripLength> new_leds{
        Led(6, 7),
        Led(1, 8),
        Led(2, 3),
        Led(3, 5),
        Led(3, 5),
        Led(3, 5),
        Led(5, 1),
        Led(7, 2),
    };
    led_manager.SetLeds(new_leds.data(), new_leds.size());
    ExecuteWholeTransition();

    // THEN
    const auto new_led_data = strip.GetData();
    const std::array<uint32_t, kStripLength> new_led_data_expected = {0, 8, 3, 5, 0, 1, 7, 2};
    EXPECT_EQ(new_led_data, new_led_data_expected);
}

TEST_F(SmoothTransitionTest, SamePositionDifferentColor)
{
    // GIVEN
    const std::array<uint32_t, kStripLength> initial_led_data{1, 2, 3, 4, 5, 6, 7, 8};
    SetInitialLeds(initial_led_data);

    // WHEN
    const std::array<Led, kStripLength> new_leds{
        Led(6, 7),
        Led(1, 8),
        Led(2, 3),
        Led(3, 5),
        Led(3, 6),  // The last one should be taken
        Led(4, 6),
        Led(5, 1),
        Led(7, 2),
    };
    led_manager.SetLeds(new_leds.data(), new_leds.size());
    ExecuteWholeTransition();

    // THEN
    const auto new_led_data = strip.GetData();
    const std::array<uint32_t, kStripLength> new_led_data_expected = {0, 8, 3, 6, 6, 1, 7, 2};
    EXPECT_EQ(new_led_data, new_led_data_expected);
}
