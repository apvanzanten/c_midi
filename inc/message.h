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
  MIDI_MSG_TYPE_MISC,
} MIDI_MessageType;

static inline uint8_t MIDI_type_to_byte(MIDI_MessageType type) { return (uint8_t)type; }

typedef struct MIDI_NoteOff {
  uint8_t note; // MIDI_Note
  uint8_t velocity;
} MIDI_NoteOff;

typedef struct MIDI_NoteOn {
  uint8_t note; // MIDI_Note
  uint8_t velocity;
} MIDI_NoteOn;

typedef struct MIDI_ControlChange {
  uint8_t control; // MIDI_ControlType
  uint8_t value;
} MIDI_ControlChange;

typedef struct MIDI_PitchBend {
  int16_t value;
} MIDI_PitchBend;

typedef struct MIDI_Message {
  uint8_t type; // MIDI_MessageType
  union {
    MIDI_NoteOff       note_off;
    MIDI_NoteOn        note_on;
    MIDI_ControlChange control_change;
    MIDI_PitchBend     pitch_bend;
  } data;
} MIDI_Message;

static inline const char * MIDI_message_type_to_str(MIDI_MessageType t) {
  switch(t) {
  case MIDI_MSG_TYPE_NOTE_OFF: return "NOTE_OFF";
  case MIDI_MSG_TYPE_NOTE_ON: return "NOTE_ON";
  case MIDI_MSG_TYPE_AFTERTOUCH_POLY: return "AFTERTOUCH_POLY";
  case MIDI_MSG_TYPE_CONTROL_CHANGE: return "CONTROL_CHANGE";
  case MIDI_MSG_TYPE_PROGRAM_CHANGE: return "PROGRAM_CHANGE";
  case MIDI_MSG_TYPE_AFTERTOUCH_MONO: return "AFTERTOUCH_MONO";
  case MIDI_MSG_TYPE_PITCH_BEND: return "PITCH_BEND";
  case MIDI_MSG_TYPE_MISC: return "MISC";
  }
  return "UNKNOWN";
}

int MIDI_note_off_msg_to_str_buffer(char * str, int max_len, MIDI_NoteOff msg);
int MIDI_note_on_msg_to_str_buffer(char * str, int max_len, MIDI_NoteOn msg);
int MIDI_control_change_msg_to_str_buffer(char * str, int max_len, MIDI_ControlChange msg);
int MIDI_pitch_bend_msg_to_str_buffer(char * str, int max_len, MIDI_PitchBend msg);

int MIDI_note_off_msg_to_str_buffer_short(char * str, int max_len, MIDI_NoteOff msg);
int MIDI_note_on_msg_to_str_buffer_short(char * str, int max_len, MIDI_NoteOn msg);
int MIDI_control_change_msg_to_str_buffer_short(char * str, int max_len, MIDI_ControlChange msg);
int MIDI_pitch_bend_msg_to_str_buffer_short(char * str, int max_len, MIDI_PitchBend msg);

int MIDI_message_to_str_buffer(char * str, int max_len, MIDI_Message msg);
int MIDI_message_to_str_buffer_short(char * str, int max_len, MIDI_Message msg);

#endif