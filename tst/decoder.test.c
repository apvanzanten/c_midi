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

#define TEST_CHANNEL      2
#define TEST_CHANNEL_BITS (TEST_CHANNEL - 1)

#define TO_BE_IGNORED_CHANNEL      3
#define TO_BE_IGNORED_CHANNEL_BITS (TO_BE_IGNORED_CHANNEL - 1)

static Result setup(void ** env_p);
static Result teardown(void ** env_p);

static Result tst_fixture(void * env) {
  Result               r       = PASS;
  const MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  EXPECT_NE(&r, NULL, decoder);
  if(HAS_FAILED(&r)) return r;

  EXPECT_EQ(&r, TEST_CHANNEL, decoder->channel);
  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));
  EXPECT_TRUE(&r, MIDI_decoder_is_ready(decoder));

  return r;
}

static Result tst_note_on(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const MIDI_MessageType msg_type      = MIDI_MSG_TYPE_NOTE_ON;
  const uint8_t          status_bit    = (1 << 7); // 0b1000'0000
  const uint8_t          status_byte   = status_bit | (msg_type << 4) | TEST_CHANNEL_BITS;
  const MIDI_Note        note          = MIDI_NOTE_C_4;
  const uint8_t          note_byte     = MIDI_note_to_byte(note);
  const uint8_t          velocity_byte = 100;

  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, status_byte));
  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, note_byte));
  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, velocity_byte));

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));

  const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_NOTE_ON, peek_res.type);
  EXPECT_EQ(&r, note, peek_res.as.channel_msg.data.note_on.note);
  EXPECT_EQ(&r, 100, peek_res.as.channel_msg.data.note_on.velocity);

  const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_NOTE_ON, pop_res.type);
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
  const uint8_t          status_byte   = status_bit | (msg_type << 4) | TEST_CHANNEL_BITS;
  const MIDI_Note        note          = MIDI_NOTE_F_2;
  const uint8_t          note_byte     = MIDI_note_to_byte(note);
  const uint8_t          velocity_byte = 0;

  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, status_byte));
  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, note_byte));
  EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, velocity_byte));

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));

  const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_NOTE_OFF, peek_res.type);
  EXPECT_EQ(&r, note, peek_res.as.channel_msg.data.note_off.note);
  EXPECT_EQ(&r, 63, peek_res.as.channel_msg.data.note_off.velocity);

  const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_NOTE_OFF, pop_res.type);
  EXPECT_EQ(&r, note, pop_res.as.channel_msg.data.note_off.note);
  EXPECT_EQ(&r, 63, pop_res.as.channel_msg.data.note_off.velocity);

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
      status_bit | (MIDI_MSG_TYPE_NOTE_ON << 4)         | TEST_CHANNEL_BITS,  MIDI_NOTE_A_3,    27,
                                                                              MIDI_NOTE_D_5,    40,
                                                                              MIDI_NOTE_A_3,     0,
                                                                              MIDI_NOTE_F_2,    29,
      status_bit | (MIDI_MSG_TYPE_NOTE_ON << 4)         | TEST_CHANNEL_BITS,  MIDI_NOTE_G_8,    20,
      status_bit | (MIDI_MSG_TYPE_NOTE_ON << 4)         | TO_BE_IGNORED_CHANNEL_BITS,   MIDI_NOTE_A_3,    99, 
                                                                                        MIDI_NOTE_A_4,    21, 
      status_bit | (MIDI_MSG_TYPE_NOTE_OFF << 4)        | TEST_CHANNEL_BITS,  MIDI_NOTE_D_5,    100,
                                                                              MIDI_NOTE_F_2,    29,
      status_bit | (MIDI_MSG_TYPE_NOTE_ON << 4)         | TO_BE_IGNORED_CHANNEL_BITS,   MIDI_NOTE_G_3,    99, 
      status_bit | (MIDI_MSG_TYPE_CONTROL_CHANGE << 4)  | TEST_CHANNEL_BITS,  MIDI_CTRL_ATTACK_TIME,      29,
                                                                              MIDI_CTRL_CUTOFF_FREQUENCY, 99,
                                                                              MIDI_CTRL_EFFECT1,          20,
      status_bit | (MIDI_MSG_TYPE_NOTE_OFF << 4)        | TEST_CHANNEL_BITS,  MIDI_NOTE_G_8,    19,
      status_bit | (MIDI_MSG_TYPE_CONTROL_CHANGE << 4)  | TO_BE_IGNORED_CHANNEL_BITS,   MIDI_CTRL_MOD_WHEEL,  29,
      status_bit | (MIDI_MSG_TYPE_CONTROL_CHANGE << 4)  | TEST_CHANNEL_BITS,  MIDI_CTRL_GENERAL_A,        101,
                                                                              MIDI_CTRL_GENERAL_A_LSB,    29,
      status_bit | (MIDI_MSG_TYPE_PITCH_BEND << 4)  | TEST_CHANNEL_BITS,  pitch_bend_lsb(8000), pitch_bend_msb(8000),
      status_bit | (MIDI_MSG_TYPE_PITCH_BEND << 4)  | TO_BE_IGNORED_CHANNEL_BITS,  pitch_bend_lsb(-2), pitch_bend_msb(-2),
      status_bit | (MIDI_MSG_TYPE_PITCH_BEND << 4)  | TEST_CHANNEL_BITS,  pitch_bend_lsb(-5000), pitch_bend_msb(-5000),
                                                                          pitch_bend_lsb(0),     pitch_bend_msb(0),
                                                                          pitch_bend_lsb(5),     pitch_bend_msb(5),

      // clang-format on
  };

  const MIDI_Message expect_msgs[] = {
      // clang-format off
      {.type = MIDI_MSG_TYPE_NOTE_ON, .as.channel_msg.data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 27}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .as.channel_msg.data.note_on = {.note = MIDI_NOTE_D_5, .velocity = 40}},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .as.channel_msg.data.note_off = {.note = MIDI_NOTE_A_3, .velocity = 63}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .as.channel_msg.data.note_on = {.note = MIDI_NOTE_F_2, .velocity = 29}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .as.channel_msg.data.note_on = {.note = MIDI_NOTE_G_8, .velocity = 20}},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .as.channel_msg.data.note_off = {.note = MIDI_NOTE_D_5, .velocity = 100}},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .as.channel_msg.data.note_off = {.note = MIDI_NOTE_F_2, .velocity = 29}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .as.channel_msg.data.control_change = {.control = MIDI_CTRL_ATTACK_TIME, .value = 29}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .as.channel_msg.data.control_change = {.control = MIDI_CTRL_CUTOFF_FREQUENCY, .value = 99}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .as.channel_msg.data.control_change = {.control = MIDI_CTRL_EFFECT1, .value = 20}},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .as.channel_msg.data.note_off = {.note = MIDI_NOTE_G_8, .velocity = 19}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .as.channel_msg.data.control_change = {.control = MIDI_CTRL_GENERAL_A, .value = 101}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .as.channel_msg.data.control_change = {.control = MIDI_CTRL_GENERAL_A_LSB, .value = 29}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .as.channel_msg.data.pitch_bend.value = 8000},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .as.channel_msg.data.pitch_bend.value = -5000},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .as.channel_msg.data.pitch_bend.value = 0},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .as.channel_msg.data.pitch_bend.value = 5},
      // clang-format on
  };

  for(size_t i = 0; i < sizeof(bytes); i++) {
    EXPECT_TRUE(&r, MIDI_decoder_is_ready(decoder));
    if(HAS_FAILED(&r)) return r;

    EXPECT_EQ(&r, OK, MIDI_push_byte(decoder, bytes[i]));
    if(HAS_FAILED(&r)) return r;
  }

  for(size_t i = 0; i < (sizeof(expect_msgs) / sizeof(expect_msgs[0])); i++) {
    const MIDI_Message expect = expect_msgs[i];

    EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));
    if(HAS_FAILED(&r)) return r;

    const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
    const MIDI_Message pop_res  = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, expect.type, peek_res.type);
    EXPECT_EQ(&r, expect.type, pop_res.type);

    if(!HAS_FAILED(&r)) {
      switch(expect.type) {
      case MIDI_MSG_TYPE_NOTE_ON:
        EXPECT_EQ(&r, expect.as.channel_msg.data.note_on.note, peek_res.as.channel_msg.data.note_on.note);
        EXPECT_EQ(&r, expect.as.channel_msg.data.note_on.velocity, peek_res.as.channel_msg.data.note_on.velocity);
        EXPECT_EQ(&r, expect.as.channel_msg.data.note_on.note, pop_res.as.channel_msg.data.note_on.note);
        EXPECT_EQ(&r, expect.as.channel_msg.data.note_on.velocity, pop_res.as.channel_msg.data.note_on.velocity);
        break;
      case MIDI_MSG_TYPE_NOTE_OFF:
        EXPECT_EQ(&r, expect.as.channel_msg.data.note_off.note, peek_res.as.channel_msg.data.note_off.note);
        EXPECT_EQ(&r, expect.as.channel_msg.data.note_off.velocity, peek_res.as.channel_msg.data.note_off.velocity);
        EXPECT_EQ(&r, expect.as.channel_msg.data.note_off.note, pop_res.as.channel_msg.data.note_off.note);
        EXPECT_EQ(&r, expect.as.channel_msg.data.note_off.velocity, pop_res.as.channel_msg.data.note_off.velocity);
        break;
      case MIDI_MSG_TYPE_CONTROL_CHANGE:
        EXPECT_EQ(&r,
                  expect.as.channel_msg.data.control_change.control,
                  peek_res.as.channel_msg.data.control_change.control);
        EXPECT_EQ(&r,
                  expect.as.channel_msg.data.control_change.value,
                  peek_res.as.channel_msg.data.control_change.value);
        EXPECT_EQ(&r,
                  expect.as.channel_msg.data.control_change.control,
                  pop_res.as.channel_msg.data.control_change.control);
        EXPECT_EQ(&r,
                  expect.as.channel_msg.data.control_change.value,
                  pop_res.as.channel_msg.data.control_change.value);
        break;
      case MIDI_MSG_TYPE_PITCH_BEND:
        EXPECT_EQ(&r, expect.as.channel_msg.data.pitch_bend.value, peek_res.as.channel_msg.data.pitch_bend.value);
        EXPECT_EQ(&r, expect.as.channel_msg.data.pitch_bend.value, pop_res.as.channel_msg.data.pitch_bend.value);
        break;
      default: EXPECT_FALSE(&r, true); break;
      }
    }

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

  EXPECT_EQ(&r, OK, MIDI_decoder_init(*pars_p, TEST_CHANNEL));

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