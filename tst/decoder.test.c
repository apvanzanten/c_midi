// MIT License
//
// Copyright (c) 2023 Arjen P. van Zanten
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

#include <cfac/test_utils.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define OK STAT_OK

#include "decoder.h"

#define TEST_CHANNEL_1      2
#define TEST_CHANNEL_1_BITS (TEST_CHANNEL_1 - 1)

#define TEST_CHANNEL_2      3
#define TEST_CHANNEL_2_BITS (TEST_CHANNEL_2 - 1)

static Result setup(void ** env_p);
static Result teardown(void ** env_p);

static Result tst_fixture(void * env) {
  Result               r       = PASS;
  const MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  EXPECT_NE(&r, NULL, decoder);
  if(HAS_FAILED(&r)) return r;

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));
  EXPECT_TRUE(&r, MIDI_decoder_is_ready(decoder));

  return r;
}

static Result tst_note_on(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const MIDI_MessageType msg_type      = MIDI_MSG_TYPE_NOTE_ON;
  const uint8_t          status_bit    = (1 << 7); // 0b1000'0000
  const uint8_t          status_byte   = status_bit | (msg_type << 4) | TEST_CHANNEL_1_BITS;
  const MIDI_Note        note          = MIDI_NOTE_C_4;
  const uint8_t          note_byte     = MIDI_note_to_byte(note);
  const uint8_t          velocity_byte = 100;

  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, status_byte));
  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, note_byte));
  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, velocity_byte));

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));

  const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_NOTE_ON, peek_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, peek_res.as.channel_msg.channel);
  EXPECT_EQ(&r, note, peek_res.as.channel_msg.data.note_on.note);
  EXPECT_EQ(&r, 100, peek_res.as.channel_msg.data.note_on.velocity);

  const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_NOTE_ON, pop_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, pop_res.as.channel_msg.channel);
  EXPECT_EQ(&r, note, pop_res.as.channel_msg.data.note_on.note);
  EXPECT_EQ(&r, 100, pop_res.as.channel_msg.data.note_on.velocity);

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));

  return r;
}

static Result tst_note_on_zero_velocity(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const MIDI_MessageType msg_type      = MIDI_MSG_TYPE_NOTE_ON;
  const uint8_t          status_bit    = (1 << 7); // 0b1000'0000
  const uint8_t          status_byte   = status_bit | (msg_type << 4) | TEST_CHANNEL_1_BITS;
  const MIDI_Note        note          = MIDI_NOTE_F_2;
  const uint8_t          note_byte     = MIDI_note_to_byte(note);
  const uint8_t          velocity_byte = 0;

  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, status_byte));
  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, note_byte));
  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, velocity_byte));

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));

  const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_NOTE_ON, peek_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, peek_res.as.channel_msg.channel);
  EXPECT_EQ(&r, note, peek_res.as.channel_msg.data.note_on.note);
  EXPECT_EQ(&r, 0, peek_res.as.channel_msg.data.note_on.velocity);

  const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_NOTE_ON, pop_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, pop_res.as.channel_msg.channel);
  EXPECT_EQ(&r, note, pop_res.as.channel_msg.data.note_on.note);
  EXPECT_EQ(&r, 0, pop_res.as.channel_msg.data.note_on.velocity);

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));

  return r;
}

static Result tst_aftertouch_mono(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const MIDI_MessageType msg_type    = MIDI_MSG_TYPE_AFTERTOUCH_MONO;
  const uint8_t          status_bit  = (1 << 7); // 0b1000'0000
  const uint8_t          status_byte = status_bit | (msg_type << 4) | TEST_CHANNEL_1_BITS;
  const uint8_t          value_byte  = 27;

  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, status_byte));
  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, value_byte));

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));

  const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_AFTERTOUCH_MONO, peek_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, peek_res.as.channel_msg.channel);
  EXPECT_EQ(&r, value_byte, peek_res.as.channel_msg.data.aftertouch_mono.value);

  const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_AFTERTOUCH_MONO, pop_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, pop_res.as.channel_msg.channel);
  EXPECT_EQ(&r, value_byte, pop_res.as.channel_msg.data.aftertouch_mono.value);

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));

  return r;
}

static Result tst_aftertouch_poly(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const MIDI_MessageType msg_type    = MIDI_MSG_TYPE_AFTERTOUCH_POLY;
  const uint8_t          status_bit  = (1 << 7); // 0b1000'0000
  const uint8_t          status_byte = status_bit | (msg_type << 4) | TEST_CHANNEL_1_BITS;
  const uint8_t          note_byte   = MIDI_NOTE_B_5;
  const uint8_t          value_byte  = 73;

  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, status_byte));
  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, note_byte));
  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, value_byte));

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));

  const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_AFTERTOUCH_POLY, peek_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, peek_res.as.channel_msg.channel);
  EXPECT_EQ(&r, note_byte, peek_res.as.channel_msg.data.aftertouch_poly.note);
  EXPECT_EQ(&r, value_byte, peek_res.as.channel_msg.data.aftertouch_poly.value);

  const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_AFTERTOUCH_POLY, pop_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, pop_res.as.channel_msg.channel);
  EXPECT_EQ(&r, note_byte, pop_res.as.channel_msg.data.aftertouch_poly.note);
  EXPECT_EQ(&r, value_byte, pop_res.as.channel_msg.data.aftertouch_poly.value);

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));

  return r;
}

static Result tst_program_change(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const MIDI_MessageType msg_type    = MIDI_MSG_TYPE_PROGRAM_CHANGE;
  const uint8_t          status_bit  = (1 << 7); // 0b1000'0000
  const uint8_t          status_byte = status_bit | (msg_type << 4) | TEST_CHANNEL_1_BITS;
  const uint8_t          program_id  = 44;

  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, status_byte));
  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, program_id));

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));

  const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_PROGRAM_CHANGE, peek_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, peek_res.as.channel_msg.channel);
  EXPECT_EQ(&r, program_id, peek_res.as.channel_msg.data.program_change.program_id);

  const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_PROGRAM_CHANGE, pop_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, pop_res.as.channel_msg.channel);
  EXPECT_EQ(&r, program_id, pop_res.as.channel_msg.data.program_change.program_id);

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));

  return r;
}

static Result tst_real_time(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const MIDI_MessageType msg_type          = MIDI_MSG_TYPE_SYSTEM;
  const uint8_t          status_bit        = (1 << 7); // 0b1000'0000
  const uint8_t          timing_clock_byte = status_bit | (msg_type << 4) | MIDI_MSG_TYPE_TIMING_CLOCK;
  const uint8_t          start_byte        = status_bit | (msg_type << 4) | MIDI_MSG_TYPE_START;
  const uint8_t          continue_byte     = status_bit | (msg_type << 4) | MIDI_MSG_TYPE_CONTINUE;
  const uint8_t          stop_byte         = status_bit | (msg_type << 4) | MIDI_MSG_TYPE_STOP;
  const uint8_t          active_sense_byte = status_bit | (msg_type << 4) | MIDI_MSG_TYPE_ACTIVE_SENSING;
  const uint8_t          reset_byte        = status_bit | (msg_type << 4) | MIDI_MSG_TYPE_SYSTEM_RESET;

  EXPECT_OK(&r, MIDI_push_byte(decoder, timing_clock_byte));
  EXPECT_OK(&r, MIDI_push_byte(decoder, start_byte));
  EXPECT_OK(&r, MIDI_push_byte(decoder, continue_byte));
  EXPECT_OK(&r, MIDI_push_byte(decoder, stop_byte));
  EXPECT_OK(&r, MIDI_push_byte(decoder, active_sense_byte));
  EXPECT_OK(&r, MIDI_push_byte(decoder, reset_byte));

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_SYSTEM, pop_res.type);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_TIMING_CLOCK, pop_res.as.system_msg.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_SYSTEM, pop_res.type);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_START, pop_res.as.system_msg.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_SYSTEM, pop_res.type);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_CONTINUE, pop_res.as.system_msg.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_SYSTEM, pop_res.type);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_STOP, pop_res.as.system_msg.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_SYSTEM, pop_res.type);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_ACTIVE_SENSING, pop_res.as.system_msg.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_SYSTEM, pop_res.type);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_SYSTEM_RESET, pop_res.as.system_msg.type);
  }

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));

  return r;
}

static Result tst_real_time_with_running_status(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const MIDI_MessageType msg_type   = MIDI_MSG_TYPE_SYSTEM;
  const uint8_t          status_bit = (1 << 7); // 0b1000'0000

  const uint8_t program_change_status_byte = status_bit | (MIDI_MSG_TYPE_PROGRAM_CHANGE << 4) | TEST_CHANNEL_1_BITS;
  const uint8_t program_id                 = 44;

  const uint8_t timing_clock_byte = status_bit | (msg_type << 4) | MIDI_MSG_TYPE_TIMING_CLOCK;
  const uint8_t start_byte        = status_bit | (msg_type << 4) | MIDI_MSG_TYPE_START;
  const uint8_t continue_byte     = status_bit | (msg_type << 4) | MIDI_MSG_TYPE_CONTINUE;
  const uint8_t stop_byte         = status_bit | (msg_type << 4) | MIDI_MSG_TYPE_STOP;
  const uint8_t active_sense_byte = status_bit | (msg_type << 4) | MIDI_MSG_TYPE_ACTIVE_SENSING;
  const uint8_t reset_byte        = status_bit | (msg_type << 4) | MIDI_MSG_TYPE_SYSTEM_RESET;

  EXPECT_OK(&r, MIDI_push_byte(decoder, program_change_status_byte));

  EXPECT_OK(&r, MIDI_push_byte(decoder, timing_clock_byte));
  EXPECT_OK(&r, MIDI_push_byte(decoder, start_byte));
  EXPECT_OK(&r, MIDI_push_byte(decoder, continue_byte));
  EXPECT_OK(&r, MIDI_push_byte(decoder, stop_byte));
  EXPECT_OK(&r, MIDI_push_byte(decoder, active_sense_byte));

  EXPECT_OK(&r, MIDI_push_byte(decoder, program_id)); // results in completion of program change message

  EXPECT_OK(&r, MIDI_push_byte(decoder, program_change_status_byte));

  EXPECT_OK(&r, MIDI_push_byte(decoder, reset_byte)); // aborts program change message

  EXPECT_OK(&r, MIDI_push_byte(decoder, program_id)); // does not result in completion of program change message

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_SYSTEM, pop_res.type);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_TIMING_CLOCK, pop_res.as.system_msg.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_SYSTEM, pop_res.type);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_START, pop_res.as.system_msg.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_SYSTEM, pop_res.type);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_CONTINUE, pop_res.as.system_msg.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_SYSTEM, pop_res.type);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_STOP, pop_res.as.system_msg.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_SYSTEM, pop_res.type);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_ACTIVE_SENSING, pop_res.as.system_msg.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_PROGRAM_CHANGE, pop_res.type);
    EXPECT_EQ(&r, TEST_CHANNEL_1, pop_res.as.channel_msg.channel);
    EXPECT_EQ(&r, program_id, pop_res.as.channel_msg.data.program_change.program_id);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_SYSTEM, pop_res.type);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_SYSTEM_RESET, pop_res.as.system_msg.type);
  }

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));

  return r;
}

static uint8_t pitch_bend_lsb(int16_t value) {
  return value & 0x7f; // 0b0111'1111
}
static uint8_t pitch_bend_msb(int16_t value) {
  const int16_t mid_point = (0x40) << 7;
  return ((value + mid_point) >> 7) & 0x7f; // 0b0111'1111;
}

static Result tst_multiple_msgs(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  printf("start test %s\n", __func__);

  const uint8_t status_bit = (1 << 7); // 0b1000'0000

  const uint8_t bytes[] = {
      // clang-format off
      status_bit | (MIDI_MSG_TYPE_NOTE_ON << 4)         | TEST_CHANNEL_1_BITS,  MIDI_NOTE_A_3,    27,
                                                                                MIDI_NOTE_D_5,    40,
                                                                                MIDI_NOTE_A_3,     0,
      status_bit | (MIDI_MSG_TYPE_SYSTEM << 4)          | MIDI_MSG_TYPE_START, 
                                                                                MIDI_NOTE_F_2,    29,
      status_bit | (MIDI_MSG_TYPE_NOTE_ON << 4)         | TEST_CHANNEL_1_BITS,  MIDI_NOTE_G_8,    20,
      status_bit | (MIDI_MSG_TYPE_SYSTEM << 4)          | MIDI_MSG_TYPE_TIMING_CLOCK, 
      status_bit | (MIDI_MSG_TYPE_NOTE_ON << 4)         | TEST_CHANNEL_2_BITS,  MIDI_NOTE_A_3,    99, 
                                                                                MIDI_NOTE_A_4,    21, 
      status_bit | (MIDI_MSG_TYPE_NOTE_OFF << 4)        | TEST_CHANNEL_1_BITS,  MIDI_NOTE_D_5,    100,
      status_bit | (MIDI_MSG_TYPE_SYSTEM << 4)          | MIDI_MSG_TYPE_TIMING_CLOCK, 
                                                                                MIDI_NOTE_F_2,    29,
      status_bit | (MIDI_MSG_TYPE_NOTE_ON << 4)         | TEST_CHANNEL_2_BITS,  MIDI_NOTE_G_3,    99, 
      status_bit | (MIDI_MSG_TYPE_CONTROL_CHANGE << 4)  | TEST_CHANNEL_1_BITS,  MIDI_CTRL_ATTACK_TIME,      29,
                                                                                MIDI_CTRL_CUTOFF_FREQUENCY, 99,
      status_bit | (MIDI_MSG_TYPE_SYSTEM << 4)          | MIDI_MSG_TYPE_TIMING_CLOCK, 
                                                                                MIDI_CTRL_EFFECT1,          20,
      status_bit | (MIDI_MSG_TYPE_PROGRAM_CHANGE << 4)  | TEST_CHANNEL_2_BITS,  3,
                                                                                28,
                                                                                83,
      status_bit | (MIDI_MSG_TYPE_NOTE_OFF << 4)        | TEST_CHANNEL_1_BITS,  
      status_bit | (MIDI_MSG_TYPE_SYSTEM << 4)          | MIDI_MSG_TYPE_TIMING_CLOCK, 
                                                                                MIDI_NOTE_G_8,    19,
      status_bit | (MIDI_MSG_TYPE_CONTROL_CHANGE << 4)  | TEST_CHANNEL_2_BITS,  MIDI_CTRL_MOD_WHEEL,  29,
      status_bit | (MIDI_MSG_TYPE_PROGRAM_CHANGE << 4)  | TEST_CHANNEL_1_BITS,  99,
      status_bit | (MIDI_MSG_TYPE_CONTROL_CHANGE << 4)  | TEST_CHANNEL_1_BITS,  MIDI_CTRL_GENERAL_A,        101,
                                                                                MIDI_CTRL_GENERAL_A_LSB,    29,
      status_bit | (MIDI_MSG_TYPE_PITCH_BEND << 4)  | TEST_CHANNEL_1_BITS,  pitch_bend_lsb(8000),  pitch_bend_msb(8000),
      status_bit | (MIDI_MSG_TYPE_PITCH_BEND << 4)  | TEST_CHANNEL_2_BITS,  pitch_bend_lsb(-2),    pitch_bend_msb(-2),
      status_bit | (MIDI_MSG_TYPE_PITCH_BEND << 4)  | TEST_CHANNEL_1_BITS,  pitch_bend_lsb(-5000), pitch_bend_msb(-5000),
                                                                            pitch_bend_lsb(0),     
      status_bit | (MIDI_MSG_TYPE_SYSTEM << 4)          | MIDI_MSG_TYPE_TIMING_CLOCK, 
                                                                            pitch_bend_msb(0),
                                                                            pitch_bend_lsb(5),     pitch_bend_msb(5),
      status_bit | (MIDI_MSG_TYPE_AFTERTOUCH_MONO << 4)  | TEST_CHANNEL_1_BITS,
      status_bit | (MIDI_MSG_TYPE_SYSTEM << 4)          | MIDI_MSG_TYPE_TIMING_CLOCK, 
                                                                                 4,
      status_bit | (MIDI_MSG_TYPE_AFTERTOUCH_MONO << 4)  | TEST_CHANNEL_2_BITS,  7,
                                                                                 24,
                                                                                 28,
      status_bit | (MIDI_MSG_TYPE_PROGRAM_CHANGE << 4)  | TEST_CHANNEL_2_BITS,  27,
      status_bit | (MIDI_MSG_TYPE_AFTERTOUCH_POLY << 4)  | TEST_CHANNEL_1_BITS,  MIDI_NOTE_G_8, 15,
                                                                                 MIDI_NOTE_G_7, 18,
                                                                                 MIDI_NOTE_F_3, 88,
      status_bit | (MIDI_MSG_TYPE_PITCH_BEND << 4)  | TEST_CHANNEL_1_BITS,  pitch_bend_lsb(293),  pitch_bend_msb(293),
      status_bit | (MIDI_MSG_TYPE_AFTERTOUCH_POLY << 4)  | TEST_CHANNEL_2_BITS,  MIDI_NOTE_A_4, 37,

      status_bit | (MIDI_MSG_TYPE_PITCH_BEND << 4)  | TEST_CHANNEL_1_BITS,  pitch_bend_lsb(293),
      status_bit | (MIDI_MSG_TYPE_SYSTEM << 4)          | MIDI_MSG_TYPE_SYSTEM_RESET, 
                                                                            pitch_bend_msb(293),
      status_bit | (MIDI_MSG_TYPE_AFTERTOUCH_POLY << 4)  | TEST_CHANNEL_2_BITS,  MIDI_NOTE_G_4, 3,
      // clang-format on
  };

  const MIDI_Message expect_msgs[] = {
      // clang-format off
      {.type = MIDI_MSG_TYPE_NOTE_ON, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 27}}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.note_on = {.note = MIDI_NOTE_D_5, .velocity = 40}}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 0}}},
      {.type = MIDI_MSG_TYPE_SYSTEM, .as.system_msg.type = MIDI_MSG_TYPE_START},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.note_on = {.note = MIDI_NOTE_F_2, .velocity = 29}}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.note_on = {.note = MIDI_NOTE_G_8, .velocity = 20}}},
      {.type = MIDI_MSG_TYPE_SYSTEM, .as.system_msg.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .as.channel_msg = {.channel=TEST_CHANNEL_2, .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 99}}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .as.channel_msg = {.channel=TEST_CHANNEL_2, .data.note_on = {.note = MIDI_NOTE_A_4, .velocity = 21}}},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.note_off = {.note = MIDI_NOTE_D_5, .velocity = 100}}},
      {.type = MIDI_MSG_TYPE_SYSTEM, .as.system_msg.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.note_off = {.note = MIDI_NOTE_F_2, .velocity = 29}}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .as.channel_msg = {.channel=TEST_CHANNEL_2, .data.note_on = {.note = MIDI_NOTE_G_3, .velocity = 99}}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.control_change = {.control = MIDI_CTRL_ATTACK_TIME, .value = 29}}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.control_change = {.control = MIDI_CTRL_CUTOFF_FREQUENCY, .value = 99}}},
      {.type = MIDI_MSG_TYPE_SYSTEM, .as.system_msg.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.control_change = {.control = MIDI_CTRL_EFFECT1, .value = 20}}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .as.channel_msg = {.channel=TEST_CHANNEL_2, .data.program_change.program_id = 3}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .as.channel_msg = {.channel=TEST_CHANNEL_2, .data.program_change.program_id = 28}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .as.channel_msg = {.channel=TEST_CHANNEL_2, .data.program_change.program_id = 83}},
      {.type = MIDI_MSG_TYPE_SYSTEM, .as.system_msg.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.note_off = {.note = MIDI_NOTE_G_8, .velocity = 19}}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .as.channel_msg = {.channel=TEST_CHANNEL_2, .data.control_change = {.control = MIDI_CTRL_MOD_WHEEL, .value = 29}}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.program_change.program_id = 99}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.control_change = {.control = MIDI_CTRL_GENERAL_A, .value = 101}}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.control_change = {.control = MIDI_CTRL_GENERAL_A_LSB, .value = 29}}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.pitch_bend.value = 8000}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .as.channel_msg = {.channel=TEST_CHANNEL_2, .data.pitch_bend.value = -2}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.pitch_bend.value = -5000}},
      {.type = MIDI_MSG_TYPE_SYSTEM, .as.system_msg.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.pitch_bend.value = 0}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.pitch_bend.value = 5}},
      {.type = MIDI_MSG_TYPE_SYSTEM, .as.system_msg.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.aftertouch_mono.value = 4}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .as.channel_msg = {.channel=TEST_CHANNEL_2, .data.aftertouch_mono.value = 7}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .as.channel_msg = {.channel=TEST_CHANNEL_2, .data.aftertouch_mono.value = 24}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .as.channel_msg = {.channel=TEST_CHANNEL_2, .data.aftertouch_mono.value = 28}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .as.channel_msg = {.channel=TEST_CHANNEL_2, .data.program_change.program_id = 27}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.aftertouch_poly = {.note= MIDI_NOTE_G_8, .value=15}}}, 
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.aftertouch_poly = {.note= MIDI_NOTE_G_7, .value=18}}}, 
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.aftertouch_poly = {.note= MIDI_NOTE_F_3, .value=88}}}, 
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .as.channel_msg = {.channel=TEST_CHANNEL_1, .data.pitch_bend.value = 293}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .as.channel_msg = {.channel=TEST_CHANNEL_2, .data.aftertouch_poly = {.note= MIDI_NOTE_A_4, .value=37}}},
      {.type = MIDI_MSG_TYPE_SYSTEM, .as.system_msg.type = MIDI_MSG_TYPE_SYSTEM_RESET},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .as.channel_msg = {.channel=TEST_CHANNEL_2, .data.aftertouch_poly = {.note= MIDI_NOTE_G_4, .value=3}}},
      // clang-format on
  };

  size_t push_idx = 0;

  for(size_t pop_idx = 0; pop_idx < (sizeof(expect_msgs) / sizeof(expect_msgs[0])); pop_idx++) {
    for(; (push_idx < (sizeof(bytes) / sizeof(bytes[0]))) && MIDI_decoder_is_ready(decoder); push_idx++) {
      EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, bytes[push_idx]));
      if(HAS_FAILED(&r)) return r;
    }

    const MIDI_Message expect = expect_msgs[pop_idx];

    EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));
    if(HAS_FAILED(&r)) return r;

    const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
    const MIDI_Message pop_res  = MIDI_decoder_pop_msg(decoder);
    EXPECT_TRUE(&r, MIDI_message_equals(&peek_res, &pop_res));

    EXPECT_TRUE(&r, MIDI_message_equals(&expect, &pop_res));

    if(HAS_FAILED(&r)) {
      {
        char buff[128] = {0};
        MIDI_message_to_str_buffer(buff, sizeof(buff) - 1, expect);
        printf("%s", buff);
      }
      printf(" != ");
      {
        char buff[128] = {0};
        MIDI_message_to_str_buffer(buff, sizeof(buff) - 1, pop_res);
        printf("%s", buff);
      }
      printf("\n");
    }
  }

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));

  return r;
}

int main(void) {
  TestWithFixture tests_with_fixture[] = {
      tst_fixture,
      tst_note_on,
      tst_note_on_zero_velocity,
      tst_aftertouch_mono,
      tst_aftertouch_poly,
      tst_program_change,
      tst_real_time,
      tst_real_time_with_running_status,
      tst_multiple_msgs,
  };

  return (run_tests_with_fixture(tests_with_fixture,
                                 sizeof(tests_with_fixture) / sizeof(TestWithFixture),
                                 setup,
                                 teardown) == PASS)
             ? 0
             : 1;
}

static Result setup(void ** env_p) {
  Result r = PASS;

  EXPECT_NE(&r, NULL, env_p);
  if(HAS_FAILED(&r)) return r;

  MIDI_Decoder ** pars_p = (MIDI_Decoder **)env_p;

  *pars_p = malloc(sizeof(MIDI_Decoder));
  EXPECT_NE(&r, NULL, *pars_p);
  if(HAS_FAILED(&r)) return r;

  EXPECT_EQ(&r, OK, MIDI_decoder_init(*pars_p));

  return r;
}

static Result teardown(void ** env_p) {
  Result r = PASS;

  EXPECT_NE(&r, NULL, env_p);
  if(HAS_FAILED(&r)) return r;

  free(*env_p);
  *env_p = NULL;

  return r;
}