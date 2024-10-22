// MIT License
//
// Copyright (c) 2024 Arjen P. van Zanten
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef C_MIDI_TEST_COMMON_H
#define C_MIDI_TEST_COMMON_H

#include "message.h"

int32_t  get_rand_i32(int32_t min, int32_t max);
uint32_t get_rand_u32(uint32_t min, uint32_t max);
uint8_t  get_rand_u7(void);
uint16_t get_rand_u14(void);

int16_t      get_rand_pitch_bend_value(void);
MIDI_Note    get_rand_note(void);
MIDI_Channel get_rand_channel(void);

MIDI_MessageType get_rand_basic_msg_type(void);
MIDI_MessageType get_rand_basic_non_realtime_msg_type(void);
MIDI_MessageType get_rand_basic_realtime_msg_type(void);

MIDI_QuarterFrameType get_rand_qf_type(void);

MIDI_Message get_rand_basic_message(void);
MIDI_Message get_rand_basic_non_realtime_message(void);
MIDI_Message get_rand_basic_realtime_message(void);

void setup_rand(void);

uint8_t get_pitch_bend_lsb(int16_t value);
uint8_t get_pitch_bend_msb(int16_t value);
int16_t make_pitch_bend_value(uint8_t lsb, uint8_t msb);

uint16_t make_song_position_pointer_value(uint8_t lsb, uint8_t msb);

#endif
