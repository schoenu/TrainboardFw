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

class StatusLedsTest : public ::testing::Test
{
  protected:
    static constexpr uint32_t kTransitionDurationInTicks = 20;
    static constexpr size_t kStripLength = 8U;
    static constexpr uint32_t kIdStrip1 = 0 << 8U;
    static constexpr uint32_t kIdStrip2 = 1 << 8U;
    TestLedStrip<kStripLength> strip1;
    TestLedStrip<kStripLength> strip2;
    static constexpr size_t kNumberOfStrips = 2U;
    using Manager = TrainboardLedManager<kNumberOfStrips>;
    etl::array<Manager::Strip, kNumberOfStrips> strips{strip1, strip2};
    Manager led_manager{strips, strip1, kTransitionDurationInTicks};
    void SetUp() override
    {
        const std::array<Led, 12> new_leds{
            Led(kIdStrip1 | 0, 42),
            Led(kIdStrip2 | 1, 1),
            Led(kIdStrip2 | 5, 5),
            Led(kIdStrip1 | 2, 7),
            Led(kIdStrip2 | 0, 8),
            Led(kIdStrip2 | 6, 9),
            Led(kIdStrip1 | 7, 10),
        };
        led_manager.SetLeds(new_leds.data(), new_leds.size());
        ExecuteWholeTransition();
    }

    void ExecuteWholeTransition()
    {
        for (auto i = 0; i < kTransitionDurationInTicks; i++)
        {
            (void)led_manager.RefreshTransition();
        }
    }
};

TEST_F(StatusLedsTest, Transitioning_SetStatusLed_ReturnFalse)
{
    strip1.ResetDidShow();
    Led led{};
    led_manager.SetLeds(&led, 0);  // Dummy led, starts the transition algorithm
    const auto did_set_leds = led_manager.SetStatusLed(LedColor::kBlue);
    EXPECT_FALSE(did_set_leds);
    EXPECT_FALSE(strip1.GetDidShow());
}

TEST_F(StatusLedsTest, NotTransitioning_SetStatusLed_ReturnTrue)
{
    strip1.ResetDidShow();
    const auto did_set_leds = led_manager.SetStatusLed(LedColor::kBlue);
    EXPECT_TRUE(did_set_leds);
    EXPECT_TRUE(strip1.GetDidShow());
}

TEST_F(StatusLedsTest, SetStatusLeds_FirstLedOfEachStripOn)
{
    strip1.ResetDidShow();
    led_manager.SetStatusLed(LedColor::kRed);
    const auto strip1_data = strip1.GetData();
    const auto strip2_data = strip2.GetData();
    const std::array<uint32_t, kStripLength> strip1_data_expected{static_cast<uint32_t>(LedColor::kRed), 0};
    const std::array<uint32_t, kStripLength> strip2_data_expected{static_cast<uint32_t>(LedColor::kRed), 0};
    EXPECT_EQ(strip1_data, strip1_data_expected);
    EXPECT_EQ(strip2_data, strip2_data_expected);
    EXPECT_TRUE(strip1.GetDidShow());
}

TEST_F(StatusLedsTest, ClearLeds_LedsOffAndNoMoreActiveLeds)
{
    // GIVEN
    strip1.ResetDidShow();

    // WHEN
    led_manager.ClearAllLeds();

    // THEN
    const std::array<uint32_t, kStripLength> strip1_data_expected{0};
    const std::array<uint32_t, kStripLength> strip2_data_expected{0};

    // 1. Data Cleared
    auto strip1_data = strip1.GetData();
    auto strip2_data = strip2.GetData();
    EXPECT_EQ(strip1_data, strip1_data_expected);
    EXPECT_EQ(strip2_data, strip2_data_expected);
    EXPECT_TRUE(strip1.GetDidShow());

    // 2. Active LEDs are zero => when doing a full transition, leds still zero
    ExecuteWholeTransition();
    strip1_data = strip1.GetData();
    strip2_data = strip2.GetData();
    EXPECT_EQ(strip1_data, strip1_data_expected);
    EXPECT_EQ(strip2_data, strip2_data_expected);
}

TEST_F(StatusLedsTest, NotTransitioning_SetTestLeds_ReturnTrue)
{
    const auto did_set_leds = led_manager.SetTestLeds();
    EXPECT_TRUE(did_set_leds);
    EXPECT_TRUE(strip1.GetDidShow());
}

TEST_F(StatusLedsTest, Transitioning_SetTestLeds_ReturnFalse)
{
    Led dummy_led{};
    led_manager.SetLeds(&dummy_led, 0);
    strip1.ResetDidShow();
    const auto did_set_leds = led_manager.SetTestLeds();
    EXPECT_FALSE(did_set_leds);
    EXPECT_FALSE(strip1.GetDidShow());
}

TEST_F(StatusLedsTest, SetTestLeds_AllLedsWhite)
{
    led_manager.SetTestLeds();
    const auto strip1_data = strip1.GetData();
    const auto strip2_data = strip2.GetData();
    std::array<uint32_t, kStripLength> strip1_data_expected;
    std::array<uint32_t, kStripLength> strip2_data_expected;
    for (size_t i = 0; i < kStripLength; i++)
    {
        strip1_data_expected.at(i) = static_cast<uint32_t>(LedColor::kWhite);
        strip2_data_expected.at(i) = static_cast<uint32_t>(LedColor::kWhite);
    }
    EXPECT_EQ(strip1_data, strip1_data_expected);
    EXPECT_EQ(strip2_data, strip2_data_expected);
    EXPECT_TRUE(strip1.GetDidShow());
}

TEST_F(StatusLedsTest, SetBrightness_BrighnessSet)
{
    constexpr uint8_t kBrightness = 42U;
    led_manager.SetBrightness(kBrightness);
    EXPECT_EQ(strip1.GetBrightness(), kBrightness);  // Strip1 is the presenter
}

TEST_F(StatusLedsTest, SetLeds_StatusLedsClearedButNotLatched)
{
    // GIVEN
    led_manager.SetStatusLed(LedColor::kBlue);

    // WHEN
    strip1.ResetDidShow();
    constexpr uint32_t kIdStrip1 = 0 << 8U;
    constexpr uint32_t kIdStrip2 = 1 << 8U;
    const std::array<Led, 7> new_leds{
        Led(kIdStrip1 | 0, 42),
        Led(kIdStrip2 | 1, 1),
        Led(kIdStrip2 | 5, 5),
        Led(kIdStrip1 | 2, 7),
        Led(kIdStrip2 | 0, 8),
        Led(kIdStrip2 | 6, 9),
        Led(kIdStrip1 | 7, 10),
    };
    led_manager.SetLeds(new_leds.data(), new_leds.size());

    // THEN
    const auto strip1_data = strip1.GetData();
    const auto strip2_data = strip2.GetData();
    const std::array<uint32_t, kStripLength> strip_data_expected{0};
    EXPECT_EQ(strip1_data, strip_data_expected);
    EXPECT_EQ(strip2_data, strip_data_expected);
    EXPECT_FALSE(strip1.GetDidShow());
}
