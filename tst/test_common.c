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

#include "test_common.h"

#include <stdlib.h>
#include <time.h>

#include "message.h"

#define RAND_FROM_ARR(arr) (arr)[get_rand_u32(0, (sizeof((arr)) / sizeof((arr)[0])) - 1)]

int32_t get_rand_i32(int32_t min, int32_t max) {
  const int64_t range      = ((int64_t)max - (int64_t)min) + 1;
  const double  val_double = (((double)rand() / (double)RAND_MAX) * range) + min;
  return (int32_t)val_double;
}

uint32_t get_rand_u32(uint32_t min, uint32_t max) {
  const int64_t range      = ((int64_t)max - (int64_t)min) + 1;
  const double  val_double = (((double)rand() / (double)RAND_MAX) * range) + min;
  return (uint32_t)val_double;
}

uint8_t  get_rand_u7(void) { return get_rand_u32(0, 0x7f); }
uint16_t get_rand_u14(void) { return get_rand_u32(0, (1 << 14) - 1); }

int16_t get_rand_pitch_bend_value(void) {
  const int16_t mid = (0x40 << 7);
  return (int16_t)get_rand_u14() - mid;
}

MIDI_Note    get_rand_note(void) { return (MIDI_Note)get_rand_u32(MIDI_NOTE_LOWEST, MIDI_NOTE_HIGHEST); }
MIDI_Channel get_rand_channel(void) { return get_rand_u32(1, 16); }

MIDI_MessageType get_rand_basic_non_realtime_msg_type(void) {
  // basic means no sysex and no system reset
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
  };
  return RAND_FROM_ARR(types);
}

MIDI_MessageType get_rand_basic_realtime_msg_type(void) {
  // basic means no system reset
  const MIDI_MessageType types[] = {
      MIDI_MSG_TYPE_TIMING_CLOCK,
      MIDI_MSG_TYPE_START,
      MIDI_MSG_TYPE_CONTINUE,
      MIDI_MSG_TYPE_STOP,
      MIDI_MSG_TYPE_ACTIVE_SENSING,
  };
  return RAND_FROM_ARR(types);
}

MIDI_MessageType get_rand_basic_msg_type(void) {
  // basic means no sysex and no system reset
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
  return RAND_FROM_ARR(types);
}

MIDI_QuarterFrameType get_rand_qf_type(void) {
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
  return RAND_FROM_ARR(types);
}

static MIDI_Message get_rand_message_of_type(MIDI_MessageType type) {
  MIDI_Message msg = {.type = type};

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
  default: break; // non-valued message
  }

  return msg;
}

MIDI_Message get_rand_basic_message(void) {
  // basic means no sysex and no system reset
  return get_rand_message_of_type(get_rand_basic_msg_type());
}

MIDI_Message get_rand_basic_non_realtime_message(void) {
  return get_rand_message_of_type(get_rand_basic_non_realtime_msg_type());
}

MIDI_Message get_rand_basic_realtime_message(void) {
  return get_rand_message_of_type(get_rand_basic_realtime_msg_type());
}

void setup_rand(void) {
  // use both time and clock to get different seeds even when called many times per second
  srand(time(NULL) + clock());
}

uint8_t get_pitch_bend_lsb(int16_t value) {
  return value & 0x7f; // 0b0111'1111
}
uint8_t get_pitch_bend_msb(int16_t value) {
  const int16_t mid_point = (0x40) << 7;
  return ((value + mid_point) >> 7) & 0x7f; // 0b0111'1111;
}
int16_t make_pitch_bend_value(uint8_t lsb, uint8_t msb) {
  const int16_t mid = 0x40 << 7;
  return (((int16_t)(msb) << 7) | (int16_t)lsb) - mid;
}

uint16_t make_song_position_pointer_value(uint8_t lsb, uint8_t msb) { return ((msb & 0x7f) << 7) | (lsb & 0x7f); }