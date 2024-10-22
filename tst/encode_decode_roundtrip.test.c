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
#include <time.h>

#include <cfac/darray.h>

#define OK STAT_OK

#include "test_common.h"

#include "decoder.h"
#include "encoder.h"
#include "note.h"

typedef struct Fixture {
  MIDI_Encoder encoder;
  MIDI_Decoder decoder;
} Fixture;

static Result setup(void ** env_p);
static Result teardown(void ** env_p);

static void expect_same_messages_across_roundtrip(Result *       r_ptr,
                                                  const char *   tst_name,
                                                  MIDI_Encoder * encoder,
                                                  MIDI_Decoder * decoder,
                                                  DAR_DArray *   input_msgs) {

#define ASSERT_NOT_FAILED()                                                                                            \
  do {                                                                                                                 \
    if(HAS_FAILED(r_ptr)) {                                                                                            \
      printf("%s failed on line %d\n", tst_name, __LINE__);                                                            \
      char in_str[1024] = {0};                                                                                         \
      MIDI_message_to_str_buffer(in_str, 1024, *in_msg);                                                               \
      printf("current message: %s\n", in_str);                                                                         \
      return;                                                                                                          \
    }                                                                                                                  \
  } while(false)

  for(const MIDI_Message * in_msg = DAR_first(input_msgs); in_msg != DAR_end(input_msgs); in_msg++) {
    EXPECT_FALSE(r_ptr, MIDI_encoder_has_output(encoder));
    EXPECT_TRUE(r_ptr, MIDI_encoder_is_ready(encoder));
    EXPECT_FALSE(r_ptr, MIDI_decoder_has_output(decoder));
    EXPECT_TRUE(r_ptr, MIDI_decoder_is_ready(decoder));
    ASSERT_NOT_FAILED();

    EXPECT_OK(r_ptr, MIDI_encoder_push_message(encoder, *in_msg));
    ASSERT_NOT_FAILED();

    while(MIDI_encoder_has_output(encoder)) {
      EXPECT_OK(r_ptr, MIDI_decoder_push_byte(decoder, MIDI_encoder_pop_byte(encoder)));
      ASSERT_NOT_FAILED();
    }

    EXPECT_TRUE(r_ptr, MIDI_decoder_has_output(decoder));
    ASSERT_NOT_FAILED();

    const MIDI_Message out_msg = MIDI_decoder_pop_msg(decoder);
    EXPECT_TRUE(r_ptr, MIDI_message_equals(*in_msg, out_msg));

    if(HAS_FAILED(r_ptr)) {
      char in_str[1024]  = {0};
      char out_str[1024] = {0};

      MIDI_message_to_str_buffer(in_str, 1024, *in_msg);
      MIDI_message_to_str_buffer(out_str, 1024, out_msg);

      printf("unequal messages encountered in %s. in: %s, out: %s\n", tst_name, in_str, out_str);
      return;
    }

    EXPECT_FALSE(r_ptr, MIDI_decoder_has_output(decoder));
    ASSERT_NOT_FAILED();
  }

#undef ASSERT_NOT_FAILED
}

static Result tst_fixture(void * env) {
  Result          r       = PASS;
  const Fixture * fixture = (Fixture *)env;
  EXPECT_NE(&r, NULL, fixture);
  if(HAS_FAILED(&r)) return r;

  const MIDI_Encoder * encoder = (MIDI_Encoder *)&fixture->encoder;
  const MIDI_Decoder * decoder = (MIDI_Decoder *)&fixture->decoder;

  EXPECT_NE(&r, NULL, encoder);
  EXPECT_NE(&r, NULL, decoder);
  if(HAS_FAILED(&r)) return r;

  EXPECT_FALSE(&r, MIDI_encoder_has_output(encoder));
  EXPECT_TRUE(&r, MIDI_encoder_is_ready(encoder));

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));
  EXPECT_TRUE(&r, MIDI_decoder_is_ready(decoder));

  return r;
}

static Result tst_basic(void * env) {
  Result    r       = PASS;
  Fixture * fixture = (Fixture *)env;
  EXPECT_NE(&r, NULL, fixture);
  if(HAS_FAILED(&r)) return r;

  MIDI_Encoder * encoder = (MIDI_Encoder *)&fixture->encoder;
  MIDI_Decoder * decoder = (MIDI_Decoder *)&fixture->decoder;

  const MIDI_Message msgs[] = {
      // clang-format off
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1, .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 27}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1, .data.note_on = {.note = MIDI_NOTE_D_5, .velocity = 40}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1, .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 0}},
      {.type = MIDI_MSG_TYPE_START},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1, .data.note_on = {.note = MIDI_NOTE_F_2, .velocity = 29}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1, .data.note_on = {.note = MIDI_NOTE_G_8, .velocity = 20}},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 2, .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 99}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 2, .data.note_on = {.note = MIDI_NOTE_A_4, .velocity = 21}},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 1, .data.note_off = {.note = MIDI_NOTE_D_5, .velocity = 100}},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 1, .data.note_off = {.note = MIDI_NOTE_F_2, .velocity = 29}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 2, .data.note_on = {.note = MIDI_NOTE_G_3, .velocity = 99}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = 1, .data.control_change = {.control = MIDI_CTRL_ATTACK_TIME, .value = 29}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = 1, .data.control_change = {.control = MIDI_CTRL_CUTOFF_FREQUENCY, .value = 99}},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = 1, .data.control_change = {.control = MIDI_CTRL_EFFECT1, .value = 20}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = 2, .data.program_change = {.program_id = 3}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = 2, .data.program_change = {.program_id = 28}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = 2, .data.program_change = {.program_id = 83}},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 1, .data.note_off = {.note = MIDI_NOTE_G_8, .velocity = 19}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = 2, .data.control_change = {.control = MIDI_CTRL_MOD_WHEEL, .value = 29}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = 1, .data.program_change = {.program_id = 99}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = 1, .data.control_change = {.control = MIDI_CTRL_GENERAL_A, .value = 101}},
      {.type = MIDI_MSG_TYPE_CONTROL_CHANGE, .channel = 1, .data.control_change = {.control = MIDI_CTRL_GENERAL_A_LSB, .value = 29}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = 1, .data.pitch_bend = {.value = 8000}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = 2, .data.pitch_bend = {.value = -2}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = 1, .data.pitch_bend = {.value = -5000}},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = 1, .data.pitch_bend = {.value = 0}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = 1, .data.pitch_bend = {.value = 5}},
      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .channel = 1, .data.aftertouch_mono = {.value = 4}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .channel = 2, .data.aftertouch_mono = {.value = 7}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .channel = 2, .data.aftertouch_mono = {.value = 24}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .channel = 2, .data.aftertouch_mono = {.value = 28}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = 2, .data.program_change = {.program_id = 27}},
      {.type = MIDI_MSG_TYPE_SONG_SELECT, .data.song_select = {.value = 19}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .channel = 1, .data.aftertouch_poly = {.note= MIDI_NOTE_G_8, .value=15}}, 
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .channel = 1, .data.aftertouch_poly = {.note= MIDI_NOTE_G_7, .value=18}}, 
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .channel = 1, .data.aftertouch_poly = {.note= MIDI_NOTE_F_3, .value=88}}, 
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = 1, .data.pitch_bend = {.value = 293}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .channel = 2, .data.aftertouch_poly = {.note= MIDI_NOTE_A_4, .value=37}},
      {.type = MIDI_MSG_TYPE_SONG_POSITION_POINTER, .data.song_position_pointer = {.value = 0x1ABC}},
      {.type = MIDI_MSG_TYPE_SYSTEM_RESET},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_POLY, .channel = 2, .data.aftertouch_poly = {.note= MIDI_NOTE_G_4, .value=3}},
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
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1, .data.note_on = {.note = MIDI_NOTE_G_8, .velocity = 20}},
      // clang-format on
  };

  DAR_DArray input_arr = {0};
  EXPECT_OK(&r,
            DAR_create_from_span(&input_arr,
                                 (SPN_Span){.begin        = msgs,
                                            .element_size = sizeof(msgs[0]),
                                            .len          = (sizeof(msgs) / sizeof(msgs[0]))}));

  expect_same_messages_across_roundtrip(&r, __func__, encoder, decoder, &input_arr);

  EXPECT_OK(&r, DAR_destroy(&input_arr));

  return r;
}

static Result tst_basic_roundtrip_many_random(void * env) {
  Result    r       = PASS;
  Fixture * fixture = (Fixture *)env;
  EXPECT_NE(&r, NULL, fixture);
  if(HAS_FAILED(&r)) return r;

  MIDI_Encoder * encoder = (MIDI_Encoder *)&fixture->encoder;
  MIDI_Decoder * decoder = (MIDI_Decoder *)&fixture->decoder;

  const size_t num_messages = 10000;

  DAR_DArray input_arr = {0};
  EXPECT_OK(&r, DAR_create(&input_arr, sizeof(MIDI_Message)));
  EXPECT_OK(&r, DAR_reserve(&input_arr, num_messages));

  for(size_t i = 0; i < num_messages; i++) {
    const MIDI_Message msg = get_rand_basic_message();
    EXPECT_OK(&r, DAR_push_back(&input_arr, &msg));
    if(HAS_FAILED(&r)) return r;
  }

  expect_same_messages_across_roundtrip(&r, __func__, encoder, decoder, &input_arr);

  EXPECT_OK(&r, DAR_destroy(&input_arr));

  return r;
}

int main(void) {
  TestWithFixture tests_with_fixture[] = {
      tst_fixture,
      tst_basic,
      tst_basic_roundtrip_many_random,
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

  setup_rand();

  Fixture ** fixture = (Fixture **)env_p;

  *fixture = malloc(sizeof(Fixture));
  EXPECT_NE(&r, NULL, *fixture);
  if(HAS_FAILED(&r)) return r;

  EXPECT_EQ(&r, OK, MIDI_encoder_init(&(*fixture)->encoder));
  EXPECT_EQ(&r, OK, MIDI_decoder_init(&(*fixture)->decoder, MIDI_DECODER_PRIO_MODE_FIFO));

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
