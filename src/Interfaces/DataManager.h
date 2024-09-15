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

#ifndef DATA_MANAGER_H_
#define DATA_MANAGER_H_

#include <cstdint>

enum class DataReaderMode
{
    kLive,     /// Read last written framehistory frame
    kHistory,  /// Read next history frametory frames
    kOffline   /// Read next offline frame
};

enum class DataWriterMode
{
    kSingle,   /// Overwrite oldest history frame
    kMultiple  /// Overwrite all history frames
};

/// @brief Abstract class representing an object to write one or multiple frames to memory
class DataWriter
{
  public:
    /// @brief Save data to memory
    /// @return
    /// `false` if data  is invalid or does not fit into a frame, `true` otherwise
    virtual bool SaveData(const uint8_t* const data, const uint32_t data_length) = 0;
    virtual ~DataWriter() = default;
};

/// @brief Abstract class representing an object to read a frame from memory
class DataReader
{
  public:
    /// @brief Read data from memory/
    /// @param data Pointer to a buffer in which the data read from memory
    /// will be copied to.
    /// @param data_length Maximum data length to be copied to the `data` buffer
    /// @return
    /// 0 if input parameters are invalid or data does not fit into provided buffer
    /// number of written bytes otherwise
    virtual uint32_t ReadData(uint8_t* const data, const uint32_t max_length) = 0;
    virtual ~DataReader() = default;
};

/// @brief Reset the data mode to `kLive`, clear history data, and reset history
/// frame pointer to the first history frame.
void DataMgr_Reset();

/// @brief Change mode for reading of data
void DataMgr_SetReaderMode(DataReaderMode mode);

/// @brief Get actual data mode for reading from store
DataReaderMode DataMgr_GetReaderMode();

/// @brief Change mode for writing of data
void DataMgr_SetWriterMode(DataWriterMode mode);

/// @brief Get actual data mode for writing to store
DataWriterMode DataMgr_GetWriterMode();

/// @brief Get a writer object
/// @return
/// `nullptr` if writer not available for the actual data mode
/// Pointer to a data writer object for the actual data mode
DataWriter* DataMgr_GetWriter();

/// @brief Get a reader object
/// @return
/// Pointer to a data reader object for the actual data mode
DataReader* DataMgr_GetReader();

#endif  // DATA_MANAGER_H_
