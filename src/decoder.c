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

#include "decoder.h"

#include <cfac/log.h>

#define OK STAT_OK

#define LOGGING_ENABLED 0

#if LOGGING_ENABLED
#define LOG(decoder, byte, msg)                                                                                        \
  LOG_STAT(STAT_OK_INFO, "state=%s; byte=(%d == 0x%x); %s", state_to_string((decoder)->state), (byte), (byte), (msg));
#else
#define LOG(...)
#endif

typedef enum StateIdx {
  ST_INIT,
  ST_RUNNING_NOTE_ON,
  ST_NOTE_ON_WITH_VALID_NOTE,
  ST_RUNNING_NOTE_OFF,
  ST_NOTE_OFF_WITH_VALID_NOTE,
  ST_RUNNING_CONTROL_CHANGE,
  ST_CONTROL_CHANGE_WITH_VALID_CONTROL,
  ST_RUNNING_PROGRAM_CHANGE,
  ST_RUNNING_PITCH_BEND,
  ST_PITCH_BEND_LSB_RECEIVED,
  ST_RUNNING_AFTERTOUCH_MONO,
  ST_RUNNING_AFTERTOUCH_POLY,
  ST_AFTERTOUCH_POLY_WITH_VALID_NOTE,
  ST_MTC_QUARTER_FRAME_STARTED,
  ST_SONG_POSITION_POINTER_STARTED,
  ST_SONG_POSITION_POINTER_LSB_RECEIVED,
  ST_SONG_SELECT_STARTED,
  ST_IN_SYSEX_SEQUENCE,
} StateIdx;

const char * state_to_string(StateIdx state) {
  switch(state) {
  case ST_INIT: return "ST_INIT";
  case ST_RUNNING_NOTE_ON: return "ST_RUNNING_NOTE_ON";
  case ST_NOTE_ON_WITH_VALID_NOTE: return "ST_NOTE_ON_WITH_VALID_NOTE";
  case ST_RUNNING_NOTE_OFF: return "ST_RUNNING_NOTE_OFF";
  case ST_NOTE_OFF_WITH_VALID_NOTE: return "ST_NOTE_OFF_WITH_VALID_NOTE";
  case ST_RUNNING_CONTROL_CHANGE: return "ST_RUNNING_CONTROL_CHANGE";
  case ST_CONTROL_CHANGE_WITH_VALID_CONTROL: return "ST_CONTROL_CHANGE_WITH_VALID_CONTROL";
  case ST_RUNNING_PROGRAM_CHANGE: return "ST_RUNNING_PROGRAM_CHANGE";
  case ST_RUNNING_PITCH_BEND: return "ST_RUNNING_PITCH_BEND";
  case ST_PITCH_BEND_LSB_RECEIVED: return "ST_PITCH_BEND_LSB_RECEIVED";
  case ST_RUNNING_AFTERTOUCH_MONO: return "ST_RUNNING_AFTERTOUCH_MONO";
  case ST_RUNNING_AFTERTOUCH_POLY: return "ST_RUNNING_AFTERTOUCH_POLY";
  case ST_AFTERTOUCH_POLY_WITH_VALID_NOTE: return "ST_AFTERTOUCH_POLY_WITH_VALID_NOTE";
  case ST_MTC_QUARTER_FRAME_STARTED: return "ST_MTC_QUARTER_FRAME_STARTED";
  case ST_SONG_POSITION_POINTER_STARTED: return "ST_SONG_POSITION_POINTER_STARTED";
  case ST_SONG_POSITION_POINTER_LSB_RECEIVED: return "ST_SONG_POSITION_POINTER_LSB_RECEIVED";
  case ST_SONG_SELECT_STARTED: return "ST_SONG_SELECT_STARTED";
  case ST_IN_SYSEX_SEQUENCE: return "ST_IN_SYSEX_SEQUENCE";
  }
  return "UNKNOWN";
}

static uint8_t               get_status_bit(uint8_t byte);
static uint8_t               get_type_bits(uint8_t byte);
static MIDI_MessageType      get_type(uint8_t status_byte);
static uint8_t               get_channel(uint8_t status_byte);
static MIDI_QuarterFrameType get_quarter_frame_type(uint8_t byte);
static MIDI_QuarterFrameType get_quarter_frame_value(uint8_t byte);
static bool                  is_status(uint8_t byte);
static bool                  is_of_type(uint8_t byte, MIDI_MessageType type);
static bool                  is_channel_type(uint8_t status_byte);
static bool                  is_single_byte_type(uint8_t status_byte);
static bool                  is_real_time(uint8_t status_byte);
static bool                  is_data_byte(uint8_t byte);
static bool                  is_system_reset(uint8_t byte);

static int16_t  make_pitch_bend_value(uint8_t lsb, uint8_t high_byte);
static uint16_t make_song_position_pointer_value(uint8_t lsb, uint8_t high_byte);

static void buff_init(MIDI_MsgBuffer * restrict buffer) { *buffer = (MIDI_MsgBuffer){0}; }

STAT_Val MIDI_decoder_init(MIDI_Decoder * restrict decoder, MIDI_DecoderPriorityMode prio_mode) {
  if(decoder == NULL) return LOG_STAT(STAT_ERR_ARGS, "decoder pointer is NULL");

  *decoder = (MIDI_Decoder){0};
  buff_init(&(decoder->msg_buffer));
  buff_init(&(decoder->prio_msg_buffer));

  decoder->state     = ST_INIT;
  decoder->prio_mode = prio_mode;

  return OK;
}

STAT_Val MIDI_decoder_set_prio_mode(MIDI_Decoder * restrict decoder, MIDI_DecoderPriorityMode prio_mode) {
  if(decoder == NULL) return LOG_STAT(STAT_ERR_ARGS, "decoder pointer is NULL");

  decoder->prio_mode = prio_mode;

  return OK;
}

STAT_Val MIDI_decoder_push_byte(MIDI_Decoder * restrict decoder, uint8_t byte) {
  if(decoder == NULL) return LOG_STAT(STAT_ERR_ARGS, "decoder pointer is NULL");
  if(!MIDI_decoder_is_ready(decoder)) return LOG_STAT(STAT_ERR_PRECONDITION, "decoder not ready");

  LOG(decoder, byte, "func entry");

  if(is_real_time(byte)) {
    if(MIDI_decoder_is_in_realtime_prio_mode(decoder)) {
      MIDI_IMPL_decoder_buff_push(&(decoder->prio_msg_buffer), (MIDI_Message){.type = get_type(byte)});
    } else {
      MIDI_IMPL_decoder_buff_push(&(decoder->msg_buffer), (MIDI_Message){.type = get_type(byte)});
    }

    if(is_system_reset(byte)) decoder->state = ST_INIT;

    LOG(decoder, byte, "handled real time byte");
    return OK;
  }

  bool try_byte_again = false;

  do {
    switch(decoder->state) {
    case ST_INIT: {
      LOG(decoder, byte, "state entry");

      if(is_status(byte)) {
        if(is_single_byte_type(byte)) {
          // single-byte type messages are all type-only, so we can push immediately
          MIDI_IMPL_decoder_buff_push(&(decoder->msg_buffer), (MIDI_Message){.type = get_type(byte)});
        } else {
          if(is_channel_type(byte)) decoder->current_channel = get_channel(byte);

          switch(get_type(byte)) {
          case MIDI_MSG_TYPE_NOTE_OFF: decoder->state = ST_RUNNING_NOTE_OFF; break;
          case MIDI_MSG_TYPE_NOTE_ON: decoder->state = ST_RUNNING_NOTE_ON; break;
          case MIDI_MSG_TYPE_AFTERTOUCH_POLY: decoder->state = ST_RUNNING_AFTERTOUCH_POLY; break;
          case MIDI_MSG_TYPE_CONTROL_CHANGE: decoder->state = ST_RUNNING_CONTROL_CHANGE; break;
          case MIDI_MSG_TYPE_PROGRAM_CHANGE: decoder->state = ST_RUNNING_PROGRAM_CHANGE; break;
          case MIDI_MSG_TYPE_AFTERTOUCH_MONO: decoder->state = ST_RUNNING_AFTERTOUCH_MONO; break;
          case MIDI_MSG_TYPE_PITCH_BEND: decoder->state = ST_RUNNING_PITCH_BEND; break;
          case MIDI_MSG_TYPE_MTC_QUARTER_FRAME: decoder->state = ST_MTC_QUARTER_FRAME_STARTED; break;
          case MIDI_MSG_TYPE_SONG_POSITION_POINTER: decoder->state = ST_SONG_POSITION_POINTER_STARTED; break;
          case MIDI_MSG_TYPE_SONG_SELECT: decoder->state = ST_SONG_SELECT_STARTED; break;
          case MIDI_MSG_TYPE_SYSEX_START:
            MIDI_IMPL_decoder_buff_push(&(decoder->msg_buffer), (MIDI_Message){.type = get_type(byte)});
            decoder->sysex_sequence_length = 0;
            decoder->state                 = ST_IN_SYSEX_SEQUENCE;
            break;
          default:
            // type invalid, single-byte, or otherwise to-be-ignored, just stay in init
            decoder->state = ST_INIT;
            break;
          }
        }
      }

      try_byte_again = false; // we never try again after going through the init state, as there would be no improvement

      LOG(decoder, byte, "handled byte from ST_INIT");
      break;
    }

    // states specific to NOTE_ON
    case ST_RUNNING_NOTE_ON: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        decoder->current_note = MIDI_byte_to_note(byte);

        decoder->state = ST_NOTE_ON_WITH_VALID_NOTE;
      } else {
        // expected data byte, try again from init state
        try_byte_again = true;
        decoder->state = ST_INIT;
      }
      break;
    }
    case ST_NOTE_ON_WITH_VALID_NOTE: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        MIDI_IMPL_decoder_buff_push(&(decoder->msg_buffer),
                                    (MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_ON,
                                                   .channel      = decoder->current_channel,
                                                   .data.note_on = {.note = decoder->current_note, .velocity = byte}});

        decoder->state = ST_RUNNING_NOTE_ON; // successfully parsed note, we may get another
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parsable, try again from init state
        LOG(decoder, byte, "byte unparsable from state");
      }
      break;
    }

    // states specific to NOTE_OFF
    case ST_RUNNING_NOTE_OFF: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        decoder->current_note = MIDI_byte_to_note(byte);

        decoder->state = ST_NOTE_OFF_WITH_VALID_NOTE;
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parsable, try again from init state
        LOG(decoder, byte, "byte unparsable from state");
      }
      break;
    }
    case ST_NOTE_OFF_WITH_VALID_NOTE: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        MIDI_IMPL_decoder_buff_push(&(decoder->msg_buffer),
                                    (MIDI_Message){.type          = MIDI_MSG_TYPE_NOTE_OFF,
                                                   .channel       = decoder->current_channel,
                                                   .data.note_off = {.note = decoder->current_note, .velocity = byte}});

        decoder->state = ST_RUNNING_NOTE_OFF; // successfully parsed note, we may get another
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parsable, try again from init state
        LOG(decoder, byte, "byte unparsable from state");
      }
      break;
    }

    // states specific to CONTROL_CHANGE
    case ST_RUNNING_CONTROL_CHANGE: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        decoder->current_control = byte;

        decoder->state = ST_CONTROL_CHANGE_WITH_VALID_CONTROL;
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parsable, try again from init state
        LOG(decoder, byte, "byte unparsable from state");
      }
      break;
    }
    case ST_CONTROL_CHANGE_WITH_VALID_CONTROL: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        MIDI_IMPL_decoder_buff_push(&(decoder->msg_buffer),
                                    (MIDI_Message){.type    = MIDI_MSG_TYPE_CONTROL_CHANGE,
                                                   .channel = decoder->current_channel,

                                                   .data.control_change = {.control = decoder->current_control,
                                                                           .value   = byte}});

        decoder->state = ST_RUNNING_CONTROL_CHANGE;
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parsable, try again from init state
        LOG(decoder, byte, "byte unparsable from state");
      }
      break;
    }

    case ST_RUNNING_PROGRAM_CHANGE: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        MIDI_IMPL_decoder_buff_push(&(decoder->msg_buffer),
                                    (MIDI_Message){.type    = MIDI_MSG_TYPE_PROGRAM_CHANGE,
                                                   .channel = decoder->current_channel,

                                                   .data.program_change.program_id = byte});

        // stay in same running state
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parsable, try again from init state
        LOG(decoder, byte, "byte unparsable from state");
      }
      break;
    }

    case ST_RUNNING_PITCH_BEND: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        decoder->pitch_bend_lsb = byte;

        decoder->state = ST_PITCH_BEND_LSB_RECEIVED;
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parsable, try again from init state
        LOG(decoder, byte, "byte unparsable from state");
      }
      break;
    }
    case ST_PITCH_BEND_LSB_RECEIVED: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        MIDI_IMPL_decoder_buff_push(&(decoder->msg_buffer),
                                    (MIDI_Message){.type            = MIDI_MSG_TYPE_PITCH_BEND,
                                                   .channel         = decoder->current_channel,
                                                   .data.pitch_bend = {
                                                       .value = make_pitch_bend_value(decoder->pitch_bend_lsb, byte)}});

        decoder->state = ST_RUNNING_PITCH_BEND; // pitch bend parsed OK, maybe we get another
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parsable, try again from init state
        LOG(decoder, byte, "byte unparsable from state");
      }
      break;
    }

    case ST_RUNNING_AFTERTOUCH_MONO: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        MIDI_IMPL_decoder_buff_push(&(decoder->msg_buffer),
                                    (MIDI_Message){.type                       = MIDI_MSG_TYPE_AFTERTOUCH_MONO,
                                                   .channel                    = decoder->current_channel,
                                                   .data.aftertouch_mono.value = byte});

        // stay in same running state
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parsable, try again from init state
        LOG(decoder, byte, "byte unparsable from state");
      }
      break;
    }

    case ST_RUNNING_AFTERTOUCH_POLY: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        decoder->current_note = MIDI_byte_to_note(byte);

        decoder->state = ST_AFTERTOUCH_POLY_WITH_VALID_NOTE;
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parsable, try again from init state
        LOG(decoder, byte, "byte unparsable from state");
      }
      break;
    }
    case ST_AFTERTOUCH_POLY_WITH_VALID_NOTE: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        MIDI_IMPL_decoder_buff_push(&(decoder->msg_buffer),
                                    (MIDI_Message){.type                 = MIDI_MSG_TYPE_AFTERTOUCH_POLY,
                                                   .channel              = decoder->current_channel,
                                                   .data.aftertouch_poly = {.note  = decoder->current_note,
                                                                            .value = byte}});

        decoder->state = ST_RUNNING_AFTERTOUCH_POLY; // parsed OK, maybe we get another
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parsable, try again from init state
        LOG(decoder, byte, "byte unparsable from state");
      }
      break;
    }

    case ST_MTC_QUARTER_FRAME_STARTED: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        MIDI_IMPL_decoder_buff_push(&(decoder->msg_buffer),
                                    (MIDI_Message){.type               = MIDI_MSG_TYPE_MTC_QUARTER_FRAME,
                                                   .data.quarter_frame = {.type  = get_quarter_frame_type(byte),
                                                                          .value = get_quarter_frame_value(byte)}});

        decoder->state = ST_INIT; // parsed OK, but there is no running status for this message type
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parsable, try again from init state
        LOG(decoder, byte, "byte unparsable from state");
      }
      break;
    }

    case ST_SONG_POSITION_POINTER_STARTED: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        decoder->song_position_lsb = byte;

        decoder->state = ST_SONG_POSITION_POINTER_LSB_RECEIVED;
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parsable, try again from init state
        LOG(decoder, byte, "byte unparsable from state");
      }
      break;
    }
    case ST_SONG_POSITION_POINTER_LSB_RECEIVED: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        MIDI_IMPL_decoder_buff_push(&(decoder->msg_buffer),
                                    (MIDI_Message){.type                       = MIDI_MSG_TYPE_SONG_POSITION_POINTER,
                                                   .data.song_position_pointer = {
                                                       .value =
                                                           make_song_position_pointer_value(decoder->song_position_lsb,
                                                                                            byte)}});

        decoder->state = ST_INIT; // song position pointer parsed OK
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parsable, try again from init state
        LOG(decoder, byte, "byte unparsable from state");
      }
      break;
    }

    case ST_SONG_SELECT_STARTED: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        MIDI_IMPL_decoder_buff_push(&(decoder->msg_buffer),
                                    (MIDI_Message){.type             = MIDI_MSG_TYPE_SONG_SELECT,
                                                   .data.song_select = {.value = byte & 0x7f}});

        decoder->state = ST_INIT; // parsed OK, but there is no running status for this message type
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parsable, try again from init state
        LOG(decoder, byte, "byte unparsable from state");
      }
      break;
    }

    case ST_IN_SYSEX_SEQUENCE: {
      LOG(decoder, byte, "state entry");
      if(is_data_byte(byte)) {
        MIDI_IMPL_decoder_buff_push(&(decoder->msg_buffer),
                                    (MIDI_Message){.type            = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE,
                                                   .data.sysex_byte = {.byte = (byte & 0x7f),
                                                                       .sequence_number =
                                                                           (decoder->sysex_sequence_length & 0x1ff)}});
        decoder->sysex_sequence_length++;

        LOG(decoder, byte, "sysex byte added to sequence");

        // stay in same state, more bytes may be on the way
      } else {
        // byte not part of sysex sequence, end it
        MIDI_IMPL_decoder_buff_push(&(decoder->msg_buffer),
                                    (MIDI_Message){.type            = MIDI_MSG_TYPE_SYSEX_STOP,
                                                   .data.sysex_stop = {.sequence_length =
                                                                           (decoder->sysex_sequence_length & 0x7fff),
                                                                       .is_length_overflowed =
                                                                           (decoder->sysex_sequence_length > 0x7fff)}});

        decoder->sysex_sequence_length = 0;

        // the byte still needs to be parsed. take it back to init.
        try_byte_again = true;
        decoder->state = ST_INIT;
        LOG(decoder, byte, "finished sysex sequence");
      }
      break;
    }
    }
  } while(try_byte_again);

  return OK;
}

static bool is_channel_type(uint8_t status_byte) {
  return is_status(status_byte) && MIDI_is_channel_type(get_type(status_byte));
}

static bool is_single_byte_type(uint8_t status_byte) {
  return is_status(status_byte) && MIDI_is_single_byte_type(get_type(status_byte));
}

static uint8_t get_status_bit(uint8_t byte) { return byte & (1 << 7) /* 0b1000'0000 */; }
static uint8_t get_type_bits(uint8_t byte) {
  const bool include_second_nibble = ((byte & 0x7f) >= 0x70);
  return byte & (include_second_nibble ? 0x7f : 0x70);
}
static uint8_t get_channel_bits(uint8_t byte) { return byte & 0xf /* 0b0000'1111 */; }

static uint8_t get_channel(uint8_t status_byte) { return get_channel_bits(status_byte) + 1; }

static MIDI_QuarterFrameType get_quarter_frame_type(uint8_t byte) { return (byte & 0xf0) >> 4; }

static MIDI_QuarterFrameType get_quarter_frame_value(uint8_t byte) { return byte & 0x0f; }

static MIDI_MessageType get_type(uint8_t status_byte) { return (MIDI_MessageType)(get_type_bits(status_byte)); }

static bool is_status(uint8_t byte) { return get_status_bit(byte) != 0; }
static bool is_of_type(uint8_t byte, MIDI_MessageType type) { return is_status(byte) && (get_type(byte) == type); }

static bool is_real_time(uint8_t status_byte) {
  return is_status(status_byte) && MIDI_is_real_time_type(get_type(status_byte));
}

static bool is_system_reset(uint8_t byte) { return is_of_type(byte, MIDI_MSG_TYPE_SYSTEM_RESET); }

static bool is_data_byte(uint8_t byte) { return !is_status(byte); }

static int16_t make_pitch_bend_value(uint8_t lsb, uint8_t msb) {
  const int16_t mid = 0x40 << 7;
  return (((int16_t)(msb) << 7) | (int16_t)lsb) - mid;
}

static uint16_t make_song_position_pointer_value(uint8_t lsb, uint8_t high_byte) {
  return (lsb & 0x7f) | (((high_byte & 0x7f) << 7));
}