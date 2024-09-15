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

#ifndef LOGGING_H_
#define LOGGING_H_

#ifdef _WIN32
#include <assert.h>
#include <iostream>
#include <source_location>
static inline void ASSERT(bool condition, std::source_location location = std::source_location::current())
{
    if (!condition)
    {
        std::cout << "Assertion failed in file " << location.file_name() << ", line " << location.line() << std::endl;
        std::cerr << "ASSERT" << std::endl;
    }
    assert(condition);
}
static inline void LOG_ERROR(const char* msg, std::source_location location = std::source_location::current())
{
    std::cout << "[ERROR] : " << msg << " in file " << location.file_name() << ", line " << location.line() << std::endl;
}
static inline void LOG_WARN(const char* msg, std::source_location location = std::source_location::current())
{
    std::cout << "\033[93m"
              << "[WARNING] : " << msg << " in file " << location.file_name() << ", line " << location.line() << "\033[0m" << std::endl;
}
static inline void LOG_INFO(const char* msg, std::source_location location = std::source_location::current())
{
    std::cout << "[INFO]    : " << msg << " in file " << location.file_name() << ", line " << location.line() << std::endl;
}
static inline void LOG_DEBUG(const char* msg)
{
    std::cout << "[DEBUG]   : " << msg << std::endl;
}
#else
#include <Arduino.h>
#define ASSERT(x) assert_func((x), __FILENAME__, __LINE__)
static inline void assert_func(bool condition, const char* filename, const int line)
{
    if (!condition)
    {
        Serial.print("[ASSERT]  : File ");
        Serial.print(filename);
        Serial.print(", line");
        Serial.println(line);
    }
}
static inline void LOG_ERROR(const char* msg)
{
    Serial.print("[ERROR]   : ");
    Serial.println(msg);
}
static inline void LOG_WARN(const char* msg)
{
    Serial.print("[WARN]    : ");
    Serial.println(msg);
}
static inline void LOG_INFO(const char* msg)
{
    Serial.print("[INFO]    : ");
    Serial.println(msg);
}
static inline void LOG_DEBUG(const char* msg)
{
    Serial.print("[DEBUG]   : ");
    Serial.println(msg);
}
#endif

#endif  // LOGGING_H_
