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

#include "message.h"

#include <stdio.h>

int MIDI_note_off_msg_to_str_buffer(char * str, int max_len, MIDI_NoteOff msg) {
  if(str == NULL) return 0;

  int len = 0;
  if(len < max_len) len += snprintf(str, max_len, "MIDI_NoteOff{note=");

  if(len < max_len) len += MIDI_note_to_str_buffer(&str[len], (max_len - len), msg.note);

  if(len < max_len) len += snprintf(&str[len], (max_len - len), ", velocity=%u}", msg.velocity);

  return len;
}

int MIDI_note_on_msg_to_str_buffer(char * str, int max_len, MIDI_NoteOn msg) {
  if(str == NULL) return 0;

  int len = 0;
  if(len < max_len) len += snprintf(str, max_len, "MIDI_NoteOn{note=");

  if(len < max_len) len += MIDI_note_to_str_buffer(&str[len], (max_len - len), msg.note);

  if(len < max_len) len += snprintf(&str[len], (max_len - len), ", velocity=%u}", msg.velocity);

  return len;
}

int MIDI_control_change_msg_to_str_buffer(char * str, int max_len, MIDI_ControlChange msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "MIDI_ControlChange{control=%s, value=%u}", MIDI_ctrl_to_str(msg.control), msg.value);
}

int MIDI_program_change_msg_to_str_buffer(char * str, int max_len, MIDI_ProgramChange msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "MIDI_ProgramChange{program_id=%u}", msg.program_id);
}

int MIDI_pitch_bend_msg_to_str_buffer(char * str, int max_len, MIDI_PitchBend msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "MIDI_PitchBend{value=%d}", msg.value);
}

int MIDI_aftertouch_mono_msg_to_str_buffer(char * str, int max_len, MIDI_AftertouchMono msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "MIDI_AftertouchMono{value=%u}", msg.value);
}

int MIDI_aftertouch_poly_msg_to_str_buffer(char * str, int max_len, MIDI_AftertouchPoly msg) {
  if(str == NULL) return 0;

  int len = 0;
  if(len < max_len) len += snprintf(str, max_len, "MIDI_AftertouchPoly{note=");

  if(len < max_len) len += MIDI_note_to_str_buffer(&str[len], (max_len - len), msg.note);

  if(len < max_len) len += snprintf(&str[len], (max_len - len), ", value=%u}", msg.value);

  return len;
}

int MIDI_note_off_msg_to_str_buffer_short(char * str, int max_len, MIDI_NoteOff msg) {
  if(str == NULL) return 0;

  int len = 0;
  if(len < max_len) len += snprintf(str, max_len, "OFF{");

  if(len < max_len) len += MIDI_note_to_str_buffer(&str[len], (max_len - len), msg.note);

  if(len < max_len) len += snprintf(&str[len], (max_len - len), ",%u}", msg.velocity);

  return len;
}

int MIDI_note_on_msg_to_str_buffer_short(char * str, int max_len, MIDI_NoteOn msg) {
  if(str == NULL) return 0;

  int len = 0;
  if(len < max_len) len += snprintf(str, max_len, "ON{");

  if(len < max_len) len += MIDI_note_to_str_buffer(&str[len], (max_len - len), msg.note);

  if(len < max_len) len += snprintf(&str[len], (max_len - len), ",%u}", msg.velocity);

  return len;
}

int MIDI_control_change_msg_to_str_buffer_short(char * str, int max_len, MIDI_ControlChange msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "CC{%s,%u}", MIDI_ctrl_to_str(msg.control), msg.value);
}

int MIDI_program_change_msg_to_str_buffer_short(char * str, int max_len, MIDI_ProgramChange msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "PC{%u}", msg.program_id);
}

int MIDI_pitch_bend_msg_to_str_buffer_short(char * str, int max_len, MIDI_PitchBend msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "PB{%d}", msg.value);
}

int MIDI_aftertouch_mono_msg_to_str_buffer_short(char * str, int max_len, MIDI_AftertouchMono msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "ATM{%u}", msg.value);
}

int MIDI_aftertouch_poly_msg_to_str_buffer_short(char * str, int max_len, MIDI_AftertouchPoly msg) {
  if(str == NULL) return 0;

  int len = 0;
  if(len < max_len) len += snprintf(str, max_len, "ATP{");

  if(len < max_len) len += MIDI_note_to_str_buffer(&str[len], (max_len - len), msg.note);

  if(len < max_len) len += snprintf(&str[len], (max_len - len), ",%u}", msg.value);

  return len;
}

int MIDI_message_to_str_buffer(char * str, int max_len, MIDI_Message msg) {
  if(str == NULL) return 0;

  int len = 0;

  if(len < max_len)
    len += snprintf(&str[len], max_len, "MIDI_Message{type=%s, ", MIDI_message_type_to_str(MIDI_get_type(msg)));

  if((len < max_len) && MIDI_is_channel_type(MIDI_get_type(msg))) {
    len += snprintf(&str[len], max_len, "channel=%u, ", MIDI_get_channel(msg));
  }

  if(len < max_len) len += snprintf(&str[len], max_len, "data=");

  if(len < max_len) {
    switch(MIDI_get_type(msg)) {
    case MIDI_MSG_TYPE_NOTE_OFF:
      len += MIDI_note_off_msg_to_str_buffer(&str[len], (max_len - len), msg.data.note_off);
      break;
    case MIDI_MSG_TYPE_NOTE_ON:
      len += MIDI_note_on_msg_to_str_buffer(&str[len], (max_len - len), msg.data.note_on);
      break;
    case MIDI_MSG_TYPE_AFTERTOUCH_POLY:
      len += MIDI_aftertouch_poly_msg_to_str_buffer(&str[len], (max_len - len), msg.data.aftertouch_poly);
      break;
    case MIDI_MSG_TYPE_CONTROL_CHANGE:
      len += MIDI_control_change_msg_to_str_buffer(&str[len], (max_len - len), msg.data.control_change);
      break;
    case MIDI_MSG_TYPE_PROGRAM_CHANGE:
      len += MIDI_program_change_msg_to_str_buffer(&str[len], (max_len - len), msg.data.program_change);
      break;
    case MIDI_MSG_TYPE_AFTERTOUCH_MONO:
      len += MIDI_aftertouch_mono_msg_to_str_buffer(&str[len], (max_len - len), msg.data.aftertouch_mono);
      break;
    case MIDI_MSG_TYPE_PITCH_BEND:
      len += MIDI_pitch_bend_msg_to_str_buffer(&str[len], (max_len - len), msg.data.pitch_bend);
      break;
    default: len += snprintf(&str[len], max_len, "N/A"); break;
    }
  }

  if(len < max_len) len += snprintf(&str[len], (max_len - len), "}");

  return len;
}

int MIDI_message_to_str_buffer_short(char * str, int max_len, MIDI_Message msg) {
  if(str == NULL) return 0;

  int len = 0;

  if((len < max_len) && MIDI_is_channel_type(MIDI_get_type(msg))) {
    len += snprintf(&str[len], (max_len - len), "%u:", MIDI_get_channel(msg));
  }

  if(len < max_len) {
    switch(MIDI_get_type(msg)) {
    case MIDI_MSG_TYPE_NOTE_OFF:
      len += MIDI_note_off_msg_to_str_buffer_short(&str[len], (max_len - len), msg.data.note_off);
      break;
    case MIDI_MSG_TYPE_NOTE_ON:
      len += MIDI_note_on_msg_to_str_buffer_short(&str[len], (max_len - len), msg.data.note_on);
      break;
    case MIDI_MSG_TYPE_AFTERTOUCH_POLY:
      len += MIDI_aftertouch_poly_msg_to_str_buffer_short(&str[len], (max_len - len), msg.data.aftertouch_poly);
      break;
    case MIDI_MSG_TYPE_CONTROL_CHANGE:
      len += MIDI_control_change_msg_to_str_buffer_short(&str[len], (max_len - len), msg.data.control_change);
      break;
    case MIDI_MSG_TYPE_PROGRAM_CHANGE:
      len += MIDI_program_change_msg_to_str_buffer_short(&str[len], (max_len - len), msg.data.program_change);
      break;
    case MIDI_MSG_TYPE_AFTERTOUCH_MONO:
      len += MIDI_aftertouch_mono_msg_to_str_buffer_short(&str[len], (max_len - len), msg.data.aftertouch_mono);
      break;
    case MIDI_MSG_TYPE_PITCH_BEND:
      len += MIDI_pitch_bend_msg_to_str_buffer_short(&str[len], (max_len - len), msg.data.pitch_bend);
      break;
    case MIDI_MSG_TYPE_TIMING_CLOCK: len += snprintf(&str[len], (max_len - len), "TCLK"); break;
    case MIDI_MSG_TYPE_START: len += snprintf(&str[len], (max_len - len), "START"); break;
    case MIDI_MSG_TYPE_CONTINUE: len += snprintf(&str[len], (max_len - len), "CONT"); break;
    case MIDI_MSG_TYPE_STOP: len += snprintf(&str[len], (max_len - len), "STOP"); break;
    case MIDI_MSG_TYPE_ACTIVE_SENSING: len += snprintf(&str[len], (max_len - len), "ASENS"); break;
    case MIDI_MSG_TYPE_SYSTEM_RESET: len += snprintf(&str[len], (max_len - len), "RESET"); break;
    default: len += snprintf(&str[len], (max_len - len), "??"); break;
    }
  }

  return len;
}

bool MIDI_message_equals(const MIDI_Message * lhs, const MIDI_Message * rhs) {
  // TODO pass by value
  if((lhs == NULL) || (rhs == NULL)) return false;
  if(lhs == rhs) return true;

  if(lhs->status_data != rhs->status_data) return false;

  if(MIDI_is_real_time_type(MIDI_get_type(*lhs))) return true;

  if(MIDI_is_channel_type(MIDI_get_type(*lhs))) {
    switch(MIDI_get_type(*lhs)) {
    case MIDI_MSG_TYPE_NOTE_OFF: return MIDI_note_off_msg_equals(&lhs->data.note_off, &rhs->data.note_off);
    case MIDI_MSG_TYPE_NOTE_ON: return MIDI_note_on_msg_equals(&lhs->data.note_on, &rhs->data.note_on);
    case MIDI_MSG_TYPE_AFTERTOUCH_POLY:
      return MIDI_aftertouch_poly_msg_equals(&lhs->data.aftertouch_poly, &rhs->data.aftertouch_poly);
    case MIDI_MSG_TYPE_CONTROL_CHANGE:
      return MIDI_control_change_msg_equals(&lhs->data.control_change, &rhs->data.control_change);
    case MIDI_MSG_TYPE_PROGRAM_CHANGE:
      return MIDI_program_change_msg_equals(&lhs->data.program_change, &rhs->data.program_change);
    case MIDI_MSG_TYPE_AFTERTOUCH_MONO:
      return MIDI_aftertouch_mono_msg_equals(&lhs->data.aftertouch_mono, &rhs->data.aftertouch_mono);
    case MIDI_MSG_TYPE_PITCH_BEND: return MIDI_pitch_bend_msg_equals(&lhs->data.pitch_bend, &rhs->data.pitch_bend);
    default: return false;
    }

    return false;
  }

  if(MIDI_is_system_type(MIDI_get_type(*lhs))) {
    // TODO

    return false;
  }

  return false;
}

bool MIDI_note_on_msg_equals(const MIDI_NoteOn * lhs, const MIDI_NoteOn * rhs) {
  if((lhs == NULL) && (rhs == NULL)) return false;
  if(lhs == rhs) return true;

  return (lhs->note == rhs->note) && (lhs->velocity == rhs->velocity);
}

bool MIDI_note_off_msg_equals(const MIDI_NoteOff * lhs, const MIDI_NoteOff * rhs) {
  if((lhs == NULL) && (rhs == NULL)) return false;
  if(lhs == rhs) return true;

  return (lhs->note == rhs->note) && (lhs->velocity == rhs->velocity);
}

bool MIDI_control_change_msg_equals(const MIDI_ControlChange * lhs, const MIDI_ControlChange * rhs) {
  if((lhs == NULL) && (rhs == NULL)) return false;
  if(lhs == rhs) return true;

  return (lhs->control == rhs->control) && (lhs->value == rhs->value);
}

bool MIDI_program_change_msg_equals(const MIDI_ProgramChange * lhs, const MIDI_ProgramChange * rhs) {
  if((lhs == NULL) && (rhs == NULL)) return false;
  if(lhs == rhs) return true;

  return (lhs->program_id == rhs->program_id);
}

bool MIDI_pitch_bend_msg_equals(const MIDI_PitchBend * lhs, const MIDI_PitchBend * rhs) {
  if((lhs == NULL) && (rhs == NULL)) return false;
  if(lhs == rhs) return true;

  return (lhs->value == rhs->value);
}

bool MIDI_aftertouch_mono_msg_equals(const MIDI_AftertouchMono * lhs, const MIDI_AftertouchMono * rhs) {
  if((lhs == NULL) && (rhs == NULL)) return false;
  if(lhs == rhs) return true;

  return (lhs->value == rhs->value);
}

bool MIDI_aftertouch_poly_msg_equals(const MIDI_AftertouchPoly * lhs, const MIDI_AftertouchPoly * rhs) {
  if((lhs == NULL) && (rhs == NULL)) return false;
  if(lhs == rhs) return true;

  return (lhs->note == rhs->note) && (lhs->value == rhs->value);
}