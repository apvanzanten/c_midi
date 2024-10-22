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
#include "message.h"
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
                                                  DAR_DArray *   input_msgs,
                                                  bool           is_verbose) {
  DAR_DArray bytes = {0};
  EXPECT_OK(r_ptr, DAR_create(&bytes, sizeof(uint8_t)));

  DAR_DArray output = {0};
  EXPECT_OK(r_ptr, DAR_create(&output, sizeof(MIDI_Message)));

#define REPORT_AND_RETURN()                                                                                            \
  do {                                                                                                                 \
    printf("%s fail\n", tst_name);                                                                                     \
    DAR_destroy(&bytes);                                                                                               \
    DAR_destroy(&output);                                                                                              \
    return;                                                                                                            \
  } while(false)

  size_t encode_idx = 0;
  size_t decode_idx = 0;

  while((output.size < input_msgs->size) && !HAS_FAILED(r_ptr)) {
    if((encode_idx < input_msgs->size) && MIDI_encoder_is_ready_to_receive(encoder)) {
      const MIDI_Message msg = *((MIDI_Message *)DAR_get(input_msgs, encode_idx));
      EXPECT_OK(r_ptr, MIDI_encoder_push_message(encoder, msg));
      encode_idx++;

      if(is_verbose) {
        char str[64] = "";
        MIDI_message_to_str_buffer_short(str, 64, msg);
        printf("in   %s (%zu out of %zu)\n", str, encode_idx, input_msgs->size);
      }

    } else if((decode_idx < bytes.size) && MIDI_decoder_is_ready_to_receive(decoder)) {
      const uint8_t byte = *((uint8_t *)DAR_get(&bytes, decode_idx));
      EXPECT_OK(r_ptr, MIDI_decoder_push_byte(decoder, byte));
      decode_idx++;

    } else if(MIDI_encoder_has_output(encoder)) {
      const uint8_t byte = MIDI_encoder_pop_byte(encoder);
      EXPECT_OK(r_ptr, DAR_push_back(&bytes, &byte));

      // if(is_verbose) printf("pop  0x%x/0d%u (%zu)\n", byte, byte, bytes.size);
      if(is_verbose) {
        printf("byte 0x%x/0d%u %s (%zu)\n",
               byte,
               byte,
               (0x80 & byte) ? (((byte & 0x7f) > 0x60) ? MIDI_message_type_to_str(byte & 0x7f)
                                                       : MIDI_message_type_to_str(byte & 0x70))
                             : "",
               bytes.size);
      }

    } else if(MIDI_decoder_has_output(decoder)) {
      const MIDI_Message msg = MIDI_decoder_pop_msg(decoder);
      EXPECT_OK(r_ptr, DAR_push_back(&output, &msg));

      if(is_verbose) {
        char str[64] = "";
        MIDI_message_to_str_buffer_short(str, 64, msg);
        printf("out  %s (%zu out of %zu)\n", str, output.size, input_msgs->size);
      }

    } else {
      EXPECT_FALSE(r_ptr, true);
      printf("encoded %zu/%zu, decoded %zu/%zu, roundtripped %zu/%zu\n",
             encode_idx,
             input_msgs->size,
             decode_idx,
             bytes.size,
             output.size,
             input_msgs->size);
    }
    if(HAS_FAILED(r_ptr)) REPORT_AND_RETURN();
  }

  EXPECT_EQ(r_ptr, output.size, input_msgs->size);

  EXPECT_TRUE(r_ptr, MIDI_decoder_is_ready_to_receive(decoder));
  EXPECT_TRUE(r_ptr, MIDI_encoder_is_ready_to_receive(encoder));
  EXPECT_FALSE(r_ptr, MIDI_decoder_has_output(decoder));
  EXPECT_FALSE(r_ptr, MIDI_encoder_has_output(encoder));

  if(HAS_FAILED(r_ptr)) REPORT_AND_RETURN();

  // check presence and order of realtime and non-realtime messages
  {
    size_t last_non_rt_idx = 0;
    size_t last_rt_idx     = 0;
    size_t non_rt_count    = 0;
    size_t rt_count        = 0;

    for(size_t in_idx = 0; in_idx < input_msgs->size; in_idx++) {
      const MIDI_Message in_msg = *(MIDI_Message *)DAR_get(input_msgs, in_idx);

      if(is_verbose) {
        char str[64] = "";
        MIDI_message_to_str_buffer_short(str, 64, in_msg);
        printf("%s (#%zu): ", str, in_idx);
      }

      if(MIDI_is_real_time_msg(in_msg)) {
        for(size_t out_idx = (in_idx == 0) ? 0 : last_rt_idx + 1; out_idx < output.size; out_idx++) {
          const MIDI_Message msg = *((MIDI_Message *)DAR_get(&output, out_idx));

          if(MIDI_message_equals(msg, in_msg)) {
            last_rt_idx = out_idx;
            rt_count++;
            if(is_verbose) printf("rt #%zu at %zu", rt_count, out_idx);
            break;
          }
        }
      } else {
        for(size_t out_idx = (in_idx == 0) ? 0 : last_non_rt_idx + 1; out_idx < output.size; out_idx++) {
          const MIDI_Message msg = *((MIDI_Message *)DAR_get(&output, out_idx));

          if(MIDI_message_equals(msg, in_msg)) {
            last_non_rt_idx = out_idx;
            non_rt_count++;
            if(is_verbose) printf("non-rt #%zu at %zu", non_rt_count, out_idx);
            break;
          }
        }
      }
      if(is_verbose) printf("\n");
    }
    EXPECT_EQ(r_ptr, input_msgs->size, non_rt_count + rt_count);
  }

  EXPECT_OK(r_ptr, DAR_destroy(&bytes));
  EXPECT_OK(r_ptr, DAR_destroy(&output));
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
  EXPECT_TRUE(&r, MIDI_encoder_is_ready_to_receive(encoder));

  EXPECT_FALSE(&r, MIDI_decoder_has_output(decoder));
  EXPECT_TRUE(&r, MIDI_decoder_is_ready_to_receive(decoder));

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

  expect_same_messages_across_roundtrip(&r, __func__, encoder, decoder, &input_arr, false);

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

  expect_same_messages_across_roundtrip(&r, __func__, encoder, decoder, &input_arr, false);

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
