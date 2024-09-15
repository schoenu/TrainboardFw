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

#include "FwConfig.h"

TEST(IsDataValidTest, NullPtrBuffer_ReturnsFalse)
{
    EXPECT_FALSE(DataConv_IsDataValid(nullptr, 42U));
}

TEST(IsDataValidTest, ZeroLength_ReturnsFalse)
{
    uint8_t buffer[42U];
    EXPECT_FALSE(DataConv_IsDataValid(buffer, 0U));
}

TEST(IsDataValidTest, LengthGreaterThanMaxBufferSize_ReturnsFalse)
{
    uint8_t buffer[42U];
    EXPECT_FALSE(DataConv_IsDataValid(buffer, kBufferSizeInBytes + 1U));
}

TEST(IsDataValidTest, LengthDoesNotMatchNumberOfLedsInSecondByte_ReturnsFalse)
{
    constexpr size_t kBufferSize = 42U;
    uint8_t buffer[kBufferSize];
    buffer[1] = 0xFF;
    EXPECT_FALSE(DataConv_IsDataValid(buffer, kBufferSize));
}

TEST(IsDataValidTest, ValidDataAndLength_ReturnsTrue)
{
    uint8_t buffer[] = {0, 49, 2, 51, 255, 255, 255, 2, 8, 255, 255, 255, 2, 30, 255, 255, 255,
                        0, 11, 255, 255, 255, 3, 78, 255, 255, 255, 2, 48, 255, 255, 255, 3, 54,
                        171, 140, 34, 3, 57, 171, 140, 34, 3, 0, 255, 0, 0, 0, 8, 255, 0, 0, 1,
                        10, 255, 0, 0, 3, 66, 255, 0, 0, 1, 32, 255, 0, 0, 3, 66, 255, 0, 0, 1,
                        8, 255, 0, 0, 0, 9, 255, 0, 0, 2, 45, 252, 3, 248, 2, 44, 252, 3, 248, 2,
                        59, 254, 199, 11, 2, 61, 254, 199, 11, 2, 33, 254, 199, 11, 3, 32, 0, 152,
                        74, 3, 29, 0, 152, 74, 1, 34, 255, 128, 0, 3, 83, 255, 128, 0, 0, 69, 255,
                        128, 0, 0, 51, 255, 128, 0, 0, 53, 255, 128, 0, 0, 79, 255, 128, 0, 0,
                        81, 255, 128, 0, 3, 1, 255, 128, 0, 0, 78, 255, 0, 0, 2, 2, 255, 0, 0, 1,
                        61, 140, 198, 62, 1, 32, 252, 3, 248, 1, 33, 252, 3, 248, 3, 11, 0, 138,
                        198, 1, 21, 0, 138, 198, 3, 10, 255, 255, 255, 0, 41, 0, 0, 255, 0, 13,
                        0, 0, 255, 0, 30, 0, 0, 255, 0, 15, 0, 0, 255, 0, 18, 0, 0, 255, 0, 4, 0,
                        0, 255, 0, 20, 0, 0, 255, 0, 48, 0, 0, 255, 0, 38, 0, 0, 255, 0, 26, 0, 0, 255};
    const auto data_length = 2 + buffer[1] * 5;
    EXPECT_TRUE(DataConv_IsDataValid(buffer, data_length));
}

//  TODO : check for OP Codes: JAL, BRANCH, LOAD
