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
  if(len < max_len) len += snprintf(str, max_len, "MIDI_NoteOff{.note=");

  if(len < max_len) len += MIDI_note_to_str_buffer(&str[len], (max_len - len), msg.note);

  if(len < max_len) len += snprintf(&str[len], (max_len - len), ", .velocity=%u}", msg.velocity);

  return len;
}

int MIDI_note_on_msg_to_str_buffer(char * str, int max_len, MIDI_NoteOn msg) {
  if(str == NULL) return 0;

  int len = 0;
  if(len < max_len) len += snprintf(str, max_len, "MIDI_NoteOn{.note=");

  if(len < max_len) len += MIDI_note_to_str_buffer(&str[len], (max_len - len), msg.note);

  if(len < max_len) len += snprintf(&str[len], (max_len - len), ", .velocity=%u}", msg.velocity);

  return len;
}

int MIDI_control_change_msg_to_str_buffer(char * str, int max_len, MIDI_ControlChange msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "MIDI_ControlChange{.control=%s, .value=%u}", MIDI_ctrl_to_str(msg.control), msg.value);
}

int MIDI_pitch_bend_msg_to_str_buffer(char * str, int max_len, MIDI_PitchBend msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "MIDI_PitchBend{.value=%d}", msg.value);
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

int MIDI_pitch_bend_msg_to_str_buffer_short(char * str, int max_len, MIDI_PitchBend msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "PB{%d}", msg.value);
}

int MIDI_message_to_str_buffer(char * str, int max_len, MIDI_Message msg) {
  if(str == NULL) return 0;

  int len = 0;

  if(len < max_len) len += snprintf(str, max_len, "MIDI_Message{.type=%s, .data=", MIDI_message_type_to_str(msg.type));

  if(len < max_len) {
    switch(msg.type) {
    case MIDI_MSG_TYPE_NOTE_OFF:
      len += MIDI_note_off_msg_to_str_buffer(&str[len], (max_len - len), msg.as.channel_msg.data.note_off);
      break;
    case MIDI_MSG_TYPE_NOTE_ON:
      len += MIDI_note_on_msg_to_str_buffer(&str[len], (max_len - len), msg.as.channel_msg.data.note_on);
      break;
    case MIDI_MSG_TYPE_AFTERTOUCH_POLY: len += snprintf(&str[len], (max_len - len), "??"); break;
    case MIDI_MSG_TYPE_CONTROL_CHANGE:
      len += MIDI_control_change_msg_to_str_buffer(&str[len], (max_len - len), msg.as.channel_msg.data.control_change);
      break;
    case MIDI_MSG_TYPE_PROGRAM_CHANGE: len += snprintf(&str[len], (max_len - len), "??"); break;
    case MIDI_MSG_TYPE_AFTERTOUCH_MONO: len += snprintf(&str[len], (max_len - len), "??"); break;
    case MIDI_MSG_TYPE_PITCH_BEND:
      len += MIDI_pitch_bend_msg_to_str_buffer(&str[len], (max_len - len), msg.as.channel_msg.data.pitch_bend);
      break;
    case MIDI_MSG_TYPE_SYSTEM: len += snprintf(&str[len], (max_len - len), "SYS"); break;
    }
  }

  if(len < max_len) len += snprintf(&str[len], (max_len - len), "}");

  return len;
}

int MIDI_message_to_str_buffer_short(char * str, int max_len, MIDI_Message msg) {
  if(str == NULL) return 0;

  int len = 0;

  if(len < max_len) {
    switch(msg.type) {
    case MIDI_MSG_TYPE_NOTE_OFF:
      len += MIDI_note_off_msg_to_str_buffer_short(&str[len], (max_len - len), msg.as.channel_msg.data.note_off);
      break;
    case MIDI_MSG_TYPE_NOTE_ON:
      len += MIDI_note_on_msg_to_str_buffer_short(&str[len], (max_len - len), msg.as.channel_msg.data.note_on);
      break;
    case MIDI_MSG_TYPE_AFTERTOUCH_POLY: len += snprintf(&str[len], (max_len - len), "??"); break;
    case MIDI_MSG_TYPE_CONTROL_CHANGE:
      len += MIDI_control_change_msg_to_str_buffer_short(&str[len],
                                                         (max_len - len),
                                                         msg.as.channel_msg.data.control_change);
      break;
    case MIDI_MSG_TYPE_PROGRAM_CHANGE: len += snprintf(&str[len], (max_len - len), "??"); break;
    case MIDI_MSG_TYPE_AFTERTOUCH_MONO: len += snprintf(&str[len], (max_len - len), "??"); break;
    case MIDI_MSG_TYPE_PITCH_BEND:
      len += MIDI_pitch_bend_msg_to_str_buffer_short(&str[len], (max_len - len), msg.as.channel_msg.data.pitch_bend);
      break;
    case MIDI_MSG_TYPE_SYSTEM: len += snprintf(&str[len], (max_len - len), "SYS"); break;
    }
  }

  return len;
}

bool MIDI_message_equals(const MIDI_Message * lhs, const MIDI_Message * rhs) {
  if((lhs == NULL) || (rhs == NULL)) return false;
  if(lhs == rhs) return true;

  if(lhs->type != rhs->type) return false;

  if(MIDI_is_channel_type(lhs->type)) {
    const MIDI_ChannelMessage * lhs_c = &lhs->as.channel_msg;
    const MIDI_ChannelMessage * rhs_c = &rhs->as.channel_msg;

    if(lhs_c->channel != rhs_c->channel) return false;

    switch(lhs->type) {
    case MIDI_MSG_TYPE_NOTE_OFF: return MIDI_note_off_msg_equals(&lhs_c->data.note_off, &rhs_c->data.note_off);
    case MIDI_MSG_TYPE_NOTE_ON: return MIDI_note_on_msg_equals(&lhs_c->data.note_on, &rhs_c->data.note_on);
    case MIDI_MSG_TYPE_AFTERTOUCH_POLY:
      return MIDI_aftertouch_poly_msg_equals(&lhs_c->data.aftertouch_poly, &rhs_c->data.aftertouch_poly);
    case MIDI_MSG_TYPE_CONTROL_CHANGE:
      return MIDI_control_change_msg_equals(&lhs_c->data.control_change, &rhs_c->data.control_change);
    case MIDI_MSG_TYPE_PROGRAM_CHANGE:
      return MIDI_program_change_msg_equals(&lhs_c->data.program_change, &rhs_c->data.program_change);
    case MIDI_MSG_TYPE_AFTERTOUCH_MONO:
      return MIDI_aftertouch_mono_msg_equals(&lhs_c->data.aftertouch_mono, &rhs_c->data.aftertouch_mono);
    case MIDI_MSG_TYPE_PITCH_BEND: return MIDI_pitch_bend_msg_equals(&lhs_c->data.pitch_bend, &rhs_c->data.pitch_bend);
    default: return false;
    }

    return false;
  }

  if(MIDI_is_system_type(lhs->type)) {
    if(lhs->as.system_msg.type != rhs->as.system_msg.type) return false;
    if(MIDI_is_real_time_type(lhs->as.system_msg.type)) return true;

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