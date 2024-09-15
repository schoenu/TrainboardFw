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

#include "TestingFsm.h"

class TestFsm : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        my_fsm.Init();
        sequence.clear();
    }
};

TEST_F(TestFsm, InitSequence)
{
    sequence.clear();
    Fsm dummy_fsm{state_1};
    dummy_fsm.Init();
    EXPECT_EQ(sequence, "init; s1-e; ");
}

TEST_F(TestFsm, InternalTransition)
{
    my_fsm.Dispatch(kA);
    EXPECT_EQ(sequence, "A; ");
}

TEST_F(TestFsm, TransitionToSelf)
{
    my_fsm.Dispatch(kB);
    EXPECT_EQ(sequence, "s1-x; B; s1-e; ");
}

TEST_F(TestFsm, TransitionToOtherState)
{
    my_fsm.Dispatch(kC);
    EXPECT_EQ(sequence, "s1-x; C; s2-e; ");
}

TEST_F(TestFsm, MultipleTransitions)
{
    my_fsm.Dispatch(kC);
    my_fsm.Dispatch(kC);
    EXPECT_EQ(sequence, "s1-x; C; s2-e; s2-x; C; s1-e; ");
}

TEST_F(TestFsm, NoDispatchFromEntry)
{
    my_fsm.Dispatch(kD);
    EXPECT_EQ(state_3.GetStatus(), Fsm::Status::kBusy);
    EXPECT_EQ(sequence, "s1-x; D; s3-e; ");
}

TEST_F(TestFsm, NoDispatchFromEventHandler)
{
    my_fsm.Dispatch(kD);
    state_3.ResetStatus();
    my_fsm.Dispatch(kFaulty);
    EXPECT_EQ(state_3.GetStatus(), Fsm::Status::kBusy);
    EXPECT_EQ(sequence, "s1-x; D; s3-e; X; ");
}

TEST_F(TestFsm, NoDispatchFromExit)
{
    my_fsm.Dispatch(kD);
    state_3.ResetStatus();
    my_fsm.Dispatch(kD);
    EXPECT_EQ(state_3.GetStatus(), Fsm::Status::kBusy);
    EXPECT_EQ(sequence, "s1-x; D; s3-e; s3-x; D; s1-e; ");
}
