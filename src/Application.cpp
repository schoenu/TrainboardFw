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

#include "BoardConfiguration.h"
#include "ConnectionListener.h"
#include "FastLedPresenter.h"
#include "FastLedStrip.h"
#include "FwConfig.h"
#include "LedManager_Trainboard.h"
#include "LightSensor.h"
#include "LightSensorLtr303.h"
#include "PushButton.h"
#include "TimerTicker.h"
#include "Trainboard.h"

using Manager = TrainboardLedManager<kNumberOfStrips>;
using StripArray = etl::array<Manager::Strip, kNumberOfStrips>;

// LEDs V1
// Strip drivers <Chipset, #leds on strip, Pin number>
static FastLedStrip<WS2812B, kV1Strip1NLeds, 13> v1_strip1{};
static FastLedStrip<WS2812B, kV1Strip2NLeds, 12> v1_strip2{};
static FastLedStrip<WS2812B, kV1Strip3NLeds, 11> v1_strip3{};
static FastLedStrip<WS2812B, kV1Strip4NLeds, 10> v1_strip4{};
StripArray strips_v1_{v1_strip1, v1_strip2, v1_strip3, v1_strip4};

// LEDs V1_1
// Strip drivers <Chipset, #leds on strip, Pin number>
static FastLedStrip<WS2812B, kV11Strip1NLeds, 13> v1_1_strip1{};
static FastLedStrip<WS2812B, kV11Strip2NLeds, 12> v1_1_strip2{};
static FastLedStrip<WS2812B, kV11Strip3NLeds, 11> v1_1_strip3{};
static FastLedStrip<WS2812B, kV11Strip4NLeds, 10> v1_1_strip4{};
StripArray strips_v1_1_{v1_1_strip1, v1_1_strip2, v1_1_strip3, v1_1_strip4};

// LEDs V1_2
// Strip drivers <Chipset, #leds on strip, Pin number>
static FastLedStrip<WS2812B, kV12Strip1NLeds, 13> v1_2_strip1{};
static FastLedStrip<WS2812B, kV12Strip2NLeds, 12> v1_2_strip2{};
static FastLedStrip<WS2812B, kV12Strip3NLeds, 11> v1_2_strip3{};
static FastLedStrip<WS2812B, kV12Strip4NLeds, 10> v1_2_strip4{};
StripArray strips_v1_2_{v1_2_strip1, v1_2_strip2, v1_2_strip3, v1_2_strip4};

static FastLedPresenter fast_led_presenter_;

static EventQueue _event_queue;
static ConnectionListener connection_listener_(_event_queue);
static PushButton push_button_{kButtonPinNr, _event_queue};
static Manager* p_manager_{nullptr};
static Trainboard* p_trainboard_{nullptr};

static inline bool InitLedManagerAndStatemachine()
{
    const auto hw_version = BoardConfig::Get().GetHwVersion();
    auto is_hw_version_valid = true;
    StripArray* strips_{nullptr};
    switch (hw_version)
    {
        case BoardConfig::HardwareVersion::kV1:
            strips_ = &strips_v1_;
            break;

        case BoardConfig::HardwareVersion::kV1_1:
            strips_ = &strips_v1_1_;
            break;

        case BoardConfig::HardwareVersion::kV1_2:
            strips_ = &strips_v1_2_;
            break;
        default:
            is_hw_version_valid = false;
            break;
    }

    if (is_hw_version_valid)
    {
        // As we must assign the strips at runtime in the constructor, manager and
        // train board must be initialized here.
        static Manager led_manager_{*strips_, fast_led_presenter_, kTransitionDurationInTicks};
        static Trainboard trainboard_{_event_queue, led_manager_};

        p_manager_ = &led_manager_;
        p_trainboard_ = &trainboard_;
    }
    return is_hw_version_valid;
}

EventQueue* Application_GetEventQueue()
{
    return &_event_queue;
}

bool Application_Init()
{
    BoardConfig::Get().ReadHwVersion();

    const auto could_get_objects = InitLedManagerAndStatemachine();
    if (could_get_objects)
    {
        ASSERT(nullptr != p_manager_);
        ASSERT(nullptr != p_trainboard_);

        [[maybe_unused]] auto _ = TimerTicker_Start(kTickPeriodMilliSeconds);
        LightSensorLtr303_Init();
        push_button_.Init();
        p_manager_->Init();
        delay(400);  // Let FastLED initialize...
        fast_led_presenter_.Init();
        p_trainboard_->Init();
    }
    else
    {
        LOG_ERROR("Could not find valid hw version!");
    }
    return could_get_objects;
}

void Application_Dispatch(uint16_t event)
{
    connection_listener_.Dispatch(event);
    p_trainboard_->DispatchEvent(event);
    push_button_.Dispatch(event);
    LightSensorLtr393_Dispatch(event);
}