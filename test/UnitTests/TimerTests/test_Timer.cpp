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

#include "Timer.h"

class TestTimer : public ::testing::Test
{
  protected:
    static constexpr uint32_t kTickPeriodMilliSecond = 2U;
    Timer timer{kTickPeriodMilliSecond};
    void SetUp() override
    {
        timer.Init();
    }
    void TearDown() override
    {
    }
    void StartRunning()
    {
        EXPECT_TRUE(timer.StartOneShot(42U));
        EXPECT_TRUE(timer.IsRunning());
        EXPECT_FALSE(timer.HasExpired());
    }
    void Expire()
    {
        constexpr uint32_t kDuration = 42U;
        EXPECT_TRUE(timer.StartOneShot(kDuration));
        for (auto i = 0U; i < kDuration / kTickPeriodMilliSecond; i++)
        {
            timer.Tick();
        }
        EXPECT_EQ(timer.GetElapsedMilliSeconds(), kDuration);
        EXPECT_TRUE(timer.HasExpired());
        EXPECT_FALSE(timer.IsRunning());
    }
};

TEST_F(TestTimer, InitiallyNotRunningAndNotExpired)
{
    EXPECT_FALSE(timer.IsRunning());
    EXPECT_FALSE(timer.HasExpired());
}

TEST_F(TestTimer, AfterInit_StartOneShot_ReturnsTrueAndRunningAndNotExpired)
{
    EXPECT_TRUE(timer.StartOneShot(42U));
    EXPECT_TRUE(timer.IsRunning());
    EXPECT_FALSE(timer.HasExpired());
}

TEST_F(TestTimer, Stopped_RestartNotPossible)
{
    EXPECT_FALSE(timer.Restart());
}

TEST_F(TestTimer, Stopped_ResetPossible)
{
    EXPECT_TRUE(timer.Reset());
}

TEST_F(TestTimer, StartWithDurationZero_ImmediatelyExpires)
{
    EXPECT_TRUE(timer.StartOneShot(0U));
    EXPECT_FALSE(timer.IsRunning());
    EXPECT_TRUE(timer.HasExpired());
}

TEST_F(TestTimer, Stopped_TickHasNoEffect)
{
    timer.Tick();
    EXPECT_EQ(timer.GetElapsedMilliSeconds(), 0U);
}

TEST_F(TestTimer, Stopped_HaltNotPossible)
{
    EXPECT_FALSE(timer.Halt());
}

TEST_F(TestTimer, Started_StartOneShot_RunningAndNotExpired)
{
    StartRunning();

    EXPECT_FALSE(timer.StartOneShot(42U));
    EXPECT_TRUE(timer.IsRunning());
    EXPECT_FALSE(timer.HasExpired());
}

TEST_F(TestTimer, Started_Stop_NotRunningNotExpired)
{
    StartRunning();
    EXPECT_TRUE(timer.Stop());
    EXPECT_FALSE(timer.IsRunning());
    EXPECT_FALSE(timer.HasExpired());
}

TEST_F(TestTimer, Started_DurationDone_ExpiredElapsedMillisecondsRight)
{
    Expire();
}

TEST_F(TestTimer, Started_Restart_RestartCountingFromZero)
{
    constexpr uint32_t kDuration = 42U;
    constexpr uint32_t kFirstRunDuration = 23U;

    // Make it run a bit
    EXPECT_TRUE(timer.StartOneShot(kDuration));
    for (auto i = 0; i < kFirstRunDuration / kTickPeriodMilliSecond; i++)
    {
        timer.Tick();
        EXPECT_FALSE(timer.HasExpired());
    }

    // Restart the timer
    EXPECT_TRUE(timer.Restart());

    // Make sure it does not expire before duration is done
    for (auto i = 0; i < kDuration / kTickPeriodMilliSecond - 1; i++)
    {
        timer.Tick();
        EXPECT_FALSE(timer.HasExpired());
    }

    // Expire on last tick
    timer.Tick();
    EXPECT_TRUE(timer.HasExpired());
}

TEST_F(TestTimer, Running_Halt_NotRunningNotExpiredTickHasNoEffect)
{
    constexpr uint32_t kDuration = 42U;
    constexpr uint32_t kFirstRunDuration = 23U;

    // GIVEN
    EXPECT_TRUE(timer.StartOneShot(kDuration));
    for (auto i = 0U; i < kFirstRunDuration / kTickPeriodMilliSecond; i++)
    {
        timer.Tick();
        EXPECT_TRUE(timer.IsRunning());
        EXPECT_FALSE(timer.HasExpired());
    }

    // WHEN
    EXPECT_TRUE(timer.Halt());

    // THEN
    EXPECT_FALSE(timer.IsRunning());
    EXPECT_FALSE(timer.HasExpired());
    const auto elapsed_time = timer.GetElapsedMilliSeconds();
    for (auto i = 0U; i < kDuration / kTickPeriodMilliSecond; i++)
    {
        timer.Tick();
        EXPECT_FALSE(timer.HasExpired());
        EXPECT_EQ(timer.GetElapsedMilliSeconds(), elapsed_time);
    }
}

TEST_F(TestTimer, Running_ResetNoPossibleAndExpiresAtDuration)
{
    constexpr uint32_t kDuration = 42U;
    EXPECT_TRUE(timer.StartOneShot(kDuration));

    for (auto i = 0U; i < kDuration / kTickPeriodMilliSecond - 1; i++)
    {
        timer.Tick();
        EXPECT_FALSE(timer.Reset());
        EXPECT_TRUE(timer.IsRunning());
        EXPECT_FALSE(timer.HasExpired());
    }

    timer.Tick();
    EXPECT_FALSE(timer.IsRunning());
    EXPECT_TRUE(timer.HasExpired());
}

TEST_F(TestTimer, Running_Tick_ElapsedTimeIncremented)
{
    StartRunning();
    EXPECT_EQ(timer.GetElapsedMilliSeconds(), 0U);
    timer.Tick();
    EXPECT_EQ(timer.GetElapsedMilliSeconds(), 1U * kTickPeriodMilliSecond);
}

TEST_F(TestTimer, Halted_Stop_CounterReset)
{
    StartRunning();
    timer.Tick();
    EXPECT_TRUE(timer.Halt());
    EXPECT_TRUE(timer.Stop());
    EXPECT_FALSE(timer.IsRunning());
    EXPECT_FALSE(timer.HasExpired());
    EXPECT_EQ(timer.GetElapsedMilliSeconds(), 0U);
}

TEST_F(TestTimer, Halted_Halt_NotPossible)
{
    StartRunning();
    EXPECT_TRUE(timer.Halt());
    EXPECT_FALSE(timer.Halt());
}

TEST_F(TestTimer, Halted_Reset_NotPossible)
{
    StartRunning();
    EXPECT_TRUE(timer.Halt());
    EXPECT_FALSE(timer.Reset());
}

TEST_F(TestTimer, Halted_Restart_RestartCountingFromZero)
{
    constexpr uint32_t kDuration = 42U;
    constexpr uint32_t kFirstRunDuration = 23U;

    // Make it run a bit
    EXPECT_TRUE(timer.StartOneShot(kDuration));
    for (auto i = 0; i < kFirstRunDuration / kTickPeriodMilliSecond; i++)
    {
        timer.Tick();
        EXPECT_FALSE(timer.HasExpired());
    }

    // Halt the timer
    EXPECT_TRUE(timer.Halt());

    // Restart the timer
    EXPECT_TRUE(timer.Restart());

    // Make sure it does not expire before duration is done
    for (auto i = 0; i < kDuration / kTickPeriodMilliSecond - 1; i++)
    {
        timer.Tick();
        EXPECT_FALSE(timer.HasExpired());
    }

    // Expire on last tick
    timer.Tick();
    EXPECT_TRUE(timer.HasExpired());
}

TEST_F(TestTimer, Halted_Continue_TickFromWhereWeWere)
{
    constexpr uint32_t kDuration = 42U;
    constexpr uint32_t kFirstRunDuration = 22U;

    // GIVEN
    EXPECT_TRUE(timer.StartOneShot(kDuration));
    for (auto i = 0U; i < kFirstRunDuration / kTickPeriodMilliSecond; i++)
    {
        timer.Tick();
        EXPECT_TRUE(timer.IsRunning());
        EXPECT_FALSE(timer.HasExpired());
    }
    EXPECT_TRUE(timer.Halt());

    // WHEN
    EXPECT_TRUE(timer.Continue());

    // THEN
    for (auto i = 0U; i < ((kDuration - kFirstRunDuration) / kTickPeriodMilliSecond - 1); i++)
    {
        timer.Tick();
        EXPECT_TRUE(timer.IsRunning());
        EXPECT_FALSE(timer.HasExpired());
    }
    timer.Tick();
    EXPECT_FALSE(timer.IsRunning());
    EXPECT_TRUE(timer.HasExpired());
}

TEST_F(TestTimer, Halted_StartOneShot_NotPossible)
{
    StartRunning();
    timer.Tick();
    EXPECT_TRUE(timer.Halt());
    EXPECT_FALSE(timer.StartOneShot(42U));
}

TEST_F(TestTimer, Expired_NotRunningAndExpired)
{
    EXPECT_TRUE(timer.StartOneShot(0U));
    EXPECT_FALSE(timer.IsRunning());
    EXPECT_TRUE(timer.HasExpired());
}

TEST_F(TestTimer, Expired_Stop_NoEffectStillExpired)
{
    EXPECT_TRUE(timer.StartOneShot(0U));
    EXPECT_FALSE(timer.Stop());
    EXPECT_TRUE(timer.HasExpired());
}

TEST_F(TestTimer, Expired_Reset_NotRunningStoppedElapsedTimeZero)
{
    Expire();
    EXPECT_TRUE(timer.Reset());
    EXPECT_FALSE(timer.HasExpired());
    EXPECT_FALSE(timer.IsRunning());
    EXPECT_EQ(timer.GetElapsedMilliSeconds(), 0U);
}

TEST_F(TestTimer, Expired_StartOneShot_NotPossible)
{
    EXPECT_TRUE(timer.StartOneShot(0U));
    EXPECT_FALSE(timer.StartOneShot(42U));
}

TEST_F(TestTimer, Expired_Restart_Running)
{
    Expire();
    EXPECT_TRUE(timer.Restart());
    timer.Tick();
    EXPECT_EQ(timer.GetElapsedMilliSeconds(), 1U * kTickPeriodMilliSecond);
}

TEST_F(TestTimer, Expired_Halt_NotPossible)
{
    Expire();
    EXPECT_FALSE(timer.Halt());
}

#ifndef DAMN_ANTIVIRUS
TEST_F(TestTimer, AssertNonZeroTickPeriod)
{
    ASSERT_DEATH({
        Timer my_faulty_timer{0U};

        // THEN
        // dies
    },
                 "ASSERT");
}
#endif
