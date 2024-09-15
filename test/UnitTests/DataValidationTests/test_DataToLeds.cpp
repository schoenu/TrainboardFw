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

#include "DataConverter.h"
#include "Led.h"

#include "etl/array.h"

class DataToLedsTest : public ::testing::Test
{
  protected:
    static constexpr size_t kNumberOfBytesReceived = 292U;
    uint8_t buffer[kNumberOfBytesReceived] = {0, 58, 3, 64, 255, 255, 255, 2, 58, 255, 255, 255, 1, 62, 255, 255, 255, 3, 84, 255,
                                              255, 255, 2, 45, 255, 255, 255, 0, 19, 255, 255, 255, 0, 19, 255, 0, 0, 1, 21, 255,
                                              0, 0, 3, 81, 255, 0, 0, 3, 11, 255, 0, 0, 1, 1, 255, 0, 0, 0, 4, 255, 0, 0, 2, 67,
                                              252, 3, 248, 2, 43, 252, 3, 248, 2, 59, 252, 3, 248, 2, 50, 252, 3, 248, 3, 21, 252,
                                              3, 248, 2, 36, 254, 199, 11, 2, 68, 254, 199, 11, 2, 53, 254, 199, 11, 2, 20, 254,
                                              199, 11, 3, 29, 0, 152, 74, 3, 44, 0, 152, 74, 3, 78, 255, 128, 0, 0, 7, 255, 128, 0,
                                              3, 1, 255, 128, 0, 0, 10, 255, 128, 0, 3, 10, 255, 128, 0, 3, 66, 255, 128, 0, 0, 80,
                                              255, 128, 0, 0, 69, 255, 128, 0, 0, 51, 255, 128, 0, 0, 79, 255, 128, 0, 3, 65, 255,
                                              128, 0, 3, 2, 255, 128, 0, 0, 53, 255, 128, 0, 0, 67, 255, 128, 0, 2, 4, 255, 0, 0, 2,
                                              3, 255, 0, 0, 1, 33, 140, 198, 62, 1, 44, 252, 3, 248, 2, 9, 252, 3, 248, 1, 45, 252,
                                              3, 248, 3, 68, 0, 138, 198, 1, 45, 0, 138, 198, 1, 44, 0, 138, 198, 3, 66, 0, 138, 198,
                                              2, 15, 255, 255, 255, 2, 17, 255, 255, 255, 0, 20, 0, 0, 255, 0, 42, 0, 0, 255, 0, 1,
                                              0, 0, 255, 0, 26, 0, 0, 255, 0, 48, 0, 0, 255, 0, 39, 0, 0, 255, 0, 18, 0, 0, 255, 0,
                                              8, 0, 0, 255, 0, 49, 0, 0, 255};

    static constexpr size_t kNumberOfLeds = 58U;
    const etl::array<Led, kNumberOfLeds> leds = {{
        {832, 16777215, 255},
        {570, 16777215, 255},
        {318, 16777215, 255},
        {852, 16777215, 255},
        {557, 16777215, 255},
        {19, 16777215, 255},
        {19, 16711680, 255},
        {277, 16711680, 255},
        {849, 16711680, 255},
        {779, 16711680, 255},
        {257, 16711680, 255},
        {4, 16711680, 255},
        {579, 16516088, 255},
        {555, 16516088, 255},
        {571, 16516088, 255},
        {562, 16516088, 255},
        {789, 16516088, 255},
        {548, 16697099, 255},
        {580, 16697099, 255},
        {565, 16697099, 255},
        {532, 16697099, 255},
        {797, 38986, 255},
        {812, 38986, 255},
        {846, 16744448, 255},
        {7, 16744448, 255},
        {769, 16744448, 255},
        {10, 16744448, 255},
        {778, 16744448, 255},
        {834, 16744448, 255},
        {80, 16744448, 255},
        {69, 16744448, 255},
        {51, 16744448, 255},
        {79, 16744448, 255},
        {833, 16744448, 255},
        {770, 16744448, 255},
        {53, 16744448, 255},
        {67, 16744448, 255},
        {516, 16711680, 255},
        {515, 16711680, 255},
        {289, 9225790, 255},
        {300, 16516088, 255},
        {521, 16516088, 255},
        {301, 16516088, 255},
        {836, 35526, 255},
        {301, 35526, 255},
        {300, 35526, 255},
        {834, 35526, 255},
        {527, 16777215, 255},
        {529, 16777215, 255},
        {20, 255, 255},
        {42, 255, 255},
        {1, 255, 255},
        {26, 255, 255},
        {48, 255, 255},
        {39, 255, 255},
        {18, 255, 255},
        {8, 255, 255},
        {49, 255, 255},
    }};
};

TEST_F(DataToLedsTest, DataNullptr_Nullopt)
{
    Led led{};
    EXPECT_EQ(DataConv_DataToLeds(nullptr, 42U, &led, 1U), std::nullopt);
}
TEST_F(DataToLedsTest, DataLengthZero_Nullopt)
{
    uint8_t data[42U] = {};
    Led led{};
    EXPECT_EQ(DataConv_DataToLeds(data, 0, &led, 1U), std::nullopt);
}
TEST_F(DataToLedsTest, LedsNullptr_Nullopt)
{
    constexpr size_t kDataLength = 42U;
    uint8_t data[kDataLength] = {};
    EXPECT_EQ(DataConv_DataToLeds(data, kDataLength, nullptr, 1U), std::nullopt);
}
TEST_F(DataToLedsTest, LedLengthZero_Nullopt)
{
    constexpr size_t kDataLength = 42U;
    uint8_t data[kDataLength] = {};
    Led led{};
    EXPECT_EQ(DataConv_DataToLeds(data, kDataLength, &led, 0U), std::nullopt);
}
TEST_F(DataToLedsTest, ConversionSuccessfull_NumberOfLeds)
{
    etl::array<Led, kNumberOfLeds> leds_found = {};
    const auto nr_of_leds_found = DataConv_DataToLeds(buffer, kNumberOfBytesReceived, leds_found.data(), leds_found.size());
    ASSERT_TRUE(nr_of_leds_found.has_value());
    EXPECT_TRUE(leds == leds_found);
    EXPECT_EQ(nr_of_leds_found.value(), kNumberOfLeds);
}
TEST_F(DataToLedsTest, DataLengthTooSmall_Zero)
{
    etl::array<Led, kNumberOfLeds> leds_found = {};
    const auto nr_of_leds_found = DataConv_DataToLeds(buffer, kNumberOfBytesReceived - 1U, leds_found.data(), leds_found.size());
    ASSERT_TRUE(nr_of_leds_found.has_value());
    EXPECT_EQ(nr_of_leds_found.value(), 0U);
}
TEST_F(DataToLedsTest, DataLengthTooBig_Zero)
{
    etl::array<Led, kNumberOfLeds> leds_found = {};
    const auto nr_of_leds_found = DataConv_DataToLeds(buffer, kNumberOfBytesReceived + 1U, leds_found.data(), leds_found.size());
    ASSERT_TRUE(nr_of_leds_found.has_value());
    EXPECT_EQ(nr_of_leds_found.value(), 0U);
}
TEST_F(DataToLedsTest, DataDoesNotFitInMaxLedsOut_Zero)
{
    etl::array<Led, kNumberOfLeds> leds_found = {};
    const auto nr_of_leds_found = DataConv_DataToLeds(buffer, kNumberOfBytesReceived, leds_found.data(), leds_found.size() - 1);
    ASSERT_TRUE(nr_of_leds_found.has_value());
    EXPECT_EQ(nr_of_leds_found.value(), 0U);
}
