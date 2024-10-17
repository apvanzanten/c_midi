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
  MIDI_MSG_TYPE_NOTE_OFF = 0,
  MIDI_MSG_TYPE_NOTE_ON,
  MIDI_MSG_TYPE_AFTERTOUCH_POLY,
  MIDI_MSG_TYPE_CONTROL_CHANGE,
  MIDI_MSG_TYPE_PROGRAM_CHANGE,
  MIDI_MSG_TYPE_AFTERTOUCH_MONO,
  MIDI_MSG_TYPE_PITCH_BEND,
  MIDI_MSG_TYPE_SYSTEM = 0x7,
} MIDI_MessageType;

static inline uint8_t      MIDI_type_to_byte(MIDI_MessageType type) { return (uint8_t)type; }
static inline const char * MIDI_message_type_to_str(MIDI_MessageType t);
static inline bool         MIDI_is_channel_type(MIDI_MessageType type);
static inline bool         MIDI_is_system_type(MIDI_MessageType type) { return type == MIDI_MSG_TYPE_SYSTEM; }

typedef enum MIDI_SystemMessageType {
  MIDI_MSG_TYPE_SYSEX = 0x0,

  // system common
  MIDI_MSG_TYPE_MTC_QUARTER_FRAME = 0x1,
  MIDI_MSG_TYPE_SONG_POSITION_POINTER,
  MIDI_MSG_TYPE_SONG_SELECT,

  MIDI_MSG_TYPE_TUNE_REQUEST = 0x6,
  MIDI_MSG_TYPE_END_OF_SYSEX,

  // system real-time
  MIDI_MSG_TYPE_TIMING_CLOCK = 0x8,
  MIDI_MSG_TYPE_START,
  MIDI_MSG_TYPE_CONTINUE,
  MIDI_MSG_TYPE_STOP,
  MIDI_MSG_TYPE_ACTIVE_SENSING,
  MIDI_MSG_TYPE_SYSTEM_RESET,
} MIDI_SystemMessageType;

static inline uint8_t      MIDI_system_type_to_byte(MIDI_SystemMessageType type) { return (uint8_t)type; }
static inline const char * MIDI_system_message_type_to_str(MIDI_SystemMessageType t);
static inline bool         MIDI_is_real_time_type(MIDI_SystemMessageType type);

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

typedef struct MIDI_SystemMessage {
  uint8_t type; // value of MIDI_SystemMessageType
  // union {
  //   // TODO
  // } data;
} MIDI_SystemMessage;

typedef struct MIDI_Message {
  uint8_t      type : 4;    // value of MIDI_MessageType
  MIDI_Channel channel : 4; // only used for channel type messages
  union {
    MIDI_NoteOff        note_off;
    MIDI_NoteOn         note_on;
    MIDI_ControlChange  control_change;
    MIDI_ProgramChange  program_change;
    MIDI_PitchBend      pitch_bend;
    MIDI_AftertouchMono aftertouch_mono;
    MIDI_AftertouchPoly aftertouch_poly;
    MIDI_SystemMessage  system_msg;
  } as;
} MIDI_Message;

int MIDI_note_off_msg_to_str_buffer(char * str, int max_len, MIDI_NoteOff msg);
int MIDI_note_on_msg_to_str_buffer(char * str, int max_len, MIDI_NoteOn msg);
int MIDI_control_change_msg_to_str_buffer(char * str, int max_len, MIDI_ControlChange msg);
int MIDI_program_change_msg_to_str_buffer(char * str, int max_len, MIDI_ProgramChange msg);
int MIDI_pitch_bend_msg_to_str_buffer(char * str, int max_len, MIDI_PitchBend msg);
int MIDI_aftertouch_mono_msg_to_str_buffer(char * str, int max_len, MIDI_AftertouchMono msg);
int MIDI_aftertouch_poly_msg_to_str_buffer(char * str, int max_len, MIDI_AftertouchPoly msg);
int MIDI_system_msg_to_str_buffer(char * str, int max_len, MIDI_SystemMessage msg);

int MIDI_note_off_msg_to_str_buffer_short(char * str, int max_len, MIDI_NoteOff msg);
int MIDI_note_on_msg_to_str_buffer_short(char * str, int max_len, MIDI_NoteOn msg);
int MIDI_control_change_msg_to_str_buffer_short(char * str, int max_len, MIDI_ControlChange msg);
int MIDI_program_change_msg_to_str_buffer_short(char * str, int max_len, MIDI_ProgramChange msg);
int MIDI_pitch_bend_msg_to_str_buffer_short(char * str, int max_len, MIDI_PitchBend msg);
int MIDI_aftertouch_mono_msg_to_str_buffer_short(char * str, int max_len, MIDI_AftertouchMono msg);
int MIDI_aftertouch_poly_msg_to_str_buffer_short(char * str, int max_len, MIDI_AftertouchPoly msg);
int MIDI_system_msg_to_str_buffer_short(char * str, int max_len, MIDI_SystemMessage msg);

int MIDI_message_to_str_buffer(char * str, int max_len, MIDI_Message msg);
int MIDI_message_to_str_buffer_short(char * str, int max_len, MIDI_Message msg);

bool MIDI_message_equals(const MIDI_Message * lhs, const MIDI_Message * rhs);

bool MIDI_note_on_msg_equals(const MIDI_NoteOn * lhs, const MIDI_NoteOn * rhs);
bool MIDI_note_off_msg_equals(const MIDI_NoteOff * lhs, const MIDI_NoteOff * rhs);
bool MIDI_control_change_msg_equals(const MIDI_ControlChange * lhs, const MIDI_ControlChange * rhs);
bool MIDI_program_change_msg_equals(const MIDI_ProgramChange * lhs, const MIDI_ProgramChange * rhs);
bool MIDI_pitch_bend_msg_equals(const MIDI_PitchBend * lhs, const MIDI_PitchBend * rhs);
bool MIDI_aftertouch_mono_msg_equals(const MIDI_AftertouchMono * lhs, const MIDI_AftertouchMono * rhs);
bool MIDI_aftertouch_poly_msg_equals(const MIDI_AftertouchPoly * lhs, const MIDI_AftertouchPoly * rhs);

static inline const char * MIDI_message_type_to_str_lower_case(MIDI_MessageType t) {
  switch(t) {
  case MIDI_MSG_TYPE_NOTE_OFF: return "note_off";
  case MIDI_MSG_TYPE_NOTE_ON: return "note_on";
  case MIDI_MSG_TYPE_AFTERTOUCH_POLY: return "aftertouch_poly";
  case MIDI_MSG_TYPE_CONTROL_CHANGE: return "control_change";
  case MIDI_MSG_TYPE_PROGRAM_CHANGE: return "program_change";
  case MIDI_MSG_TYPE_AFTERTOUCH_MONO: return "aftertouch_mono";
  case MIDI_MSG_TYPE_PITCH_BEND: return "pitch_bend";
  case MIDI_MSG_TYPE_SYSTEM: return "system_msg";
  }
  return "unknown";
}

static inline const char * MIDI_message_type_to_str(MIDI_MessageType t) {
  switch(t) {
  case MIDI_MSG_TYPE_NOTE_OFF: return "NOTE_OFF";
  case MIDI_MSG_TYPE_NOTE_ON: return "NOTE_ON";
  case MIDI_MSG_TYPE_AFTERTOUCH_POLY: return "AFTERTOUCH_POLY";
  case MIDI_MSG_TYPE_CONTROL_CHANGE: return "CONTROL_CHANGE";
  case MIDI_MSG_TYPE_PROGRAM_CHANGE: return "PROGRAM_CHANGE";
  case MIDI_MSG_TYPE_AFTERTOUCH_MONO: return "AFTERTOUCH_MONO";
  case MIDI_MSG_TYPE_PITCH_BEND: return "PITCH_BEND";
  case MIDI_MSG_TYPE_SYSTEM: return "SYSTEM";
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

static inline const char * MIDI_system_message_type_to_str(MIDI_SystemMessageType t) {
  switch(t) {
  case MIDI_MSG_TYPE_SYSEX: return "SYSEX";
  case MIDI_MSG_TYPE_MTC_QUARTER_FRAME: return "MTC_QUARTER_FRAME";
  case MIDI_MSG_TYPE_SONG_POSITION_POINTER: return "SONG_POSITION_POINTER";
  case MIDI_MSG_TYPE_SONG_SELECT: return "SONG_SELECT";
  case MIDI_MSG_TYPE_TUNE_REQUEST: return "TUNE_REQUEST";
  case MIDI_MSG_TYPE_END_OF_SYSEX: return "END_OF_SYSEX";
  case MIDI_MSG_TYPE_TIMING_CLOCK: return "TIMING_CLOCK";
  case MIDI_MSG_TYPE_START: return "START";
  case MIDI_MSG_TYPE_CONTINUE: return "CONTINUE";
  case MIDI_MSG_TYPE_STOP: return "STOP";
  case MIDI_MSG_TYPE_ACTIVE_SENSING: return "ACTIVE_SENSING";
  case MIDI_MSG_TYPE_SYSTEM_RESET: return "SYSTEM_RESET";
  }
  return "UNKNOWN";
}

static inline bool MIDI_is_real_time_type(MIDI_SystemMessageType type) {
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

#endif