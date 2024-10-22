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
#include <time.h>

#include <cfac/darray.h>

#define OK STAT_OK

#include "test_common.h"

#include "decoder.h"

#define TEST_CHANNEL_1      2
#define TEST_CHANNEL_1_BITS (TEST_CHANNEL_1 - 1)

#define TEST_CHANNEL_2      3
#define TEST_CHANNEL_2_BITS (TEST_CHANNEL_2 - 1)

static Result setup(void ** env_p);
static Result teardown(void ** env_p);

static void check_input_and_output(Result *             r_ptr,
                                   const char *         test_name,
                                   MIDI_Decoder *       decoder,
                                   const uint8_t *      input,
                                   size_t               input_n,
                                   const MIDI_Message * expect_output,
                                   size_t               expect_output_n);

static Result tst_fixture(void * env) {
  Result               r       = PASS;
  const MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  EXPECT_NE(&r, NULL, decoder);
  if(HAS_FAILED(&r)) return r;

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));
  EXPECT_TRUE(&r, MIDI_decoder_is_ready_to_receive(decoder));

  return r;
}

static Result tst_note_on(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const MIDI_MessageType msg_type      = MIDI_MSG_TYPE_NOTE_ON;
  const uint8_t          status_bit    = (1 << 7); // 0b1000'0000
  const uint8_t          status_byte   = status_bit | msg_type | TEST_CHANNEL_1_BITS;
  const MIDI_Note        note          = MIDI_NOTE_C_4;
  const uint8_t          note_byte     = MIDI_note_to_byte(note);
  const uint8_t          velocity_byte = 100;

  EXPECT_EQ(&r, OK, MIDI_decoder_push_byte(decoder, status_byte));
  EXPECT_EQ(&r, OK, MIDI_decoder_push_byte(decoder, note_byte));
  EXPECT_EQ(&r, OK, MIDI_decoder_push_byte(decoder, velocity_byte));

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));

  const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_NOTE_ON, peek_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, peek_res.channel);
  EXPECT_EQ(&r, note, peek_res.data.note_on.note);
  EXPECT_EQ(&r, 100, peek_res.data.note_on.velocity);

  const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_NOTE_ON, pop_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, pop_res.channel);
  EXPECT_EQ(&r, note, pop_res.data.note_on.note);
  EXPECT_EQ(&r, 100, pop_res.data.note_on.velocity);

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));

  return r;
}

static Result tst_note_on_zero_velocity(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const MIDI_MessageType msg_type      = MIDI_MSG_TYPE_NOTE_ON;
  const uint8_t          status_bit    = (1 << 7); // 0b1000'0000
  const uint8_t          status_byte   = status_bit | msg_type | TEST_CHANNEL_1_BITS;
  const MIDI_Note        note          = MIDI_NOTE_F_2;
  const uint8_t          note_byte     = MIDI_note_to_byte(note);
  const uint8_t          velocity_byte = 0;

  EXPECT_EQ(&r, OK, MIDI_decoder_push_byte(decoder, status_byte));
  EXPECT_EQ(&r, OK, MIDI_decoder_push_byte(decoder, note_byte));
  EXPECT_EQ(&r, OK, MIDI_decoder_push_byte(decoder, velocity_byte));

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));

  const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_NOTE_ON, peek_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, peek_res.channel);
  EXPECT_EQ(&r, note, peek_res.data.note_on.note);
  EXPECT_EQ(&r, 0, peek_res.data.note_on.velocity);

  const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_NOTE_ON, pop_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, pop_res.channel);
  EXPECT_EQ(&r, note, pop_res.data.note_on.note);
  EXPECT_EQ(&r, 0, pop_res.data.note_on.velocity);

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));

  return r;
}

static Result tst_aftertouch_mono(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const MIDI_MessageType msg_type    = MIDI_MSG_TYPE_AFTERTOUCH_MONO;
  const uint8_t          status_bit  = (1 << 7); // 0b1000'0000
  const uint8_t          status_byte = status_bit | msg_type | TEST_CHANNEL_1_BITS;
  const uint8_t          value_byte  = 27;

  EXPECT_EQ(&r, OK, MIDI_decoder_push_byte(decoder, status_byte));
  EXPECT_EQ(&r, OK, MIDI_decoder_push_byte(decoder, value_byte));

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));

  const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_AFTERTOUCH_MONO, peek_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, peek_res.channel);
  EXPECT_EQ(&r, value_byte, peek_res.data.aftertouch_mono.value);

  const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_AFTERTOUCH_MONO, pop_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, pop_res.channel);
  EXPECT_EQ(&r, value_byte, pop_res.data.aftertouch_mono.value);

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));

  return r;
}

static Result tst_aftertouch_poly(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const MIDI_MessageType msg_type    = MIDI_MSG_TYPE_AFTERTOUCH_POLY;
  const uint8_t          status_bit  = (1 << 7); // 0b1000'0000
  const uint8_t          status_byte = status_bit | msg_type | TEST_CHANNEL_1_BITS;
  const uint8_t          note_byte   = MIDI_NOTE_B_5;
  const uint8_t          value_byte  = 73;

  EXPECT_EQ(&r, OK, MIDI_decoder_push_byte(decoder, status_byte));
  EXPECT_EQ(&r, OK, MIDI_decoder_push_byte(decoder, note_byte));
  EXPECT_EQ(&r, OK, MIDI_decoder_push_byte(decoder, value_byte));

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));

  const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_AFTERTOUCH_POLY, peek_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, peek_res.channel);
  EXPECT_EQ(&r, note_byte, peek_res.data.aftertouch_poly.note);
  EXPECT_EQ(&r, value_byte, peek_res.data.aftertouch_poly.value);

  const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_AFTERTOUCH_POLY, pop_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, pop_res.channel);
  EXPECT_EQ(&r, note_byte, pop_res.data.aftertouch_poly.note);
  EXPECT_EQ(&r, value_byte, pop_res.data.aftertouch_poly.value);

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));

  return r;
}

static Result tst_program_change(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const MIDI_MessageType msg_type    = MIDI_MSG_TYPE_PROGRAM_CHANGE;
  const uint8_t          status_bit  = (1 << 7); // 0b1000'0000
  const uint8_t          status_byte = status_bit | msg_type | TEST_CHANNEL_1_BITS;
  const uint8_t          program_id  = 44;

  EXPECT_EQ(&r, OK, MIDI_decoder_push_byte(decoder, status_byte));
  EXPECT_EQ(&r, OK, MIDI_decoder_push_byte(decoder, program_id));

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));

  const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_PROGRAM_CHANGE, peek_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, peek_res.channel);
  EXPECT_EQ(&r, program_id, peek_res.data.program_change.program_id);

  const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
  EXPECT_EQ(&r, MIDI_MSG_TYPE_PROGRAM_CHANGE, pop_res.type);
  EXPECT_EQ(&r, TEST_CHANNEL_1, pop_res.channel);
  EXPECT_EQ(&r, program_id, pop_res.data.program_change.program_id);

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));

  return r;
}

static Result tst_real_time(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const uint8_t status_bit        = (1 << 7); // 0b1000'0000
  const uint8_t timing_clock_byte = status_bit | MIDI_MSG_TYPE_TIMING_CLOCK;
  const uint8_t start_byte        = status_bit | MIDI_MSG_TYPE_START;
  const uint8_t continue_byte     = status_bit | MIDI_MSG_TYPE_CONTINUE;
  const uint8_t stop_byte         = status_bit | MIDI_MSG_TYPE_STOP;
  const uint8_t active_sense_byte = status_bit | MIDI_MSG_TYPE_ACTIVE_SENSING;
  const uint8_t reset_byte        = status_bit | MIDI_MSG_TYPE_SYSTEM_RESET;

  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, timing_clock_byte));
  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, start_byte));
  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, continue_byte));
  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, stop_byte));
  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, active_sense_byte));
  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, reset_byte));

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_TIMING_CLOCK, pop_res.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_START, pop_res.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_CONTINUE, pop_res.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_STOP, pop_res.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_ACTIVE_SENSING, pop_res.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_SYSTEM_RESET, pop_res.type);
  }

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));

  return r;
}

static Result tst_real_time_with_running_status(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const uint8_t status_bit = (1 << 7); // 0b1000'0000

  const uint8_t program_change_status_byte = status_bit | MIDI_MSG_TYPE_PROGRAM_CHANGE | TEST_CHANNEL_1_BITS;
  const uint8_t program_id                 = 44;

  const uint8_t timing_clock_byte = status_bit | MIDI_MSG_TYPE_TIMING_CLOCK;
  const uint8_t start_byte        = status_bit | MIDI_MSG_TYPE_START;
  const uint8_t continue_byte     = status_bit | MIDI_MSG_TYPE_CONTINUE;
  const uint8_t stop_byte         = status_bit | MIDI_MSG_TYPE_STOP;
  const uint8_t active_sense_byte = status_bit | MIDI_MSG_TYPE_ACTIVE_SENSING;
  const uint8_t reset_byte        = status_bit | MIDI_MSG_TYPE_SYSTEM_RESET;

  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, program_change_status_byte));

  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, timing_clock_byte));
  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, start_byte));
  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, continue_byte));
  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, stop_byte));
  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, active_sense_byte));

  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, program_id)); // results in completion of program change message

  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, program_change_status_byte));

  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, reset_byte)); // aborts program change message

  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, program_id)); // does not result in completion of program change message

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_TIMING_CLOCK, pop_res.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_START, pop_res.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_CONTINUE, pop_res.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_STOP, pop_res.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_ACTIVE_SENSING, pop_res.type);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_PROGRAM_CHANGE, pop_res.type);
    EXPECT_EQ(&r, TEST_CHANNEL_1, pop_res.channel);
    EXPECT_EQ(&r, program_id, pop_res.data.program_change.program_id);
  }

  {
    const MIDI_Message pop_res = MIDI_decoder_pop_msg(decoder);
    EXPECT_EQ(&r, MIDI_MSG_TYPE_SYSTEM_RESET, pop_res.type);
  }

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));

  return r;
}

static Result tst_real_time_prio_mode(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  EXPECT_OK(&r, MIDI_decoder_set_prio_mode(decoder, MIDI_DECODER_PRIO_MODE_REALTIME_FIRST));

  const uint8_t status_bit = (1 << 7); // 0b1000'0000

  const uint8_t bytes[] = {
      // clang-format off
      status_bit | MIDI_MSG_TYPE_NOTE_ON          | TEST_CHANNEL_1_BITS,  MIDI_NOTE_A_3,    27,
                                                                          MIDI_NOTE_D_5,    40,
                                                                          MIDI_NOTE_A_3,     0,
      status_bit | MIDI_MSG_TYPE_START, 
                                                                          MIDI_NOTE_F_2,    29,
      status_bit | MIDI_MSG_TYPE_NOTE_ON          | TEST_CHANNEL_1_BITS,  MIDI_NOTE_G_8,    20,
      status_bit | MIDI_MSG_TYPE_TIMING_CLOCK,
      // clang-format on
  };

  const MIDI_Message expect_msgs[] = {
      // clang-format off
      // realtime messages will be first even though they were not sent first
      {.type = MIDI_MSG_TYPE_START},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = TEST_CHANNEL_1, .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 27}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = TEST_CHANNEL_1, .data.note_on = {.note = MIDI_NOTE_D_5, .velocity = 40}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = TEST_CHANNEL_1, .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 0}},
      // clang-format on
  };

  for(size_t push_idx = 0; (push_idx < (sizeof(bytes) / sizeof(bytes[0]))); push_idx++) {
    EXPECT_TRUE(&r, MIDI_decoder_is_ready_to_receive(decoder));
    EXPECT_EQ(&r, OK, MIDI_decoder_push_byte(decoder, bytes[push_idx]));
    if(HAS_FAILED(&r)) return r;
  }

  for(size_t pop_idx = 0; pop_idx < (sizeof(expect_msgs) / sizeof(expect_msgs[0])); pop_idx++) {
    const MIDI_Message expect = expect_msgs[pop_idx];

    EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));
    if(HAS_FAILED(&r)) return r;

    const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
    const MIDI_Message pop_res  = MIDI_decoder_pop_msg(decoder);
    EXPECT_TRUE(&r, MIDI_message_equals(peek_res, pop_res));

    EXPECT_TRUE(&r, MIDI_message_equals(expect, pop_res));

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

  return r;
}

static Result tst_song_position_pointer(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const uint8_t status_bit = (1 << 7); // 0b1000'0000

  const uint8_t  status_byte = status_bit | MIDI_MSG_TYPE_SONG_POSITION_POINTER;
  const uint16_t value       = 16000;
  const uint8_t  value_msb   = (value >> 7) & 0x7f;
  const uint8_t  value_lsb   = value & 0x7f;

  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, status_byte));
  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, value_lsb));
  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, value_msb));

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));
  if(HAS_FAILED(&r)) return r;

  const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
  const MIDI_Message pop_res  = MIDI_decoder_pop_msg(decoder);
  EXPECT_TRUE(&r, MIDI_message_equals(peek_res, pop_res));
  EXPECT_TRUE(&r,
              MIDI_message_equals((MIDI_Message){.type = MIDI_MSG_TYPE_SONG_POSITION_POINTER,
                                                 .data.song_position_pointer.value = value},
                                  pop_res));

  if(HAS_FAILED(&r)) {
    char buff[128] = {0};
    MIDI_message_to_str_buffer(buff, sizeof(buff) - 1, pop_res);
    printf("%s\n", buff);
  }

  return r;
}

static Result tst_song_select(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const uint8_t status_bit = (1 << 7); // 0b1000'0000

  const uint8_t status_byte = status_bit | MIDI_MSG_TYPE_SONG_SELECT;
  const uint8_t value       = 100;

  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, status_byte));
  EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, value));

  EXPECT_TRUE(&r, MIDI_decoder_has_output(decoder));
  if(HAS_FAILED(&r)) return r;

  const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
  const MIDI_Message pop_res  = MIDI_decoder_pop_msg(decoder);
  EXPECT_TRUE(&r, MIDI_message_equals(peek_res, pop_res));
  EXPECT_TRUE(&r,
              MIDI_message_equals((MIDI_Message){.type = MIDI_MSG_TYPE_SONG_SELECT, .data.song_select.value = value},
                                  pop_res));

  if(HAS_FAILED(&r)) {
    char buff[128] = {0};
    MIDI_message_to_str_buffer(buff, sizeof(buff) - 1, pop_res);
    printf("%s\n", buff);
  }

  return r;
}

static Result tst_sysex_sequence(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const uint8_t status_bit = (1 << 7); // 0b1000'0000

  const uint8_t input[] = {status_bit | MIDI_MSG_TYPE_SYSEX_START,
                           0x08,
                           0x19,
                           0x2a,
                           0x3b,
                           0x4c,
                           0x5d,
                           0x6e,
                           0x7f,
                           status_bit | MIDI_MSG_TYPE_SYSEX_STOP};

  const MIDI_Message expect_msgs[] = {
      {.type = MIDI_MSG_TYPE_SYSEX_START},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x08, .sequence_number = 0}},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x19, .sequence_number = 1}},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x2a, .sequence_number = 2}},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x3b, .sequence_number = 3}},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x4c, .sequence_number = 4}},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x5d, .sequence_number = 5}},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x6e, .sequence_number = 6}},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x7f, .sequence_number = 7}},
      {.type = MIDI_MSG_TYPE_SYSEX_STOP, .data.sysex_stop = {.sequence_length = 8, .is_length_overflowed = false}},
  };

  const size_t input_n       = sizeof(input) / sizeof(input[0]);
  const size_t expect_msgs_n = sizeof(expect_msgs) / sizeof(expect_msgs[0]);

  check_input_and_output(&r, __func__, decoder, input, input_n, expect_msgs, expect_msgs_n);

  return r;
}

static Result tst_sysex_sequence_with_realtime_interruptions(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const uint8_t status_bit = (1 << 7); // 0b1000'0000

  const uint8_t input[] = {status_bit | MIDI_MSG_TYPE_SYSEX_START,
                           0x08,
                           0x19,
                           0x2a,
                           status_bit | MIDI_MSG_TYPE_TIMING_CLOCK,
                           0x3b,
                           0x4c,
                           0x5d,
                           status_bit | MIDI_MSG_TYPE_STOP,
                           0x6e,
                           0x7f,
                           status_bit | MIDI_MSG_TYPE_SYSEX_STOP};

  const MIDI_Message expect_msgs[] = {
      {.type = MIDI_MSG_TYPE_SYSEX_START},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x08, .sequence_number = 0}},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x19, .sequence_number = 1}},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x2a, .sequence_number = 2}},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x3b, .sequence_number = 3}},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x4c, .sequence_number = 4}},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x5d, .sequence_number = 5}},
      {.type = MIDI_MSG_TYPE_STOP},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x6e, .sequence_number = 6}},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x7f, .sequence_number = 7}},
      {.type = MIDI_MSG_TYPE_SYSEX_STOP, .data.sysex_stop = {.sequence_length = 8, .is_length_overflowed = false}},
  };

  const size_t input_n       = sizeof(input) / sizeof(input[0]);
  const size_t expect_msgs_n = sizeof(expect_msgs) / sizeof(expect_msgs[0]);

  check_input_and_output(&r, __func__, decoder, input, input_n, expect_msgs, expect_msgs_n);

  return r;
}

static Result tst_sysex_sequence_with_length_overflow(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  const uint8_t status_bit = (1 << 7); // 0b1000'0000

  const uint8_t start_byte = status_bit | MIDI_MSG_TYPE_SYSEX_START;
  const uint8_t stop_byte  = status_bit | MIDI_MSG_TYPE_SYSEX_STOP;

  const size_t sequence_length = 40000;

  DAR_DArray input_arr = {0};
  EXPECT_OK(&r, DAR_create(&input_arr, sizeof(uint8_t)));
  EXPECT_OK(&r, DAR_reserve(&input_arr, sequence_length + 2));

  DAR_DArray expect_arr = {0};
  EXPECT_OK(&r, DAR_create(&expect_arr, sizeof(MIDI_Message)));
  EXPECT_OK(&r, DAR_reserve(&expect_arr, sequence_length + 2));

  if(HAS_FAILED(&r)) return r;

  EXPECT_OK(&r, DAR_push_back(&input_arr, &start_byte));
  EXPECT_OK(&r, DAR_push_back(&expect_arr, &(MIDI_Message){.type = MIDI_MSG_TYPE_SYSEX_START}));

  for(size_t i = 0; i < sequence_length; i++) {
    const uint8_t byte = rand() % 0x7f;

    EXPECT_OK(&r, DAR_push_back(&input_arr, &byte));
    EXPECT_OK(&r,
              DAR_push_back(&expect_arr,
                            &(MIDI_Message){.type            = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE,
                                            .data.sysex_byte = {.byte = byte, .sequence_number = i}}));
  }

  EXPECT_OK(&r, DAR_push_back(&input_arr, &stop_byte));
  EXPECT_OK(&r,
            DAR_push_back(&expect_arr,
                          &(MIDI_Message){.type            = MIDI_MSG_TYPE_SYSEX_STOP,
                                          .data.sysex_stop = {.sequence_length      = (sequence_length & 0x7fff),
                                                              .is_length_overflowed = true}}));

  check_input_and_output(&r, __func__, decoder, input_arr.data, input_arr.size, expect_arr.data, expect_arr.size);

  EXPECT_OK(&r, DAR_destroy(&input_arr));
  EXPECT_OK(&r, DAR_destroy(&expect_arr));

  return r;
}

static Result tst_multiple_msgs(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  printf("start test %s\n", __func__);

  const uint8_t status_bit = (1 << 7); // 0b1000'0000

  const uint8_t bytes[] = {
      // clang-format off
      status_bit | MIDI_MSG_TYPE_NOTE_ON          | TEST_CHANNEL_1_BITS,  MIDI_NOTE_A_3,    27,
                                                                          MIDI_NOTE_D_5,    40,
                                                                          MIDI_NOTE_A_3,     0,
      status_bit | MIDI_MSG_TYPE_START, 
                                                                          MIDI_NOTE_F_2,    29,
      status_bit | MIDI_MSG_TYPE_NOTE_ON          | TEST_CHANNEL_1_BITS,  MIDI_NOTE_G_8,    20,
      status_bit | MIDI_MSG_TYPE_TIMING_CLOCK, 
      status_bit | MIDI_MSG_TYPE_NOTE_ON          | TEST_CHANNEL_2_BITS,  MIDI_NOTE_A_3,    99, 
                                                                          MIDI_NOTE_A_4,    21, 
      status_bit | MIDI_MSG_TYPE_NOTE_OFF         | TEST_CHANNEL_1_BITS,  MIDI_NOTE_D_5,    100,
      status_bit | MIDI_MSG_TYPE_TIMING_CLOCK, 
                                                                          MIDI_NOTE_F_2,    29,
      status_bit | MIDI_MSG_TYPE_NOTE_ON          | TEST_CHANNEL_2_BITS,  MIDI_NOTE_G_3,    99, 
      status_bit | MIDI_MSG_TYPE_CONTROL_CHANGE   | TEST_CHANNEL_1_BITS,  MIDI_CTRL_ATTACK_TIME,      29,
                                                                          MIDI_CTRL_CUTOFF_FREQUENCY, 99,
      status_bit | MIDI_MSG_TYPE_TIMING_CLOCK, 
                                                                          MIDI_CTRL_EFFECT1,          20,
      status_bit | MIDI_MSG_TYPE_PROGRAM_CHANGE   | TEST_CHANNEL_2_BITS,  3,
                                                                          28,
                                                                          83,
      status_bit | MIDI_MSG_TYPE_NOTE_OFF         | TEST_CHANNEL_1_BITS,  
      status_bit | MIDI_MSG_TYPE_TIMING_CLOCK, 
                                                                          MIDI_NOTE_G_8,    19,
      status_bit | MIDI_MSG_TYPE_CONTROL_CHANGE   | TEST_CHANNEL_2_BITS,  MIDI_CTRL_MOD_WHEEL,  29,
      status_bit | MIDI_MSG_TYPE_PROGRAM_CHANGE   | TEST_CHANNEL_1_BITS,  99,
      status_bit | MIDI_MSG_TYPE_CONTROL_CHANGE   | TEST_CHANNEL_1_BITS,  MIDI_CTRL_GENERAL_A,        101,
                                                                          MIDI_CTRL_GENERAL_A_LSB,    29,
      status_bit | MIDI_MSG_TYPE_PITCH_BEND       | TEST_CHANNEL_1_BITS,  get_pitch_bend_lsb(8000),   get_pitch_bend_msb(8000),
      status_bit | MIDI_MSG_TYPE_PITCH_BEND       | TEST_CHANNEL_2_BITS,  get_pitch_bend_lsb(-2),     get_pitch_bend_msb(-2),
      status_bit | MIDI_MSG_TYPE_PITCH_BEND       | TEST_CHANNEL_1_BITS,  get_pitch_bend_lsb(-5000),  get_pitch_bend_msb(-5000),
                                                                          get_pitch_bend_lsb(0),     
      status_bit | MIDI_MSG_TYPE_TIMING_CLOCK, 
                                                                                                      get_pitch_bend_msb(0),
                                                                          get_pitch_bend_lsb(5),      get_pitch_bend_msb(5),
      status_bit | MIDI_MSG_TYPE_AFTERTOUCH_MONO  | TEST_CHANNEL_1_BITS,
      status_bit | MIDI_MSG_TYPE_TIMING_CLOCK, 
                                                                          4,
      status_bit | MIDI_MSG_TYPE_AFTERTOUCH_MONO  | TEST_CHANNEL_2_BITS,  7,
                                                                          24,
                                                                          28,
      status_bit | MIDI_MSG_TYPE_PROGRAM_CHANGE   | TEST_CHANNEL_2_BITS,  27,
      status_bit | MIDI_MSG_TYPE_SONG_SELECT,                             19,
      status_bit | MIDI_MSG_TYPE_AFTERTOUCH_POLY  | TEST_CHANNEL_1_BITS,  MIDI_NOTE_G_8, 15,
                                                                          MIDI_NOTE_G_7, 18,
                                                                          MIDI_NOTE_F_3, 88,
      status_bit | MIDI_MSG_TYPE_PITCH_BEND       | TEST_CHANNEL_1_BITS,  get_pitch_bend_lsb(293),  get_pitch_bend_msb(293),
      status_bit | MIDI_MSG_TYPE_AFTERTOUCH_POLY  | TEST_CHANNEL_2_BITS,  MIDI_NOTE_A_4, 37,

      status_bit | MIDI_MSG_TYPE_SONG_POSITION_POINTER,                   (0x1ABC & 0x7f), (0x1ABC >> 7),

      status_bit | MIDI_MSG_TYPE_PITCH_BEND       | TEST_CHANNEL_1_BITS,  get_pitch_bend_lsb(293),
      status_bit | MIDI_MSG_TYPE_SYSTEM_RESET, 
                                                                                                    get_pitch_bend_msb(293),
      status_bit | MIDI_MSG_TYPE_AFTERTOUCH_POLY  | TEST_CHANNEL_2_BITS,  MIDI_NOTE_G_4, 3,

      status_bit | MIDI_MSG_TYPE_MTC_QUARTER_FRAME, (MIDI_QF_TYPE_SECONDS_LOW_NIBBLE << 4) | 3,
      status_bit | MIDI_MSG_TYPE_MTC_QUARTER_FRAME, 
      status_bit | MIDI_MSG_TYPE_TIMING_CLOCK, 
                                                    (MIDI_QF_TYPE_SECONDS_HIGH_NIBBLE << 4) | 5,
      status_bit | MIDI_MSG_TYPE_TUNE_REQUEST,
       
      status_bit | MIDI_MSG_TYPE_SYSEX_START, 0x0a, 0x1b, 0x2c, 0x3d, 0x4e, 0x5f, status_bit | MIDI_MSG_TYPE_SYSEX_STOP,

      status_bit | MIDI_MSG_TYPE_SYSEX_START, 0x0a, 
      status_bit | MIDI_MSG_TYPE_TIMING_CLOCK, 
                                                    0x1b, 0x2c, 
      status_bit | MIDI_MSG_TYPE_START, 
                                                                0x3d, 0x4e, 0x5f, 
      status_bit | MIDI_MSG_TYPE_TIMING_CLOCK, 
                                                                                  status_bit | MIDI_MSG_TYPE_SYSEX_STOP,

      status_bit | MIDI_MSG_TYPE_SYSEX_START, 0x0a, 0x1b, 0x2c, 0x3d, 0x4e, 0x5f, 0x6a, // missing sysex stop
      status_bit | MIDI_MSG_TYPE_NOTE_ON | TEST_CHANNEL_1_BITS,  MIDI_NOTE_G_8,    20,

                                                                                  status_bit | MIDI_MSG_TYPE_SYSEX_STOP, // ignore late sysex stop

      // clang-format on
  };

  const MIDI_Message expect_msgs[] = {
      // clang-format off
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = TEST_CHANNEL_1, .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 27}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = TEST_CHANNEL_1, .data.note_on = {.note = MIDI_NOTE_D_5, .velocity = 40}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = TEST_CHANNEL_1, .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 0}},
      {.type = MIDI_MSG_TYPE_START},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = TEST_CHANNEL_1, .data.note_on = {.note = MIDI_NOTE_F_2, .velocity = 29}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = TEST_CHANNEL_1, .data.note_on = {.note = MIDI_NOTE_G_8, .velocity = 20}},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = TEST_CHANNEL_2, .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 99}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = TEST_CHANNEL_2, .data.note_on = {.note = MIDI_NOTE_A_4, .velocity = 21}},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = TEST_CHANNEL_1, .data.note_off = {.note = MIDI_NOTE_D_5, .velocity = 100}},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = TEST_CHANNEL_1, .data.note_off = {.note = MIDI_NOTE_F_2, .velocity = 29}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = TEST_CHANNEL_2, .data.note_on = {.note = MIDI_NOTE_G_3, .velocity = 99}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = TEST_CHANNEL_1, .data.control_change = {.control = MIDI_CTRL_ATTACK_TIME, .value = 29}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = TEST_CHANNEL_1, .data.control_change = {.control = MIDI_CTRL_CUTOFF_FREQUENCY, .value = 99}},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = TEST_CHANNEL_1, .data.control_change = {.control = MIDI_CTRL_EFFECT1, .value = 20}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = TEST_CHANNEL_2, .data.program_change = {.program_id = 3}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = TEST_CHANNEL_2, .data.program_change = {.program_id = 28}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = TEST_CHANNEL_2, .data.program_change = {.program_id = 83}},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = TEST_CHANNEL_1, .data.note_off = {.note = MIDI_NOTE_G_8, .velocity = 19}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = TEST_CHANNEL_2, .data.control_change = {.control = MIDI_CTRL_MOD_WHEEL, .value = 29}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = TEST_CHANNEL_1, .data.program_change = {.program_id = 99}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = TEST_CHANNEL_1, .data.control_change = {.control = MIDI_CTRL_GENERAL_A, .value = 101}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = TEST_CHANNEL_1, .data.control_change = {.control = MIDI_CTRL_GENERAL_A_LSB, .value = 29}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = TEST_CHANNEL_1, .data.pitch_bend = {.value = 8000}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = TEST_CHANNEL_2, .data.pitch_bend = {.value = -2}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = TEST_CHANNEL_1, .data.pitch_bend = {.value = -5000}},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = TEST_CHANNEL_1, .data.pitch_bend = {.value = 0}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = TEST_CHANNEL_1, .data.pitch_bend = {.value = 5}},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .channel = TEST_CHANNEL_1, .data.aftertouch_mono = {.value = 4}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .channel = TEST_CHANNEL_2, .data.aftertouch_mono = {.value = 7}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .channel = TEST_CHANNEL_2, .data.aftertouch_mono = {.value = 24}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .channel = TEST_CHANNEL_2, .data.aftertouch_mono = {.value = 28}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = TEST_CHANNEL_2, .data.program_change = {.program_id = 27}},
      {.type = MIDI_MSG_TYPE_SONG_SELECT, .data.song_select = {.value = 19}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .channel = TEST_CHANNEL_1, .data.aftertouch_poly = {.note= MIDI_NOTE_G_8, .value=15}}, 
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .channel = TEST_CHANNEL_1, .data.aftertouch_poly = {.note= MIDI_NOTE_G_7, .value=18}}, 
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .channel = TEST_CHANNEL_1, .data.aftertouch_poly = {.note= MIDI_NOTE_F_3, .value=88}}, 
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = TEST_CHANNEL_1, .data.pitch_bend = {.value = 293}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .channel = TEST_CHANNEL_2, .data.aftertouch_poly = {.note= MIDI_NOTE_A_4, .value=37}},
      {.type = MIDI_MSG_TYPE_SONG_POSITION_POINTER, .data.song_position_pointer = {.value = 0x1ABC}},
      {.type = MIDI_MSG_TYPE_SYSTEM_RESET},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .channel = TEST_CHANNEL_2, .data.aftertouch_poly = {.note= MIDI_NOTE_G_4, .value=3}},
      {.type = MIDI_MSG_TYPE_MTC_QUARTER_FRAME, .data.quarter_frame = {.type= MIDI_QF_TYPE_SECONDS_LOW_NIBBLE, .value=3}},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_MTC_QUARTER_FRAME, .data.quarter_frame = {.type= MIDI_QF_TYPE_SECONDS_HIGH_NIBBLE, .value=5}},
      {.type = MIDI_MSG_TYPE_TUNE_REQUEST},
      {.type = MIDI_MSG_TYPE_SYSEX_START}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x0a, .sequence_number = 0}}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x1b, .sequence_number = 1}}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x2c, .sequence_number = 2}}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x3d, .sequence_number = 3}}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x4e, .sequence_number = 4}}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x5f, .sequence_number = 5}}, 
      {.type = MIDI_MSG_TYPE_SYSEX_STOP, .data.sysex_stop = {.sequence_length = 6, .is_length_overflowed=false}},
      {.type = MIDI_MSG_TYPE_SYSEX_START}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x0a, .sequence_number = 0}}, 
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x1b, .sequence_number = 1}}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x2c, .sequence_number = 2}}, 
      {.type = MIDI_MSG_TYPE_START},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x3d, .sequence_number = 3}}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x4e, .sequence_number = 4}}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x5f, .sequence_number = 5}}, 
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_SYSEX_STOP, .data.sysex_stop = {.sequence_length = 6, .is_length_overflowed=false}},
      {.type = MIDI_MSG_TYPE_SYSEX_START}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x0a, .sequence_number = 0}}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x1b, .sequence_number = 1}}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x2c, .sequence_number = 2}}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x3d, .sequence_number = 3}}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x4e, .sequence_number = 4}}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x5f, .sequence_number = 5}}, 
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x6a, .sequence_number = 6}}, 
      {.type = MIDI_MSG_TYPE_SYSEX_STOP, .data.sysex_stop = {.sequence_length = 7, .is_length_overflowed=false}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = TEST_CHANNEL_1, .data.note_on = {.note = MIDI_NOTE_G_8, .velocity = 20}},
      // clang-format on
  };

  check_input_and_output(&r,
                         __func__,
                         decoder,
                         bytes,
                         (sizeof(bytes) / sizeof(bytes[0])),
                         expect_msgs,
                         (sizeof(expect_msgs) / sizeof(expect_msgs[0])));

  return r;
}

static Result tst_random_byte_input(void * env) {
  Result         r       = PASS;
  MIDI_Decoder * decoder = (MIDI_Decoder *)env;

  printf("start test %s\n", __func__);

  // push many random bytes. decoder should never return any error

  const size_t num_bytes = 100000;

  for(size_t i = 0; i < num_bytes; i++) {
    const uint8_t byte = get_rand_u32(0, UINT8_MAX);

    EXPECT_OK(&r, MIDI_decoder_push_byte(decoder, byte));

    while(MIDI_decoder_has_output(decoder)) { MIDI_decoder_pop_msg(decoder); }
    if(HAS_FAILED(&r)) return r;
  }

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
      tst_song_position_pointer,
      tst_real_time,
      tst_real_time_with_running_status,
      tst_real_time_prio_mode,
      tst_song_select,
      tst_sysex_sequence,
      tst_sysex_sequence_with_realtime_interruptions,
      tst_sysex_sequence_with_length_overflow,
      tst_multiple_msgs,
      tst_random_byte_input,
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

  // use both time and clock so we get a different seed even if we call this many times per second
  setup_rand();

  MIDI_Decoder ** decoder = (MIDI_Decoder **)env_p;

  *decoder = malloc(sizeof(MIDI_Decoder));
  EXPECT_NE(&r, NULL, *decoder);
  if(HAS_FAILED(&r)) return r;

  EXPECT_EQ(&r, OK, MIDI_decoder_init(*decoder));

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

static void check_input_and_output(Result *             r_ptr,
                                   const char *         test_name,
                                   MIDI_Decoder *       decoder,
                                   const uint8_t *      input,
                                   size_t               input_n,
                                   const MIDI_Message * expect_output,
                                   size_t               expect_output_n) {
  EXPECT_NE(r_ptr, NULL, test_name);
  EXPECT_NE(r_ptr, NULL, decoder);
  EXPECT_NE(r_ptr, NULL, input);
  EXPECT_NE(r_ptr, NULL, expect_output);

  if(HAS_FAILED(r_ptr)) return;

  size_t input_idx = 0;

  for(size_t output_idx = 0; output_idx < expect_output_n; output_idx++) {
    for(; (input_idx < input_n) && MIDI_decoder_is_ready_to_receive(decoder); input_idx++) {
      EXPECT_EQ(r_ptr, OK, MIDI_decoder_push_byte(decoder, input[input_idx]));
      if(HAS_FAILED(r_ptr)) return;
    }

    const MIDI_Message expect = expect_output[output_idx];

    EXPECT_TRUE(r_ptr, MIDI_decoder_has_output(decoder));
    if(HAS_FAILED(r_ptr)) return;

    const MIDI_Message peek_res = MIDI_decoder_peek_msg(decoder);
    const MIDI_Message pop_res  = MIDI_decoder_pop_msg(decoder);
    EXPECT_TRUE(r_ptr, MIDI_message_equals(peek_res, pop_res));

    EXPECT_TRUE(r_ptr, MIDI_message_equals(expect, pop_res));

    if(HAS_FAILED(r_ptr)) {
      printf("failure in %s\n", test_name);
      printf("expected:\t");
      {
        char buff[1024] = {0};
        MIDI_message_to_str_buffer(buff, sizeof(buff) - 1, expect);
        printf("%s", buff);
      }
      printf("\nactual:\t\t");
      {
        char buff[1024] = {0};
        MIDI_message_to_str_buffer(buff, sizeof(buff) - 1, pop_res);
        printf("%s", buff);
      }
      printf("\n");
    }
  }

  EXPECT_EQ(r_ptr, input_n, input_idx);
  EXPECT_FALSE(r_ptr, MIDI_decoder_has_output(decoder));
}