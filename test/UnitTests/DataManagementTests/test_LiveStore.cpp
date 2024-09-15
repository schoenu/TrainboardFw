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

#include "DataManager.h"
#include "FwConfig.h"
#include "Logging.h"

#include <array>

class DataMgrLiveStoreTests : public ::testing::Test
{
  protected:
    DataReader* reader_;
    DataWriter* writer_;
    void SetUp() override
    {
        DataMgr_Reset();
        DataMgr_SetReaderMode(DataReaderMode::kLive);
        DataMgr_SetWriterMode(DataWriterMode::kSingle);
        reader_ = DataMgr_GetReader();
        writer_ = DataMgr_GetWriter();
        ASSERT_NE(reader_, nullptr);
        ASSERT_NE(writer_, nullptr);
    }
};

TEST_F(DataMgrLiveStoreTests, Reader_DataNullptr_ReturnZero)
{
    EXPECT_EQ(reader_->ReadData(nullptr, 42U), 0U);
}

TEST_F(DataMgrLiveStoreTests, Reader_LengthZero_ReturnZero)
{
    uint8_t buffer[42U];
    EXPECT_EQ(reader_->ReadData(buffer, 0U), 0U);
}

TEST_F(DataMgrLiveStoreTests, Writer_DataNullptr_ReturnFalse)
{
    EXPECT_FALSE(writer_->SaveData(nullptr, 42U));
}

TEST_F(DataMgrLiveStoreTests, Writer_LengthZero_ReturnFalse)
{
    uint8_t buffer[42U] = {};
    EXPECT_FALSE(writer_->SaveData(buffer, 0U));
}

TEST_F(DataMgrLiveStoreTests, Writer_LengthTooBig_ReturnFalse)
{
    uint8_t buffer[kBufferSizeInBytes + 1] = {};
    EXPECT_FALSE(writer_->SaveData(buffer, kBufferSizeInBytes + 1));
}

TEST_F(DataMgrLiveStoreTests, WriteDataOnce_ReadBack_Data)
{
    constexpr size_t kDataLength = 5U;
    const std::array<uint8_t, kDataLength> data = {1, 2, 3, 4, 5};
    EXPECT_TRUE(writer_->SaveData(data.data(), data.size()));

    std::array<uint8_t, kDataLength> data_read = {};
    auto length_read = reader_->ReadData(data_read.data(), data_read.size());
    EXPECT_EQ(length_read, kDataLength);
    EXPECT_TRUE(data_read == data);
}

TEST_F(DataMgrLiveStoreTests, WriteDataTwice_ReadOnce_LastDataReturned)
{
    constexpr size_t kDataLength = 5U;
    const std::array<uint8_t, kDataLength> data1 = {1, 2, 3, 4, 5};
    const std::array<uint8_t, kDataLength> data2 = {6, 7, 8, 9, 10};
    EXPECT_TRUE(writer_->SaveData(data1.data(), data1.size()));
    EXPECT_TRUE(writer_->SaveData(data2.data(), data2.size()));

    std::array<uint8_t, kDataLength> data_read = {};
    auto length_read = reader_->ReadData(data_read.data(), data_read.size());
    EXPECT_EQ(length_read, kDataLength);
    EXPECT_TRUE(data_read == data2);
}

TEST_F(DataMgrLiveStoreTests, WriteDataTwice_ReadTwice_LastDataReturnedTwice)
{
    constexpr size_t kDataLength = 5U;
    const std::array<uint8_t, kDataLength> data1 = {1, 2, 3, 4, 5};
    const std::array<uint8_t, kDataLength> data2 = {6, 7, 8, 9, 10};
    EXPECT_TRUE(writer_->SaveData(data1.data(), data1.size()));
    EXPECT_TRUE(writer_->SaveData(data2.data(), data2.size()));

    std::array<uint8_t, kDataLength> data_read = {};
    (void)reader_->ReadData(data_read.data(), data_read.size());
    auto length_read = reader_->ReadData(data_read.data(), data_read.size());
    EXPECT_EQ(length_read, kDataLength);
    EXPECT_TRUE(data_read == data2);
}

TEST_F(DataMgrLiveStoreTests, WriteData70time_Read_LastDataReturned)
{
    constexpr size_t kDataLength = 2U;
    std::array<uint8_t, kDataLength> data{};
    for (uint8_t i = 0; i < kNumberOfHistoryFrames; i++)
    {
        data[0] = i;
        data[1] = 2 * i;
        EXPECT_TRUE(writer_->SaveData(data.data(), data.size()));
    }

    std::array<uint8_t, kDataLength> data_read = {};
    auto length_read = reader_->ReadData(data_read.data(), data_read.size());
    EXPECT_EQ(length_read, kDataLength);
    EXPECT_TRUE(data_read == data);
}

TEST_F(DataMgrLiveStoreTests, WriteDataMoreThan70time_Read_LastDataReturned)
{
    constexpr size_t kDataLength = 2U;
    std::array<uint8_t, kDataLength> data{};
    for (uint8_t i = 0; i < kNumberOfHistoryFrames + 2; i++)
    {
        data[0] = i;
        data[1] = 2 * i;
        EXPECT_TRUE(writer_->SaveData(data.data(), data.size()));
    }

    std::array<uint8_t, kDataLength> data_read = {};
    auto length_read = reader_->ReadData(data_read.data(), data_read.size());
    EXPECT_EQ(length_read, kDataLength);
    EXPECT_TRUE(data_read == data);
}

TEST_F(DataMgrLiveStoreTests, ReadWithMaxLengthLessThanFrameSize_ReturnZero)
{
    constexpr size_t kDataLength = 5U;
    const std::array<uint8_t, kDataLength> data = {1, 2, 3, 4, 5};
    EXPECT_TRUE(writer_->SaveData(data.data(), data.size()));

    std::array<uint8_t, kDataLength> data_read = {};
    const auto length_read = reader_->ReadData(data_read.data(), data_read.size() - 1);
    EXPECT_EQ(0, length_read);
}
