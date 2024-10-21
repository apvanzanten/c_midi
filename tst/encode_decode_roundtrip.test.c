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

static MIDI_Message get_random_basic_message(void);

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
    const MIDI_Message msg = get_random_basic_message();
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

  srand(time(NULL) + clock());

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

static uint32_t get_rand_u32(uint32_t min, uint32_t max) {
  const int64_t range      = ((int64_t)max - (int64_t)min) + 1;
  const double  val_double = (((double)rand() / (double)RAND_MAX) * range) + min;
  return (uint32_t)val_double;
}

static uint8_t  get_rand_u7(void) { return get_rand_u32(0, 0x7f); }
static uint16_t get_rand_u14(void) { return get_rand_u32(0, (1 << 14) - 1); }

static int16_t get_rand_pitch_bend_value(void) {
  const int16_t mid = (0x40 << 7);
  return (int16_t)get_rand_u14() - mid;
}

static MIDI_Note    get_rand_note(void) { return (MIDI_Note)get_rand_u32(MIDI_NOTE_LOWEST, MIDI_NOTE_HIGHEST); }
static MIDI_Channel get_rand_channel(void) { return get_rand_u32(1, 16); }

static MIDI_MessageType get_rand_basic_msg_type(void) {
  const MIDI_MessageType types[] = {
      MIDI_MSG_TYPE_NOTE_OFF,
      MIDI_MSG_TYPE_NOTE_ON,
      MIDI_MSG_TYPE_AFTERTOUCH_POLY,
      MIDI_MSG_TYPE_CONTROL_CHANGE,
      MIDI_MSG_TYPE_PROGRAM_CHANGE,
      MIDI_MSG_TYPE_AFTERTOUCH_MONO,
      MIDI_MSG_TYPE_PITCH_BEND,
      MIDI_MSG_TYPE_MTC_QUARTER_FRAME,
      MIDI_MSG_TYPE_SONG_POSITION_POINTER,
      MIDI_MSG_TYPE_SONG_SELECT,
      MIDI_MSG_TYPE_TUNE_REQUEST,
      MIDI_MSG_TYPE_TIMING_CLOCK,
      MIDI_MSG_TYPE_START,
      MIDI_MSG_TYPE_CONTINUE,
      MIDI_MSG_TYPE_STOP,
      MIDI_MSG_TYPE_ACTIVE_SENSING,
  };

  return types[get_rand_u32(0, (sizeof(types) / sizeof(types[0])) - 1)];
}

static MIDI_QuarterFrameType get_rand_qf_type(void) {
  const MIDI_QuarterFrameType types[] = {
      MIDI_QF_TYPE_FRAME_LOW_NIBBLE,
      MIDI_QF_TYPE_FRAME_HIGH_NIBBLE,
      MIDI_QF_TYPE_SECONDS_LOW_NIBBLE,
      MIDI_QF_TYPE_SECONDS_HIGH_NIBBLE,
      MIDI_QF_TYPE_MINUTES_LOW_NIBBLE,
      MIDI_QF_TYPE_MINUTES_HIGH_NIBBLE,
      MIDI_QF_TYPE_HOURS_LOW_NIBBLE,
      MIDI_QF_TYPE_HOURS_HIGH_NIBBLE,
  };

  return types[get_rand_u32(0, (sizeof(types) / sizeof(types[0])) - 1)];
}

static MIDI_Message get_random_basic_message(void) {
  // basic means no sysex and no system reset
  MIDI_Message msg = {0};

  msg.type = get_rand_basic_msg_type();

  if(MIDI_is_single_byte_type(msg.type)) return msg;

  if(MIDI_is_channel_type(msg.type)) msg.channel = get_rand_channel();

  switch(msg.type) {
  case MIDI_MSG_TYPE_NOTE_OFF:
    msg.data.note_off.note     = get_rand_note();
    msg.data.note_off.velocity = get_rand_u7();
    break;
  case MIDI_MSG_TYPE_NOTE_ON:
    msg.data.note_on.note     = get_rand_note();
    msg.data.note_on.velocity = get_rand_u7();
    break;
  case MIDI_MSG_TYPE_AFTERTOUCH_POLY:
    msg.data.aftertouch_poly.note  = get_rand_note();
    msg.data.aftertouch_poly.value = get_rand_u7();
    break;
  case MIDI_MSG_TYPE_CONTROL_CHANGE:
    msg.data.control_change.control = get_rand_u7();
    msg.data.control_change.value   = get_rand_u7();
    break;
  case MIDI_MSG_TYPE_PROGRAM_CHANGE: msg.data.program_change.program_id = get_rand_u7(); break;
  case MIDI_MSG_TYPE_AFTERTOUCH_MONO: msg.data.aftertouch_mono.value = get_rand_u7(); break;
  case MIDI_MSG_TYPE_PITCH_BEND: msg.data.pitch_bend.value = get_rand_pitch_bend_value(); break;
  case MIDI_MSG_TYPE_MTC_QUARTER_FRAME:
    msg.data.quarter_frame.type  = get_rand_qf_type();
    msg.data.quarter_frame.value = get_rand_u32(0, 0xf);
    break;
  case MIDI_MSG_TYPE_SONG_POSITION_POINTER:
    msg.data.song_position_pointer.value = get_rand_u32(0, get_rand_u14());
    break;
  case MIDI_MSG_TYPE_SONG_SELECT: msg.data.song_select.value = get_rand_u7(); break;
  default: printf("invalid message type! %u\n", msg.type); break;
  }

  return msg;
}