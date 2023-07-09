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

#endif