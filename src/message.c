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
  if(len < max_len) len += snprintf(str, max_len, "NoteOff{note=");

  if(len < max_len) len += MIDI_note_to_str_buffer(&str[len], (max_len - len), msg.note);

  if(len < max_len) len += snprintf(&str[len], (max_len - len), ", velocity=%u}", msg.velocity);

  return len;
}

int MIDI_note_on_msg_to_str_buffer(char * str, int max_len, MIDI_NoteOn msg) {
  if(str == NULL) return 0;

  int len = 0;
  if(len < max_len) len += snprintf(str, max_len, "NoteOn{note=");

  if(len < max_len) len += MIDI_note_to_str_buffer(&str[len], (max_len - len), msg.note);

  if(len < max_len) len += snprintf(&str[len], (max_len - len), ", velocity=%u}", msg.velocity);

  return len;
}

int MIDI_control_change_msg_to_str_buffer(char * str, int max_len, MIDI_ControlChange msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "ControlChange{control=%s, value=%u}", MIDI_ctrl_to_str(msg.control), msg.value);
}

int MIDI_program_change_msg_to_str_buffer(char * str, int max_len, MIDI_ProgramChange msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "ProgramChange{program_id=%u}", msg.program_id);
}

int MIDI_pitch_bend_msg_to_str_buffer(char * str, int max_len, MIDI_PitchBend msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "PitchBend{value=%d}", msg.value);
}

int MIDI_aftertouch_mono_msg_to_str_buffer(char * str, int max_len, MIDI_AftertouchMono msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "AftertouchMono{value=%u}", msg.value);
}

int MIDI_aftertouch_poly_msg_to_str_buffer(char * str, int max_len, MIDI_AftertouchPoly msg) {
  if(str == NULL) return 0;

  int len = 0;
  if(len < max_len) len += snprintf(str, max_len, "AftertouchPoly{note=");

  if(len < max_len) len += MIDI_note_to_str_buffer(&str[len], (max_len - len), msg.note);

  if(len < max_len) len += snprintf(&str[len], (max_len - len), ", value=%u}", msg.value);

  return len;
}

int MIDI_quarter_frame_msg_to_str_buffer(char * str, int max_len, MIDI_QuarterFrame msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "QuarterFrame{type=%s, value=%u}", MIDI_quarter_frame_type_to_str(msg.type), msg.value);
}

int MIDI_song_position_pointer_msg_to_str_buffer(char * str, int max_len, MIDI_SongPositionPointer msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "SongPositionPointer{value=%u}", msg.value);
}

int MIDI_song_select_msg_to_str_buffer(char * str, int max_len, MIDI_SongSelect msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "SongSelect{value=%u}", msg.value);
}

int MIDI_sysex_byte_msg_to_str_buffer(char * str, int max_len, MIDI_SysexByte msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "SysexByte{sequence_number=%u, byte=0x%x}", msg.sequence_number, msg.byte);
}

int MIDI_sysex_stop_msg_to_str_buffer(char * str, int max_len, MIDI_SysexStop msg) {
  if(str == NULL) return 0;

  return snprintf(str,
                  max_len,
                  "SysexStop{sequence_length=%u, is_length_overflowed=%s}",
                  msg.sequence_length,
                  msg.is_length_overflowed ? "true" : "false");
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

int MIDI_quarter_frame_msg_to_str_buffer_short(char * str, int max_len, MIDI_QuarterFrame msg) {
  if(str == NULL) return 0;

  int len = 0;
  if(len < max_len) len += snprintf(str, max_len, "QF{");

  if(len < max_len) len += snprintf(&str[len], (max_len - len), "%s,", MIDI_quarter_frame_type_to_str_short(msg.type));

  if(len < max_len) len += snprintf(&str[len], (max_len - len), "%u}", msg.value);

  return len;
}

int MIDI_song_position_pointer_msg_to_str_buffer_short(char * str, int max_len, MIDI_SongPositionPointer msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "SPP{%u}", msg.value);
}

int MIDI_song_select_msg_to_str_buffer_short(char * str, int max_len, MIDI_SongSelect msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "SS{%u}", msg.value);
}

int MIDI_sysex_byte_msg_to_str_buffer_short(char * str, int max_len, MIDI_SysexByte msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "SSX{%u:0x%x}", msg.sequence_number, msg.byte);
}

int MIDI_sysex_stop_msg_to_str_buffer_short(char * str, int max_len, MIDI_SysexStop msg) {
  if(str == NULL) return 0;

  return snprintf(str, max_len, "SSX_STOP{%u%s}", msg.sequence_length, msg.is_length_overflowed ? "+" : "");
}

int MIDI_message_to_str_buffer(char * str, int max_len, MIDI_Message msg) {
  if(str == NULL) return 0;

  int len = 0;

  if(len < max_len) len += snprintf(&str[len], max_len, "MIDI_Message{type=%s, ", MIDI_message_type_to_str(msg.type));

  if((len < max_len) && MIDI_is_channel_type(msg.type)) {
    len += snprintf(&str[len], max_len, "channel=%u, ", msg.channel);
  }

  if(len < max_len) len += snprintf(&str[len], max_len, "data=");

  if(len < max_len) {
    switch(msg.type) {
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
    case MIDI_MSG_TYPE_MTC_QUARTER_FRAME:
      len += MIDI_quarter_frame_msg_to_str_buffer(&str[len], (max_len - len), msg.data.quarter_frame);
      break;
    case MIDI_MSG_TYPE_SONG_POSITION_POINTER:
      len += MIDI_song_position_pointer_msg_to_str_buffer(&str[len], (max_len - len), msg.data.song_position_pointer);
      break;
    case MIDI_MSG_TYPE_SONG_SELECT:
      len += MIDI_song_select_msg_to_str_buffer(&str[len], (max_len - len), msg.data.song_select);
      break;
    case MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE:
      len += MIDI_sysex_byte_msg_to_str_buffer(&str[len], (max_len - len), msg.data.sysex_byte);
      break;
    case MIDI_MSG_TYPE_SYSEX_STOP:
      len += MIDI_sysex_stop_msg_to_str_buffer(&str[len], (max_len - len), msg.data.sysex_stop);
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

  if((len < max_len) && MIDI_is_channel_type(msg.type)) {
    len += snprintf(&str[len], (max_len - len), "%u:", msg.channel);
  }

  if(len < max_len) {
    switch(msg.type) {
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
    case MIDI_MSG_TYPE_MTC_QUARTER_FRAME:
      len += MIDI_quarter_frame_msg_to_str_buffer_short(&str[len], (max_len - len), msg.data.quarter_frame);
      break;
    case MIDI_MSG_TYPE_SONG_POSITION_POINTER:
      len += MIDI_song_position_pointer_msg_to_str_buffer_short(&str[len],
                                                                (max_len - len),
                                                                msg.data.song_position_pointer);
      break;
    case MIDI_MSG_TYPE_SONG_SELECT:
      len += MIDI_song_select_msg_to_str_buffer_short(&str[len], (max_len - len), msg.data.song_select);
      break;
    case MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE:
      len += MIDI_sysex_byte_msg_to_str_buffer_short(&str[len], (max_len - len), msg.data.sysex_byte);
      break;
    case MIDI_MSG_TYPE_SYSEX_STOP:
      len += MIDI_sysex_stop_msg_to_str_buffer_short(&str[len], (max_len - len), msg.data.sysex_stop);
      break;
    case MIDI_MSG_TYPE_TIMING_CLOCK: len += snprintf(&str[len], (max_len - len), "TCLK"); break;
    case MIDI_MSG_TYPE_START: len += snprintf(&str[len], (max_len - len), "START"); break;
    case MIDI_MSG_TYPE_CONTINUE: len += snprintf(&str[len], (max_len - len), "CONT"); break;
    case MIDI_MSG_TYPE_STOP: len += snprintf(&str[len], (max_len - len), "STOP"); break;
    case MIDI_MSG_TYPE_ACTIVE_SENSING: len += snprintf(&str[len], (max_len - len), "ASENS"); break;
    case MIDI_MSG_TYPE_SYSTEM_RESET: len += snprintf(&str[len], (max_len - len), "RESET"); break;
    case MIDI_MSG_TYPE_SYSEX_START: len += snprintf(&str[len], (max_len - len), "SSX_START"); break;
    default: len += snprintf(&str[len], (max_len - len), "??"); break;
    }
  }

  return len;
}

bool MIDI_message_equals(MIDI_Message lhs, MIDI_Message rhs) {
  if(lhs.type != rhs.type) return false;

  if(MIDI_is_single_byte_type(lhs.type)) return true;

  if(MIDI_is_channel_type(lhs.type) && (lhs.channel != rhs.channel)) return false;

  switch(lhs.type) {
  case MIDI_MSG_TYPE_NOTE_OFF: return MIDI_note_off_msg_equals(lhs.data.note_off, rhs.data.note_off);
  case MIDI_MSG_TYPE_NOTE_ON: return MIDI_note_on_msg_equals(lhs.data.note_on, rhs.data.note_on);
  case MIDI_MSG_TYPE_AFTERTOUCH_POLY:
    return MIDI_aftertouch_poly_msg_equals(lhs.data.aftertouch_poly, rhs.data.aftertouch_poly);
  case MIDI_MSG_TYPE_CONTROL_CHANGE:
    return MIDI_control_change_msg_equals(lhs.data.control_change, rhs.data.control_change);
  case MIDI_MSG_TYPE_PROGRAM_CHANGE:
    return MIDI_program_change_msg_equals(lhs.data.program_change, rhs.data.program_change);
  case MIDI_MSG_TYPE_AFTERTOUCH_MONO:
    return MIDI_aftertouch_mono_msg_equals(lhs.data.aftertouch_mono, rhs.data.aftertouch_mono);
  case MIDI_MSG_TYPE_PITCH_BEND: return MIDI_pitch_bend_msg_equals(lhs.data.pitch_bend, rhs.data.pitch_bend);
  case MIDI_MSG_TYPE_MTC_QUARTER_FRAME:
    return MIDI_quarter_frame_msg_equals(lhs.data.quarter_frame, rhs.data.quarter_frame);
  case MIDI_MSG_TYPE_SONG_POSITION_POINTER:
    return MIDI_song_position_pointer_msg_equals(lhs.data.song_position_pointer, rhs.data.song_position_pointer);
  case MIDI_MSG_TYPE_SONG_SELECT: return MIDI_song_select_msg_equals(lhs.data.song_select, rhs.data.song_select);
  case MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE: return MIDI_sysex_byte_msg_equals(lhs.data.sysex_byte, rhs.data.sysex_byte);
  case MIDI_MSG_TYPE_SYSEX_STOP: return MIDI_sysex_stop_msg_equals(lhs.data.sysex_stop, rhs.data.sysex_stop);
  case MIDI_MSG_TYPE_SYSEX_START: return true; // special single-byte type, not included with the rest
  default: return false;
  }

  return false;
}

bool MIDI_note_on_msg_equals(MIDI_NoteOn lhs, MIDI_NoteOn rhs) {
  return (lhs.note == rhs.note) && (lhs.velocity == rhs.velocity);
}

bool MIDI_note_off_msg_equals(MIDI_NoteOff lhs, MIDI_NoteOff rhs) {
  return (lhs.note == rhs.note) && (lhs.velocity == rhs.velocity);
}

bool MIDI_control_change_msg_equals(MIDI_ControlChange lhs, MIDI_ControlChange rhs) {
  return (lhs.control == rhs.control) && (lhs.value == rhs.value);
}

bool MIDI_program_change_msg_equals(MIDI_ProgramChange lhs, MIDI_ProgramChange rhs) {
  return (lhs.program_id == rhs.program_id);
}

bool MIDI_pitch_bend_msg_equals(MIDI_PitchBend lhs, MIDI_PitchBend rhs) { return (lhs.value == rhs.value); }

bool MIDI_aftertouch_mono_msg_equals(MIDI_AftertouchMono lhs, MIDI_AftertouchMono rhs) {
  return (lhs.value == rhs.value);
}

bool MIDI_aftertouch_poly_msg_equals(MIDI_AftertouchPoly lhs, MIDI_AftertouchPoly rhs) {
  return (lhs.note == rhs.note) && (lhs.value == rhs.value);
}

bool MIDI_quarter_frame_msg_equals(MIDI_QuarterFrame lhs, MIDI_QuarterFrame rhs) {
  return (lhs.type == rhs.type) && (lhs.value == rhs.value);
}

bool MIDI_song_position_pointer_msg_equals(MIDI_SongPositionPointer lhs, MIDI_SongPositionPointer rhs) {
  return (lhs.value == rhs.value);
}

bool MIDI_song_select_msg_equals(MIDI_SongSelect lhs, MIDI_SongSelect rhs) { return (lhs.value == rhs.value); }

bool MIDI_sysex_byte_msg_equals(MIDI_SysexByte lhs, MIDI_SysexByte rhs) {
  return (lhs.byte == rhs.byte) && (lhs.sequence_number == rhs.sequence_number);
}

bool MIDI_sysex_stop_msg_equals(MIDI_SysexStop lhs, MIDI_SysexStop rhs) {
  return (lhs.sequence_length == rhs.sequence_length) && (lhs.is_length_overflowed == rhs.is_length_overflowed);
}