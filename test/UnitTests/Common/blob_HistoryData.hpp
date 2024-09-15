#ifndef BLOB_HISTORYDATA_HPP_
#define BLOB_HISTORYDATA_HPP_

#include <array>
#include <cstdint>

#define HISTORY_BUFFER_LENGTH 12500

extern const std::array<uint8_t, HISTORY_BUFFER_LENGTH> history_buffer;

#endif  // BLOB_HISTORYDATA_HPP_
