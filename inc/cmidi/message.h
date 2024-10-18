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

  // channel types, all with zero nibble to make space for channel
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

typedef struct MIDI_Message {
  bool    reserved : 1; // reserved for hacking our way into supporting sysex
  uint8_t status_data : 7;
  union {
    MIDI_NoteOff        note_off;
    MIDI_NoteOn         note_on;
    MIDI_ControlChange  control_change;
    MIDI_ProgramChange  program_change;
    MIDI_PitchBend      pitch_bend;
    MIDI_AftertouchMono aftertouch_mono;
    MIDI_AftertouchPoly aftertouch_poly;
  } data;
} MIDI_Message;

// NOTE excludes status bit
static inline uint8_t MIDI_make_channel_status_data(MIDI_MessageType type, MIDI_Channel channel);
static inline uint8_t MIDI_make_system_status_data(MIDI_MessageType type);

static inline MIDI_MessageType MIDI_get_type(MIDI_Message msg);
static inline MIDI_MessageType MIDI_get_type_from_status_data(uint8_t status_data);
static inline MIDI_Channel     MIDI_get_channel(MIDI_Message msg);
static inline MIDI_Channel     MIDI_get_channel_from_status_data(uint8_t status_data);

static inline MIDI_Message MIDI_make_note_on_msg(MIDI_Channel channel, MIDI_NoteOn data);
static inline MIDI_Message MIDI_make_note_off_msg(MIDI_Channel channel, MIDI_NoteOff data);
static inline MIDI_Message MIDI_make_control_change_msg(MIDI_Channel channel, MIDI_ControlChange data);
static inline MIDI_Message MIDI_make_program_change_msg(MIDI_Channel channel, MIDI_ProgramChange data);
static inline MIDI_Message MIDI_make_pitch_bend_msg(MIDI_Channel channel, MIDI_PitchBend data);
static inline MIDI_Message MIDI_make_aftertouch_mono_msg(MIDI_Channel channel, MIDI_AftertouchMono data);
static inline MIDI_Message MIDI_make_aftertouch_poly_msg(MIDI_Channel channel, MIDI_AftertouchPoly data);

static inline MIDI_Message MIDI_make_real_time_msg(MIDI_MessageType type);

int MIDI_note_off_msg_to_str_buffer(char * str, int max_len, MIDI_NoteOff msg);
int MIDI_note_on_msg_to_str_buffer(char * str, int max_len, MIDI_NoteOn msg);
int MIDI_control_change_msg_to_str_buffer(char * str, int max_len, MIDI_ControlChange msg);
int MIDI_program_change_msg_to_str_buffer(char * str, int max_len, MIDI_ProgramChange msg);
int MIDI_pitch_bend_msg_to_str_buffer(char * str, int max_len, MIDI_PitchBend msg);
int MIDI_aftertouch_mono_msg_to_str_buffer(char * str, int max_len, MIDI_AftertouchMono msg);
int MIDI_aftertouch_poly_msg_to_str_buffer(char * str, int max_len, MIDI_AftertouchPoly msg);

int MIDI_note_off_msg_to_str_buffer_short(char * str, int max_len, MIDI_NoteOff msg);
int MIDI_note_on_msg_to_str_buffer_short(char * str, int max_len, MIDI_NoteOn msg);
int MIDI_control_change_msg_to_str_buffer_short(char * str, int max_len, MIDI_ControlChange msg);
int MIDI_program_change_msg_to_str_buffer_short(char * str, int max_len, MIDI_ProgramChange msg);
int MIDI_pitch_bend_msg_to_str_buffer_short(char * str, int max_len, MIDI_PitchBend msg);
int MIDI_aftertouch_mono_msg_to_str_buffer_short(char * str, int max_len, MIDI_AftertouchMono msg);
int MIDI_aftertouch_poly_msg_to_str_buffer_short(char * str, int max_len, MIDI_AftertouchPoly msg);

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
  case MIDI_MSG_TYPE_MTC_QUARTER_FRAME: return "mtc_quarter_frame";
  case MIDI_MSG_TYPE_SONG_POSITION_POINTER: return "song_position_pointer";
  case MIDI_MSG_TYPE_SONG_SELECT: return "song_select";
  case MIDI_MSG_TYPE_TUNE_REQUEST: return "tune_request";
  case MIDI_MSG_TYPE_SYSEX_START: return "sysex_start";
  case MIDI_MSG_TYPE_SYSEX_STOP: return "sysex_stop";
  case MIDI_MSG_TYPE_TIMING_CLOCK: return "timing_clock";
  case MIDI_MSG_TYPE_START: return "start";
  case MIDI_MSG_TYPE_CONTINUE: return "continue";
  case MIDI_MSG_TYPE_STOP: return "stop";
  case MIDI_MSG_TYPE_ACTIVE_SENSING: return "active_sensing";
  case MIDI_MSG_TYPE_SYSTEM_RESET: return "system_reset";
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

static inline uint8_t MIDI_make_channel_status_data(MIDI_MessageType type, MIDI_Channel channel) {
  // NOTE excludes status bit
  return ((type & 0x70) | ((channel - 1) & 0xf));
}

static inline uint8_t MIDI_make_system_status_data(MIDI_MessageType type) {
  // NOTE excludes status bit
  return (type & 0x7f);
}
static inline MIDI_Channel MIDI_get_channel_from_status_data(uint8_t status_data) { return (status_data & 0xf) + 1; }
static inline MIDI_Channel MIDI_get_channel(MIDI_Message msg) {
  return MIDI_get_channel_from_status_data(msg.status_data);
}

static inline MIDI_MessageType MIDI_get_type_from_status_data(uint8_t status_data) {
  const bool is_system_msg = ((status_data & 0x70) == 0x70);
  return (is_system_msg ? (status_data & 0x7f) : (status_data & 0x70));
}

static inline MIDI_MessageType MIDI_get_type(MIDI_Message msg) {
  return MIDI_get_type_from_status_data(msg.status_data);
}

static inline MIDI_Message MIDI_make_note_on_msg(MIDI_Channel channel, MIDI_NoteOn data) {
  return (MIDI_Message){.status_data  = MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_ON, channel),
                        .data.note_on = data};
}
static inline MIDI_Message MIDI_make_note_off_msg(MIDI_Channel channel, MIDI_NoteOff data) {
  return (MIDI_Message){.status_data   = MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_OFF, channel),
                        .data.note_off = data};
}
static inline MIDI_Message MIDI_make_control_change_msg(MIDI_Channel channel, MIDI_ControlChange data) {
  return (MIDI_Message){.status_data         = MIDI_make_channel_status_data(MIDI_MSG_TYPE_CONTROL_CHANGE, channel),
                        .data.control_change = data};
}
static inline MIDI_Message MIDI_make_program_change_msg(MIDI_Channel channel, MIDI_ProgramChange data) {
  return (MIDI_Message){.status_data         = MIDI_make_channel_status_data(MIDI_MSG_TYPE_PROGRAM_CHANGE, channel),
                        .data.program_change = data};
}
static inline MIDI_Message MIDI_make_pitch_bend_msg(MIDI_Channel channel, MIDI_PitchBend data) {
  return (MIDI_Message){.status_data     = MIDI_make_channel_status_data(MIDI_MSG_TYPE_PITCH_BEND, channel),
                        .data.pitch_bend = data};
}
static inline MIDI_Message MIDI_make_aftertouch_mono_msg(MIDI_Channel channel, MIDI_AftertouchMono data) {
  return (MIDI_Message){.status_data          = MIDI_make_channel_status_data(MIDI_MSG_TYPE_AFTERTOUCH_MONO, channel),
                        .data.aftertouch_mono = data};
}
static inline MIDI_Message MIDI_make_aftertouch_poly_msg(MIDI_Channel channel, MIDI_AftertouchPoly data) {
  return (MIDI_Message){.status_data          = MIDI_make_channel_status_data(MIDI_MSG_TYPE_AFTERTOUCH_POLY, channel),
                        .data.aftertouch_poly = data};
}

static inline MIDI_Message MIDI_make_real_time_msg(MIDI_MessageType type) {
  return (MIDI_Message){.status_data = MIDI_make_system_status_data(type)};
}

#endif
