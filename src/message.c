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

int MIDI_message_to_str_buffer(char * str, int max_len, MIDI_Message msg) {
  if(str == NULL) return 0;

  int len = 0;

  if(len < max_len) len += snprintf(str, max_len, "MIDI_Message{.type=%s, .data=", MIDI_message_type_to_str(msg.type));

  if(len < max_len) {
    switch(msg.type) {
    case MIDI_MSG_TYPE_NOTE_OFF:
      len += MIDI_note_off_msg_to_str_buffer(&str[len], (max_len - len), msg.data.note_off);
      break;
    case MIDI_MSG_TYPE_NOTE_ON:
      len += MIDI_note_on_msg_to_str_buffer(&str[len], (max_len - len), msg.data.note_on);
      break;
    case MIDI_MSG_TYPE_AFTERTOUCH_POLY: len += snprintf(&str[len], (max_len - len), "??"); break;
    case MIDI_MSG_TYPE_CONTROL_CHANGE:
      len += MIDI_control_change_msg_to_str_buffer(&str[len], (max_len - len), msg.data.control_change);
      break;
    case MIDI_MSG_TYPE_PROGRAM_CHANGE: len += snprintf(&str[len], (max_len - len), "??"); break;
    case MIDI_MSG_TYPE_AFTERTOUCH_MONO: len += snprintf(&str[len], (max_len - len), "??"); break;
    case MIDI_MSG_TYPE_PITCH_BEND:
      len += MIDI_pitch_bend_msg_to_str_buffer(&str[len], (max_len - len), msg.data.pitch_bend);
      break;
    case MIDI_MSG_TYPE_MISC: len += snprintf(&str[len], (max_len - len), "??"); break;
    }
  }

  if(len < max_len) len += snprintf(&str[len], (max_len - len), "}");

  return len;
}