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

#include "TestingFsm.h"

EventWithAction transition_a{nullptr, "A; "};
EventWithAction transition_b{&state_1, "B; "};
EventWithAction transition_c21{&state_1, "C; "};
EventWithAction transition_c12{&state_2, "C; "};
EventWithAction transition_d13{&state_3, "D; "};
EventWithAction transition_d31{&state_1, "D; "};
EventWithAction fault_transition{nullptr, "X; "};

Fsm my_fsm{state_1};
S1 state_1;
S2 state_2;
S3 state_3{my_fsm, kFaulty};
std::string sequence{};
