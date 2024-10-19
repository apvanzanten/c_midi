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

#ifndef C_MIDI_MESSAGE_H
#define C_MIDI_MESSAGE_H

#include <stdbool.h>
#include <stdint.h>

#include "control.h"
#include "note.h"

typedef enum MIDI_MessageType {
  // NOTE excludes status bit

  // channel types
  MIDI_MSG_TYPE_NOTE_OFF        = 0x00,
  MIDI_MSG_TYPE_NOTE_ON         = 0x10,
  MIDI_MSG_TYPE_AFTERTOUCH_POLY = 0x20,
  MIDI_MSG_TYPE_CONTROL_CHANGE  = 0x30,
  MIDI_MSG_TYPE_PROGRAM_CHANGE  = 0x40,
  MIDI_MSG_TYPE_AFTERTOUCH_MONO = 0x50,
  MIDI_MSG_TYPE_PITCH_BEND      = 0x60,

  // system types, all with first nibble set to 7
  MIDI_MSG_TYPE_MTC_QUARTER_FRAME     = 0x71,
  MIDI_MSG_TYPE_SONG_POSITION_POINTER = 0x72,
  MIDI_MSG_TYPE_SONG_SELECT           = 0x73,
  MIDI_MSG_TYPE_TUNE_REQUEST          = 0x76,

  MIDI_MSG_TYPE_SYSEX_START = 0x70,
  MIDI_MSG_TYPE_SYSEX_STOP  = 0x77,

  // system real-time
  MIDI_MSG_TYPE_TIMING_CLOCK   = 0x78,
  MIDI_MSG_TYPE_START          = 0x7a,
  MIDI_MSG_TYPE_CONTINUE       = 0x7b,
  MIDI_MSG_TYPE_STOP           = 0x7c,
  MIDI_MSG_TYPE_ACTIVE_SENSING = 0x7e,
  MIDI_MSG_TYPE_SYSTEM_RESET   = 0x7f,
} MIDI_MessageType;

static inline uint8_t      MIDI_type_to_byte(MIDI_MessageType type) { return (uint8_t)type; }
static inline const char * MIDI_message_type_to_str(MIDI_MessageType t);
static inline bool         MIDI_is_channel_type(MIDI_MessageType type);
static inline bool         MIDI_is_system_type(MIDI_MessageType type);
static inline bool         MIDI_is_real_time_type(MIDI_MessageType type);
static inline bool         MIDI_is_single_byte_type(MIDI_MessageType type);

typedef struct MIDI_NoteOff {
  uint8_t note; // value of MIDI_Note
  uint8_t velocity;
} MIDI_NoteOff;

typedef struct MIDI_NoteOn {
  uint8_t note; // value of MIDI_Note
  uint8_t velocity;
} MIDI_NoteOn;

typedef struct MIDI_ControlChange {
  uint8_t control; // value of MIDI_Note
  uint8_t value;
} MIDI_ControlChange;

typedef struct MIDI_ProgramChange {
  uint8_t program_id;
} MIDI_ProgramChange;

typedef struct MIDI_PitchBend {
  int16_t value;
} MIDI_PitchBend;

typedef struct MIDI_AftertouchMono {
  uint8_t value;
} MIDI_AftertouchMono;

typedef struct MIDI_AftertouchPoly {
  uint8_t note; // value of MIDI_Note
  uint8_t value;
} MIDI_AftertouchPoly;

typedef uint8_t MIDI_Channel;

typedef enum MIDI_QuarterFrameType {
  MIDI_QF_TYPE_FRAME_LOW_NIBBLE = 0,
  MIDI_QF_TYPE_FRAME_HIGH_NIBBLE,
  MIDI_QF_TYPE_SECONDS_LOW_NIBBLE,
  MIDI_QF_TYPE_SECONDS_HIGH_NIBBLE,
  MIDI_QF_TYPE_MINUTES_LOW_NIBBLE,
  MIDI_QF_TYPE_MINUTES_HIGH_NIBBLE,
  MIDI_QF_TYPE_HOURS_LOW_NIBBLE,
  MIDI_QF_TYPE_HOURS_HIGH_NIBBLE,
} MIDI_QuarterFrameType;

static inline const char * MIDI_quarter_frame_type_to_str(MIDI_QuarterFrameType type);

typedef struct MIDI_QuarterFrame {
  uint8_t type : 4; // value of MIDI_QuarterFrameType
  uint8_t value : 4;
} MIDI_QuarterFrame;

typedef struct MIDI_Message {
  bool         is_non_standard_msg : 1; // currently unused, reserved for hacking our way into supporting sysex
  uint8_t      type : 7;                // value of MIDI_MessageType
  MIDI_Channel channel : 5;             // [1,16]
  uint8_t      reserved : 3;            // reserved for who knows what

  union {
    MIDI_NoteOff        note_off;
    MIDI_NoteOn         note_on;
    MIDI_ControlChange  control_change;
    MIDI_ProgramChange  program_change;
    MIDI_PitchBend      pitch_bend;
    MIDI_AftertouchMono aftertouch_mono;
    MIDI_AftertouchPoly aftertouch_poly;
    MIDI_QuarterFrame   quarter_frame;
  } data;
} MIDI_Message;

static inline bool MIDI_is_channel_msg(MIDI_Message msg);
static inline bool MIDI_is_system_msg(MIDI_Message msg);
static inline bool MIDI_is_real_time_msg(MIDI_Message msg);
static inline bool MIDI_is_single_byte_msg(MIDI_Message msg);

int MIDI_note_off_msg_to_str_buffer(char * str, int max_len, MIDI_NoteOff msg);
int MIDI_note_on_msg_to_str_buffer(char * str, int max_len, MIDI_NoteOn msg);
int MIDI_control_change_msg_to_str_buffer(char * str, int max_len, MIDI_ControlChange msg);
int MIDI_program_change_msg_to_str_buffer(char * str, int max_len, MIDI_ProgramChange msg);
int MIDI_pitch_bend_msg_to_str_buffer(char * str, int max_len, MIDI_PitchBend msg);
int MIDI_aftertouch_mono_msg_to_str_buffer(char * str, int max_len, MIDI_AftertouchMono msg);
int MIDI_aftertouch_poly_msg_to_str_buffer(char * str, int max_len, MIDI_AftertouchPoly msg);
int MIDI_quarter_frame_msg_to_str_buffer(char * str, int max_len, MIDI_QuarterFrame msg);

int MIDI_note_off_msg_to_str_buffer_short(char * str, int max_len, MIDI_NoteOff msg);
int MIDI_note_on_msg_to_str_buffer_short(char * str, int max_len, MIDI_NoteOn msg);
int MIDI_control_change_msg_to_str_buffer_short(char * str, int max_len, MIDI_ControlChange msg);
int MIDI_program_change_msg_to_str_buffer_short(char * str, int max_len, MIDI_ProgramChange msg);
int MIDI_pitch_bend_msg_to_str_buffer_short(char * str, int max_len, MIDI_PitchBend msg);
int MIDI_aftertouch_mono_msg_to_str_buffer_short(char * str, int max_len, MIDI_AftertouchMono msg);
int MIDI_aftertouch_poly_msg_to_str_buffer_short(char * str, int max_len, MIDI_AftertouchPoly msg);
int MIDI_quarter_frame_msg_to_str_buffer_short(char * str, int max_len, MIDI_QuarterFrame msg);

int MIDI_message_to_str_buffer(char * str, int max_len, MIDI_Message msg);
int MIDI_message_to_str_buffer_short(char * str, int max_len, MIDI_Message msg);

bool MIDI_message_equals(MIDI_Message lhs, MIDI_Message rhs);

bool MIDI_note_on_msg_equals(MIDI_NoteOn lhs, MIDI_NoteOn rhs);
bool MIDI_note_off_msg_equals(MIDI_NoteOff lhs, MIDI_NoteOff rhs);
bool MIDI_control_change_msg_equals(MIDI_ControlChange lhs, MIDI_ControlChange rhs);
bool MIDI_program_change_msg_equals(MIDI_ProgramChange lhs, MIDI_ProgramChange rhs);
bool MIDI_pitch_bend_msg_equals(MIDI_PitchBend lhs, MIDI_PitchBend rhs);
bool MIDI_aftertouch_mono_msg_equals(MIDI_AftertouchMono lhs, MIDI_AftertouchMono rhs);
bool MIDI_aftertouch_poly_msg_equals(MIDI_AftertouchPoly lhs, MIDI_AftertouchPoly rhs);
bool MIDI_quarter_frame_msg_equals(MIDI_QuarterFrame lhs, MIDI_QuarterFrame rhs);

static inline const char * MIDI_message_type_to_str(MIDI_MessageType t) {
  switch(t) {
  case MIDI_MSG_TYPE_NOTE_OFF: return "NOTE_OFF";
  case MIDI_MSG_TYPE_NOTE_ON: return "NOTE_ON";
  case MIDI_MSG_TYPE_AFTERTOUCH_POLY: return "AFTERTOUCH_POLY";
  case MIDI_MSG_TYPE_CONTROL_CHANGE: return "CONTROL_CHANGE";
  case MIDI_MSG_TYPE_PROGRAM_CHANGE: return "PROGRAM_CHANGE";
  case MIDI_MSG_TYPE_AFTERTOUCH_MONO: return "AFTERTOUCH_MONO";
  case MIDI_MSG_TYPE_PITCH_BEND: return "PITCH_BEND";
  case MIDI_MSG_TYPE_MTC_QUARTER_FRAME: return "MTC_QUARTER_FRAME";
  case MIDI_MSG_TYPE_SONG_POSITION_POINTER: return "SONG_POSITION_POINTER";
  case MIDI_MSG_TYPE_SONG_SELECT: return "SONG_SELECT";
  case MIDI_MSG_TYPE_TUNE_REQUEST: return "TUNE_REQUEST";
  case MIDI_MSG_TYPE_SYSEX_START: return "SYSEX_START";
  case MIDI_MSG_TYPE_SYSEX_STOP: return "SYSEX_STOP";
  case MIDI_MSG_TYPE_TIMING_CLOCK: return "TIMING_CLOCK";
  case MIDI_MSG_TYPE_START: return "START";
  case MIDI_MSG_TYPE_CONTINUE: return "CONTINUE";
  case MIDI_MSG_TYPE_STOP: return "STOP";
  case MIDI_MSG_TYPE_ACTIVE_SENSING: return "ACTIVE_SENSING";
  case MIDI_MSG_TYPE_SYSTEM_RESET: return "SYSTEM_RESET";
  }
  return "UNKNOWN";
}

static inline bool MIDI_is_channel_type(MIDI_MessageType type) {
  switch(type) {
  case MIDI_MSG_TYPE_NOTE_OFF: return true;
  case MIDI_MSG_TYPE_NOTE_ON: return true;
  case MIDI_MSG_TYPE_AFTERTOUCH_POLY: return true;
  case MIDI_MSG_TYPE_CONTROL_CHANGE: return true;
  case MIDI_MSG_TYPE_PROGRAM_CHANGE: return true;
  case MIDI_MSG_TYPE_AFTERTOUCH_MONO: return true;
  case MIDI_MSG_TYPE_PITCH_BEND: return true;
  default: return false;
  }
}

static inline bool MIDI_is_system_type(MIDI_MessageType type) {
  switch(type) {
  case MIDI_MSG_TYPE_MTC_QUARTER_FRAME: return true;
  case MIDI_MSG_TYPE_SONG_POSITION_POINTER: return true;
  case MIDI_MSG_TYPE_SONG_SELECT: return true;
  case MIDI_MSG_TYPE_TUNE_REQUEST: return true;
  case MIDI_MSG_TYPE_SYSEX_START: return true;
  case MIDI_MSG_TYPE_SYSEX_STOP: return true;
  case MIDI_MSG_TYPE_TIMING_CLOCK: return true;
  case MIDI_MSG_TYPE_START: return true;
  case MIDI_MSG_TYPE_CONTINUE: return true;
  case MIDI_MSG_TYPE_STOP: return true;
  case MIDI_MSG_TYPE_ACTIVE_SENSING: return true;
  case MIDI_MSG_TYPE_SYSTEM_RESET: return true;
  default: return false;
  }
}

static inline bool MIDI_is_real_time_type(MIDI_MessageType type) {
  switch(type) {
  case MIDI_MSG_TYPE_TIMING_CLOCK: return true;
  case MIDI_MSG_TYPE_START: return true;
  case MIDI_MSG_TYPE_CONTINUE: return true;
  case MIDI_MSG_TYPE_STOP: return true;
  case MIDI_MSG_TYPE_ACTIVE_SENSING: return true;
  case MIDI_MSG_TYPE_SYSTEM_RESET: return true;
  default: return false;
  }
}

static inline bool MIDI_is_single_byte_type(MIDI_MessageType type) {
  if(MIDI_is_real_time_type(type)) return true;
  switch(type) {
  case MIDI_MSG_TYPE_TUNE_REQUEST: return true;
  case MIDI_MSG_TYPE_SYSEX_STOP: return true;
  default: return false;
  }
}

static inline bool MIDI_is_channel_msg(MIDI_Message msg) { return MIDI_is_channel_type(msg.type); }
static inline bool MIDI_is_system_msg(MIDI_Message msg) { return MIDI_is_system_type(msg.type); }
static inline bool MIDI_is_real_time_msg(MIDI_Message msg) { return MIDI_is_real_time_type(msg.type); }
static inline bool MIDI_is_single_byte_msg(MIDI_Message msg) { return MIDI_is_single_byte_type(msg.type); }

static inline const char * MIDI_quarter_frame_type_to_str(MIDI_QuarterFrameType type) {
  switch(type) {
  case MIDI_QF_TYPE_FRAME_LOW_NIBBLE: return "FRAME_LOW_NIBBLE";
  case MIDI_QF_TYPE_FRAME_HIGH_NIBBLE: return "FRAME_HIGH_NIBBLE";
  case MIDI_QF_TYPE_SECONDS_LOW_NIBBLE: return "SECONDS_LOW_NIBBLE";
  case MIDI_QF_TYPE_SECONDS_HIGH_NIBBLE: return "SECONDS_HIGH_NIBBLE";
  case MIDI_QF_TYPE_MINUTES_LOW_NIBBLE: return "MINUTES_LOW_NIBBLE";
  case MIDI_QF_TYPE_MINUTES_HIGH_NIBBLE: return "MINUTES_HIGH_NIBBLE";
  case MIDI_QF_TYPE_HOURS_LOW_NIBBLE: return "HOURS_LOW_NIBBLE";
  case MIDI_QF_TYPE_HOURS_HIGH_NIBBLE: return "HOURS_HIGH_NIBBLE";
  }
  return "UNKNOWN";
}

static inline const char * MIDI_quarter_frame_type_to_str_short(MIDI_QuarterFrameType type) {
  switch(type) {
  case MIDI_QF_TYPE_FRAME_LOW_NIBBLE: return "FRM_L";
  case MIDI_QF_TYPE_FRAME_HIGH_NIBBLE: return "FRM_H";
  case MIDI_QF_TYPE_SECONDS_LOW_NIBBLE: return "SEC_L";
  case MIDI_QF_TYPE_SECONDS_HIGH_NIBBLE: return "SEC_H";
  case MIDI_QF_TYPE_MINUTES_LOW_NIBBLE: return "MIN_L";
  case MIDI_QF_TYPE_MINUTES_HIGH_NIBBLE: return "MIN_H";
  case MIDI_QF_TYPE_HOURS_LOW_NIBBLE: return "HRS_L";
  case MIDI_QF_TYPE_HOURS_HIGH_NIBBLE: return "HRS_H";
  }
  return "UNKNOWN";
}

#endif
