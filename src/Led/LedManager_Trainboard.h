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
#ifndef LED_MANAGER_TRAINBOARD_H_
#define LED_MANAGER_TRAINBOARD_H_

#include "Led.h"
#include "LedManager.h"
#include "LedPresenter.h"
#include "LedStrip.h"
#include "Logging.h"

#include <functional>
#include "etl/algorithm.h"
#include "etl/array.h"
#include "etl/vector.h"

template<size_t N>
class TrainboardLedManager : public LedManager
{
  public:
    using Strip = std::reference_wrapper<LedStrip>;

    TrainboardLedManager(const etl::array<Strip, N>& strips, LedPresenter& presenter, uint32_t transition_duration)
        : transition_duration_(transition_duration), half_transition_duration_(transition_duration / 2), strips_(strips), presenter_(presenter) {}

    void Init()
    {
        for (auto& strip : strips_)
        {
            strip.get().Init();
        }
    }

    bool SetStatusLed(LedColor color) override
    {
        if (!is_transitioning_)
        {
            ClearAllLeds();
            status_led_color_ = color;
            for (auto& strip : strips_)
            {
                strip.get().Set(0U, static_cast<uint32_t>(color), UINT8_MAX);
            }
            presenter_.Show();
        }
        return !is_transitioning_;
    }

    bool SetTestLeds() override
    {
        if (!is_transitioning_)
        {
            for (auto& strip : strips_)
            {
                strip.get().Test();
            }
        }
        return !is_transitioning_;
    }

    void SetLeds(const Led* const leds, uint32_t leds_length) override
    {
        ASSERT(nullptr != leds);

        ClearStatusLeds();
        ResetTransition();
        is_transitioning_ = true;
        GetNewActiveLeds(leds, leds_length);
        AssignLedsToSwapOrFadeIn(leds_new_);
        AssignLedsToFadeOut(leds_new_);
        AssignNewActiveLeds(leds_new_);
    }

    void ClearAllLeds() override
    {
        for (auto& strip : strips_)
        {
            strip.get().ClearAll();
        }
        presenter_.Show();
        ResetTransition();
        leds_active_.clear();
    }

    bool RefreshTransition() override
    {
        bool is_finished = false;
        transition_cnt_++;
        if (transition_cnt_ >= transition_duration_)
        {
            FadeLedsInOut(UINT8_MAX);
            is_finished = true;
            is_transitioning_ = false;
        }
        else
        {
            if (transition_cnt_ < half_transition_duration_)
            {
                FadeOutLedsToSwap();
            }
            else
            {
                const auto scaling_in_u32 = (static_cast<uint32_t>(UINT8_MAX) * (transition_cnt_ - half_transition_duration_)) / half_transition_duration_;
                const auto scaling_in = static_cast<uint8_t>(etl::clamp(scaling_in_u32, 0U, static_cast<uint32_t>(UINT8_MAX)));
                FadeLedsInOut(scaling_in);
            }
        }
        presenter_.Show();
        return is_finished;
    }

    void SetBrightness(uint8_t brightness) override
    {
        presenter_.SetBrightness(brightness);
    }

  private:
    const uint32_t transition_duration_;
    const uint32_t half_transition_duration_;
    uint32_t transition_cnt_{0};
    bool is_transitioning_{false};
    LedColor status_led_color_{LedColor::kBlack};

    const etl::array<Strip, N>& strips_;
    LedPresenter& presenter_;

    static constexpr uint32_t kMaxLeds = 512U;
    using LedVector = etl::vector<Led, kMaxLeds>;
    LedVector leds_new_ = LedVector();
    LedVector leds_active_ = LedVector();
    LedVector leds_in_ = LedVector();    // fade in on second half of transition
    LedVector leds_out_ = LedVector();   // fade out on second half of transition
    LedVector leds_swap_ = LedVector();  // fade out on first half of transition

    void ClearStatusLeds()
    {
        if (LedColor::kBlack != status_led_color_)
        {
            for (auto& strip : strips_)
            {
                strip.get().Set(0, 0, 0);  // Clear the first Led of the strip
            }
            status_led_color_ = LedColor::kBlack;
        }
        else
        {
            // Status LEDs not set, nothing to clear. Otherwise we might
            // clear some train LED...
        }
    }

    void ResetTransition()
    {
        transition_cnt_ = 0U;
        is_transitioning_ = false;
        leds_out_.clear();
        leds_swap_.clear();
        leds_in_.clear();
    }

    void GetNewActiveLeds(const Led* const leds, const uint32_t leds_length)
    {
        leds_new_.clear();
        for (auto i = 0U; i < leds_length; i++)
        {
            const auto led = leds[i];
            const auto strip_id = led.GetStripId();
            if (strip_id < N)
            {
                const auto position = led.GetPosition();
                if (position < strips_[strip_id].get().GetSize())
                {
                    leds_new_.push_back(led);
                }
                else
                {
                    // Ignore invalid index
                }
            }
            else
            {
                // Ignore invalid strip ids
            }
        }
    }

    void AssignLedsToSwapOrFadeIn(const LedVector& new_active_leds)
    {
        for (const auto& led : new_active_leds)
        {
            const auto changes_color = [led](const Led& active_led) {
                const auto has_same_id = led.GetId() == active_led.GetId();
                const auto has_different_color = led.GetColor() != active_led.GetColor();
                return has_same_id && has_different_color;
            };
            auto it = etl::find_if(leds_active_.begin(), leds_active_.end(), changes_color);
            if (it != leds_active_.end())
            {
                // New led on same position but different color
                leds_swap_.push_back(*it);  // old led to fade out
                leds_in_.push_back(led);    // new led to fade in
            }
            else
            {
                if (etl::find(leds_active_.begin(), leds_active_.end(), led) == leds_active_.end())
                {
                    // New led to fade in
                    leds_in_.push_back(led);
                }
            }
        }
    }

    void AssignLedsToFadeOut(const LedVector& new_active_leds)
    {
        for (const auto& active_led : leds_active_)
        {
            if (etl::find(new_active_leds.begin(), new_active_leds.end(), active_led) == new_active_leds.end())
            {
                leds_out_.push_back(active_led);
            }
        }
    }

    void AssignNewActiveLeds(const LedVector& new_active_leds)
    {
        leds_active_ = new_active_leds;
    }

    void FadeOutLedsToSwap()
    {
        const auto scaling_u32 = 2 * transition_cnt_ * static_cast<uint32_t>(UINT8_MAX) / transition_duration_;
        const auto scaling = static_cast<uint8_t>(etl::clamp(scaling_u32, 0U, static_cast<uint32_t>(UINT8_MAX)));
        const uint8_t scaling_out = UINT8_MAX - scaling;
        for (const auto& led : leds_swap_)
        {
            strips_[led.GetStripId()].get().Set(led.GetPosition(), led.GetColor(), scaling_out);
        }
    }

    void FadeLedsInOut(uint8_t scaling_in)
    {
        for (const auto& led : leds_out_)
        {
            const uint8_t scaling_out = UINT8_MAX - scaling_in;
            strips_[led.GetStripId()].get().Set(led.GetPosition(), led.GetColor(), scaling_out);
        }
        for (const auto& led : leds_in_)
        {
            strips_[led.GetStripId()].get().Set(led.GetPosition(), led.GetColor(), scaling_in);
        }
    }
};

#endif  // LED_MANAGER_TRAINBOARD_H_
