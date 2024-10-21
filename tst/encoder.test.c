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

#include <cfac/test_utils.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define OK STAT_OK

#include "encoder.h"

static Result setup(void ** env_p);
static Result teardown(void ** env_p);

static Result tst_fixture(void * env) {
  Result               r       = PASS;
  const MIDI_Encoder * encoder = (MIDI_Encoder *)env;

  EXPECT_NE(&r, NULL, encoder);
  if(HAS_FAILED(&r)) return r;

  EXPECT_FALSE(&r, MIDI_encoder_has_output(encoder));
  EXPECT_TRUE(&r, MIDI_encoder_is_ready(encoder));

  return r;
}

static void expect_output_given_input(Result *             r_ptr,
                                      const char *         tst_name,
                                      MIDI_Encoder *       encoder,
                                      const MIDI_Message * input,
                                      size_t               input_n,
                                      const uint8_t *      output,
                                      size_t               output_n) {
  size_t input_idx  = 0;
  size_t output_idx = 0;

  while(output_idx < output_n) {
    while((input_idx < input_n) && MIDI_encoder_is_ready(encoder)) {
      EXPECT_OK(r_ptr, MIDI_encoder_push_message(encoder, input[input_idx++]));
    }

    EXPECT_TRUE(r_ptr, MIDI_encoder_has_output(encoder));
    if(HAS_FAILED(r_ptr)) {
      printf("failure in test %s, no more ouput after %zu out of %zu bytes\n", tst_name, output_idx, output_n);
      break;
    }

    const uint8_t expect_byte = output[output_idx];
    const uint8_t out_byte    = MIDI_encoder_pop_byte(encoder);

    EXPECT_EQ(r_ptr, expect_byte, out_byte);

    if(HAS_FAILED(r_ptr)) {
      printf("failure in test %s, byte %zu: (0d%u,0x%x) != (0d%u,0x%x)\n",
             tst_name,
             output_idx,
             expect_byte,
             expect_byte,
             out_byte,
             out_byte);
      break;
    }
    output_idx++;
  }
}

static Result tst_note_on_off(void * env) {
  Result         r       = PASS;
  MIDI_Encoder * encoder = (MIDI_Encoder *)env;

  EXPECT_TRUE(&r, MIDI_encoder_is_ready(encoder));

  const MIDI_Message input[] = {
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 2, .data.note_off = {.note = MIDI_NOTE_A_2, .velocity = 100}},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 1, .data.note_off = {.note = MIDI_NOTE_D_4, .velocity = 23}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1, .data.note_on = {.note = MIDI_NOTE_F_8, .velocity = 88}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1, .data.note_on = {.note = MIDI_NOTE_C_8, .velocity = 80}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1, .data.note_on = {.note = MIDI_NOTE_G_3, .velocity = 12}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 3, .data.note_on = {.note = MIDI_NOTE_G_3, .velocity = 12}},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 3, .data.note_off = {.note = MIDI_NOTE_E_3, .velocity = 99}},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 3, .data.note_off = {.note = MIDI_NOTE_C_3, .velocity = 85}},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 3, .data.note_off = {.note = MIDI_NOTE_G_3, .velocity = 95}},
  };
  const uint8_t expect_output[] = {
      // clang-format off
      // status byte                              note byte         velocity byte 
      0x80 | MIDI_MSG_TYPE_NOTE_OFF | 1,          MIDI_NOTE_A_2,    100,
      0x80 | MIDI_MSG_TYPE_NOTE_OFF /* | 0 */,    MIDI_NOTE_D_4,    23,
      0x80 | MIDI_MSG_TYPE_NOTE_ON  /* | 0 */,    MIDI_NOTE_F_8,    88,
      /* no status byte, running mode */          MIDI_NOTE_C_8,    80,
      /* no status byte, running mode */          MIDI_NOTE_G_3,    12,
      0x80 | MIDI_MSG_TYPE_NOTE_ON  | 2,          MIDI_NOTE_G_3,    12,
      0x80 | MIDI_MSG_TYPE_NOTE_OFF | 2,          MIDI_NOTE_E_3,    99,
      /* no status byte, running mode */          MIDI_NOTE_C_3,    85,
      /* no status byte, running mode */          MIDI_NOTE_G_3,    95,
      // clang-format on
  };

  expect_output_given_input(&r,
                            __func__,
                            encoder,
                            input,
                            sizeof(input) / sizeof(input[0]),
                            expect_output,
                            sizeof(expect_output) / sizeof(expect_output[0]));

  return r;
}

static int16_t make_pitch_bend_value(uint8_t lsb, uint8_t msb) {
  const int16_t mid = 0x40 << 7;
  return (((int16_t)(msb) << 7) | (int16_t)lsb) - mid;
}

static Result tst_channel_messages(void * env) {
  Result         r       = PASS;
  MIDI_Encoder * encoder = (MIDI_Encoder *)env;

  EXPECT_TRUE(&r, MIDI_encoder_is_ready(encoder));

  const MIDI_Message input[] = {
      // clang-format off
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1, .data.note_on = {.note = MIDI_NOTE_C_8, .velocity = 80}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1, .data.note_on = {.note = MIDI_NOTE_G_3, .velocity = 12}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = 3, .data.control_change = {.control = MIDI_CTRL_ATTACK_TIME, .value = 20}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = 3, .data.control_change = {.control = MIDI_CTRL_CUTOFF_FREQUENCY, .value = 50}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = 3, .data.control_change = {.control = MIDI_CTRL_RESONANCE, .value = 15}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = 4, .data.control_change = {.control = MIDI_CTRL_RESONANCE, .value = 15}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = 4, .data.control_change = {.control = MIDI_CTRL_CUTOFF_FREQUENCY, .value = 50}},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 3, .data.note_off = {.note = MIDI_NOTE_E_3, .velocity = 99}},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 3, .data.note_off = {.note = MIDI_NOTE_G_3, .velocity = 95}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = 2, .data.program_change = {.program_id = 2}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = 2, .data.program_change = {.program_id = 23}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = 2, .data.pitch_bend = {.value = make_pitch_bend_value(0x1a, 0x4b)}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = 2, .data.pitch_bend = {.value = make_pitch_bend_value(0x2c, 0x5d)}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = 2, .data.pitch_bend = {.value = make_pitch_bend_value(0x3e, 0x6f)}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 2, .data.note_on = {.note = MIDI_NOTE_C_4, .velocity = 99}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = 2, .data.pitch_bend = {.value = make_pitch_bend_value(0x0a, 0x7a)}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .channel = 1, .data.aftertouch_mono = {.value = 29}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .channel = 1, .data.aftertouch_mono = {.value = 34}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .channel = 1, .data.aftertouch_poly = {.note = MIDI_NOTE_A_3, .value = 35}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .channel = 1, .data.aftertouch_poly = {.note = MIDI_NOTE_C_3, .value = 38}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .channel = 1, .data.aftertouch_poly = {.note = MIDI_NOTE_G_4, .value = 38}},
      // clang-format on
  };
  const uint8_t expect_output[] = {
      // clang-format off
      // status byte                                    first data byte               second data byte 
      0x80 | MIDI_MSG_TYPE_NOTE_ON  /* | 0 */,          MIDI_NOTE_C_8,                80,
      /* no status byte, running mode */                MIDI_NOTE_G_3,                12,
      0x80 | MIDI_MSG_TYPE_CONTROL_CHANGE | 2,          MIDI_CTRL_ATTACK_TIME,        20,
      /* no status byte, running mode */                MIDI_CTRL_CUTOFF_FREQUENCY,   50,
      /* no status byte, running mode */                MIDI_CTRL_RESONANCE,          15,
      0x80 | MIDI_MSG_TYPE_CONTROL_CHANGE | 3,          MIDI_CTRL_RESONANCE,          15,
      /* no status byte, running mode */                MIDI_CTRL_CUTOFF_FREQUENCY,   50,
      0x80 | MIDI_MSG_TYPE_NOTE_OFF | 2,                MIDI_NOTE_E_3,                99,
      /* no status byte, running mode */                MIDI_NOTE_G_3,                95,
      0x80 | MIDI_MSG_TYPE_PROGRAM_CHANGE | 1,          2,
      /* no status byte, running mode */                23,
      0x80 | MIDI_MSG_TYPE_PITCH_BEND | 1,              0x1a,                         0x4b,
      /* no status byte, running mode */                0x2c,                         0x5d,
      /* no status byte, running mode */                0x3e,                         0x6f,
      0x80 | MIDI_MSG_TYPE_NOTE_ON  | 1,                MIDI_NOTE_C_4,                99,
      0x80 | MIDI_MSG_TYPE_PITCH_BEND | 1,              0x0a,                         0x7a,
      0x80 | MIDI_MSG_TYPE_AFTERTOUCH_MONO /* | 0 */,   29,
      /* no status byte, running mode */                34,
      0x80 | MIDI_MSG_TYPE_AFTERTOUCH_POLY  /* | 0 */,  MIDI_NOTE_A_3,                35,
      /* no status byte, running mode */                MIDI_NOTE_C_3,                38,
      /* no status byte, running mode */                MIDI_NOTE_G_4,                38,
      // clang-format on
  };

  expect_output_given_input(&r,
                            __func__,
                            encoder,
                            input,
                            sizeof(input) / sizeof(input[0]),
                            expect_output,
                            sizeof(expect_output) / sizeof(expect_output[0]));

  return r;
}

static Result tst_realtime_messages(void * env) {
  Result         r       = PASS;
  MIDI_Encoder * encoder = (MIDI_Encoder *)env;

  EXPECT_TRUE(&r, MIDI_encoder_is_ready(encoder));

  const MIDI_Message input[] = {
      // clang-format off
      {.type = MIDI_MSG_TYPE_ACTIVE_SENSING},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_CONTINUE},
      {.type = MIDI_MSG_TYPE_START},
      {.type = MIDI_MSG_TYPE_STOP},
      {.type = MIDI_MSG_TYPE_SYSTEM_RESET},
      // clang-format on
  };
  const uint8_t expect_output[] = {
      // clang-format off
      // status byte
      0x80 | MIDI_MSG_TYPE_ACTIVE_SENSING,
      0x80 | MIDI_MSG_TYPE_TIMING_CLOCK,
      0x80 | MIDI_MSG_TYPE_CONTINUE,
      0x80 | MIDI_MSG_TYPE_START,
      0x80 | MIDI_MSG_TYPE_STOP,
      0x80 | MIDI_MSG_TYPE_SYSTEM_RESET,
      // clang-format on
  };

  expect_output_given_input(&r,
                            __func__,
                            encoder,
                            input,
                            sizeof(input) / sizeof(input[0]),
                            expect_output,
                            sizeof(expect_output) / sizeof(expect_output[0]));

  return r;
}

static Result tst_channel_messages_with_realtime_interruptions(void * env) {
  Result         r       = PASS;
  MIDI_Encoder * encoder = (MIDI_Encoder *)env;

  EXPECT_TRUE(&r, MIDI_encoder_is_ready(encoder));

  const MIDI_Message input[] = {
      // clang-format off
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 3, .data.note_off = {.note = MIDI_NOTE_E_3, .velocity = 99}},
      {.type = MIDI_MSG_TYPE_ACTIVE_SENSING},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 3, .data.note_off = {.note = MIDI_NOTE_G_3, .velocity = 95}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1, .data.note_on = {.note = MIDI_NOTE_C_8, .velocity = 80}},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1, .data.note_on = {.note = MIDI_NOTE_G_3, .velocity = 12}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = 3, .data.control_change = {.control = MIDI_CTRL_ATTACK_TIME, .value = 20}},
      {.type = MIDI_MSG_TYPE_CONTINUE},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = 3, .data.control_change = {.control = MIDI_CTRL_CUTOFF_FREQUENCY, .value = 50}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = 2, .data.program_change = {.program_id = 2}},
      {.type = MIDI_MSG_TYPE_START},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = 2, .data.program_change = {.program_id = 23}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = 2, .data.pitch_bend = {.value = make_pitch_bend_value(0x1a, 0x4b)}},
      {.type = MIDI_MSG_TYPE_STOP},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = 2, .data.pitch_bend = {.value = make_pitch_bend_value(0x2c, 0x5d)}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .channel = 1, .data.aftertouch_mono = {.value = 29}},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .channel = 1, .data.aftertouch_mono = {.value = 34}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .channel = 1, .data.aftertouch_poly = {.note = MIDI_NOTE_C_3, .value = 38}},
      {.type = MIDI_MSG_TYPE_ACTIVE_SENSING},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .channel = 1, .data.aftertouch_poly = {.note = MIDI_NOTE_G_3, .value = 38}},
      {.type = MIDI_MSG_TYPE_SYSTEM_RESET},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .channel = 1, .data.aftertouch_poly = {.note = MIDI_NOTE_G_4, .value = 38}},
      // clang-format on
  };
  const uint8_t expect_output[] = {
      // clang-format off
      // status byte                                    first data byte               second data byte 
      0x80 | MIDI_MSG_TYPE_NOTE_OFF | 2,                MIDI_NOTE_E_3,                99,
      0x80 | MIDI_MSG_TYPE_ACTIVE_SENSING,
      /* no status byte, running mode */                MIDI_NOTE_G_3,                95,
      0x80 | MIDI_MSG_TYPE_NOTE_ON  /* | 0 */,          MIDI_NOTE_C_8,                80,
      0x80 | MIDI_MSG_TYPE_TIMING_CLOCK,
      /* no status byte, running mode */                MIDI_NOTE_G_3,                12,
      0x80 | MIDI_MSG_TYPE_CONTROL_CHANGE | 2,          MIDI_CTRL_ATTACK_TIME,        20,
      0x80 | MIDI_MSG_TYPE_CONTINUE,
      /* no status byte, running mode */                MIDI_CTRL_CUTOFF_FREQUENCY,   50,
      0x80 | MIDI_MSG_TYPE_PROGRAM_CHANGE | 1,          2,
      0x80 | MIDI_MSG_TYPE_START,
      /* no status byte, running mode */                23,
      0x80 | MIDI_MSG_TYPE_PITCH_BEND | 1,              0x1a,                         0x4b,
      0x80 | MIDI_MSG_TYPE_STOP,
      /* no status byte, running mode */                0x2c,                         0x5d,
      0x80 | MIDI_MSG_TYPE_AFTERTOUCH_MONO /* | 0 */,   29,
      0x80 | MIDI_MSG_TYPE_TIMING_CLOCK,
      /* no status byte, running mode */                34,
      0x80 | MIDI_MSG_TYPE_AFTERTOUCH_POLY  /* | 0 */,  MIDI_NOTE_C_3,                38,
      0x80 | MIDI_MSG_TYPE_ACTIVE_SENSING,
      /* no status byte, running mode */                MIDI_NOTE_G_3,                38,
      0x80 | MIDI_MSG_TYPE_SYSTEM_RESET, // is realtime but should reset running state regardless
      0x80 | MIDI_MSG_TYPE_AFTERTOUCH_POLY  /* | 0 */,  MIDI_NOTE_G_4,                38,
      // clang-format on
  };

  expect_output_given_input(&r,
                            __func__,
                            encoder,
                            input,
                            sizeof(input) / sizeof(input[0]),
                            expect_output,
                            sizeof(expect_output) / sizeof(expect_output[0]));

  return r;
}

int main(void) {
  TestWithFixture tests_with_fixture[] = {
      tst_fixture,
      tst_note_on_off,
      tst_channel_messages,
      tst_realtime_messages,
      tst_channel_messages_with_realtime_interruptions,
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

  MIDI_Encoder ** pars_p = (MIDI_Encoder **)env_p;

  *pars_p = malloc(sizeof(MIDI_Encoder));
  EXPECT_NE(&r, NULL, *pars_p);
  if(HAS_FAILED(&r)) return r;

  EXPECT_EQ(&r, OK, MIDI_encoder_init(*pars_p));

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