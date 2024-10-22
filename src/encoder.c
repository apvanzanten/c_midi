// MIT License
//
// Copyright (c) 2024 Arjen P. van Zanten
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

#include "encoder.h"

#include <cfac/log.h>

#define OK STAT_OK

typedef enum StateIdx {
  ST_INIT,
  ST_RUNNING_STATUS,
  ST_SYSTEM_MESSAGE_STARTED,
  ST_IN_SYSEX_SEQUENCE,
} StateIdx;

static void buff_init(MIDI_ByteBuffer * restrict buffer) { *buffer = (MIDI_ByteBuffer){0}; }

STAT_Val MIDI_encoder_init(MIDI_Encoder * restrict encoder) {
  if(encoder == NULL) return LOG_STAT(STAT_ERR_ARGS, "encoder is NULL");

  *encoder = (MIDI_Encoder){0};

  buff_init(&encoder->out_buffer);
  buff_init(&encoder->prio_out_buffer);

  encoder->state        = ST_INIT;
  encoder->prio_mode    = MIDI_ENCODER_PRIO_MODE_FIFO;
  encoder->current_type = MIDI_MSG_TYPE_NON_STD_NONE;

  return OK;
}

STAT_Val MIDI_encoder_reset(MIDI_Encoder * restrict encoder) {
  if(encoder == NULL) return LOG_STAT(STAT_ERR_ARGS, "encoder pointer is NULL");

  // remember only settings, reset everything else
  MIDI_EncoderPriorityMode prio_mode = encoder->prio_mode;

  buff_init(&(encoder->out_buffer));
  buff_init(&(encoder->prio_out_buffer));

  *encoder = (MIDI_Encoder){0};

  encoder->state        = ST_INIT;
  encoder->current_type = MIDI_MSG_TYPE_NON_STD_NONE;
  encoder->prio_mode    = prio_mode;

  return OK;
}

STAT_Val MIDI_encoder_set_prio_mode(MIDI_Encoder * restrict encoder, MIDI_EncoderPriorityMode prio) {
  if(encoder == NULL) return LOG_STAT(STAT_ERR_ARGS, "encoder is NULL");

  encoder->prio_mode = prio;

  return OK;
}

static uint8_t get_channel_status_byte(MIDI_MessageType type, MIDI_Channel channel) {
  return 0x80                    // status bit
         | (type & 0x70)         // type bits, channel types only use bits 4,5,6
         | ((channel - 1) & 0xf) // channel bits
      ;
}
static uint8_t get_system_status_byte(MIDI_MessageType type) { return 0x80 | type; }

static void push_byte(MIDI_Encoder * restrict encoder, uint8_t byte) {
  MIDI_IMPL_encoder_buff_push(&encoder->out_buffer, byte);
}

static uint8_t make_pitch_bend_lsb(int16_t value) {
  return value & 0x7f; // 0b0111'1111
}
static uint8_t make_pitch_bend_msb(int16_t value) {
  const int16_t mid_point = (0x40) << 7;
  return ((value + mid_point) >> 7) & 0x7f; // 0b0111'1111;
}

static uint8_t make_quarter_frame_byte(MIDI_QuarterFrame qf) { return ((qf.type & 0xf) << 4) | (qf.value & 0xf); }

static uint8_t make_song_position_pointer_lsb(MIDI_SongPositionPointer spp) { return (spp.value & 0x7f); }

static uint8_t make_song_position_pointer_msb(MIDI_SongPositionPointer spp) { return ((spp.value >> 7) & 0x7f); }

STAT_Val MIDI_encoder_push_message(MIDI_Encoder * restrict encoder, MIDI_Message msg) {
  if(encoder == NULL) return LOG_STAT(STAT_ERR_ARGS, "encoder is NULL");
  if(!MIDI_encoder_is_ready_to_receive(encoder)) return LOG_STAT(STAT_ERR_PRECONDITION, "encoder not ready");

  bool msg_finished = false;

  if(MIDI_is_real_time_msg(msg)) {
    // real time messages are sent immediately, as they are allowed to be interleaved with everything

    const uint8_t byte = get_system_status_byte(msg.type);

    if((encoder->prio_mode == MIDI_ENCODER_PRIO_MODE_REALTIME_FIRST) && MIDI_is_prioritizable_msg(msg)) {
      MIDI_IMPL_encoder_buff_push(&encoder->prio_out_buffer, byte);
    } else {
      MIDI_IMPL_encoder_buff_push(&encoder->out_buffer, byte);
    }

    if(msg.type == MIDI_MSG_TYPE_SYSTEM_RESET) {
      encoder->state                 = ST_INIT;
      encoder->current_channel       = 0;
      encoder->current_type          = MIDI_MSG_TYPE_NON_STD_NONE;
      encoder->sysex_sequence_length = 0;
    }

    msg_finished = true;
  }

  while(!msg_finished) {
    switch(encoder->state) {

    case ST_INIT: {
      if(MIDI_is_channel_msg(msg)) {
        push_byte(encoder, get_channel_status_byte(msg.type, msg.channel));
        encoder->current_type    = msg.type;
        encoder->current_channel = msg.channel;

        // status byte pushed, handle rest in ST_RUNNING_STATUS
        msg_finished   = false;
        encoder->state = ST_RUNNING_STATUS;
      } else if(msg.type == MIDI_MSG_TYPE_SYSEX_START) {
        push_byte(encoder, get_system_status_byte(msg.type));
        encoder->sysex_sequence_length = 0;

        msg_finished   = true;
        encoder->state = ST_IN_SYSEX_SEQUENCE;
      } else if(msg.type == MIDI_MSG_TYPE_SYSEX_STOP) {
        // sysex stop encountered while not in sysex sequence, simply ignore
      } else if(MIDI_is_system_msg(msg)) {
        // NOTE realtime system messages and sysex start/stops are already handled above

        push_byte(encoder, get_system_status_byte(msg.type));
        encoder->current_type = msg.type;

        msg_finished   = false;
        encoder->state = ST_SYSTEM_MESSAGE_STARTED;
      } else if(MIDI_is_non_standard_msg(msg)) {
        return LOG_STAT(STAT_ERR_PARSE,
                        "encountered unexpected non-standard message: %s",
                        MIDI_message_type_to_str(msg.type));
      } else {
        return LOG_STAT(STAT_ERR_ARGS,
                        "invalid or unsupported message type: %u (%s)",
                        msg.type,
                        MIDI_message_type_to_str(msg.type));
      }
      break;
    }

    case ST_RUNNING_STATUS: {
      if((msg.type != encoder->current_type) || (msg.channel != encoder->current_channel)) {
        encoder->current_channel = 0;
        encoder->current_type    = MIDI_MSG_TYPE_NON_STD_NONE;

        msg_finished   = false; // new status byte needed, try message again from init state
        encoder->state = ST_INIT;
      } else {
        switch(msg.type) {
        case MIDI_MSG_TYPE_NOTE_OFF:
          push_byte(encoder, msg.data.note_off.note);
          push_byte(encoder, msg.data.note_off.velocity);
          break;
        case MIDI_MSG_TYPE_NOTE_ON:
          push_byte(encoder, msg.data.note_on.note);
          push_byte(encoder, msg.data.note_on.velocity);
          break;
        case MIDI_MSG_TYPE_AFTERTOUCH_POLY:
          push_byte(encoder, msg.data.aftertouch_poly.note);
          push_byte(encoder, msg.data.aftertouch_poly.value);
          break;
        case MIDI_MSG_TYPE_CONTROL_CHANGE:
          push_byte(encoder, msg.data.control_change.control);
          push_byte(encoder, msg.data.control_change.value);
          break;
        case MIDI_MSG_TYPE_PROGRAM_CHANGE: push_byte(encoder, msg.data.program_change.program_id); break;
        case MIDI_MSG_TYPE_AFTERTOUCH_MONO: push_byte(encoder, msg.data.aftertouch_mono.value); break;
        case MIDI_MSG_TYPE_PITCH_BEND:
          push_byte(encoder, make_pitch_bend_lsb(msg.data.pitch_bend.value));
          push_byte(encoder, make_pitch_bend_msb(msg.data.pitch_bend.value));
          break;
        default:
          return LOG_STAT(STAT_ERR_ARGS,
                          "invalid or unsupported message type: %u (%s)",
                          msg.type,
                          MIDI_message_type_to_str(msg.type));
        }

        // message bytes successfully pushed, continue in running status
        msg_finished   = true;
        encoder->state = ST_RUNNING_STATUS;
      }
      break;
    }

    case ST_SYSTEM_MESSAGE_STARTED: {
      switch(msg.type) {
      case MIDI_MSG_TYPE_MTC_QUARTER_FRAME: push_byte(encoder, make_quarter_frame_byte(msg.data.quarter_frame)); break;
      case MIDI_MSG_TYPE_SONG_POSITION_POINTER:
        push_byte(encoder, make_song_position_pointer_lsb(msg.data.song_position_pointer));
        push_byte(encoder, make_song_position_pointer_msb(msg.data.song_position_pointer));
        break;
      case MIDI_MSG_TYPE_SONG_SELECT: push_byte(encoder, msg.data.song_select.value & 0x7f); break;
      case MIDI_MSG_TYPE_TUNE_REQUEST: /* single-byte message, all bytes already sent */ break;
      default:
        return LOG_STAT(STAT_ERR_ARGS,
                        "invalid or unsupported message type: %u (%s)",
                        msg.type,
                        MIDI_message_type_to_str(msg.type));
      }

      // message bytes successfully pushed, reset type and return to init
      encoder->current_type = MIDI_MSG_TYPE_NON_STD_NONE;
      msg_finished          = true;
      encoder->state        = ST_INIT;

      break;
    }
    case ST_IN_SYSEX_SEQUENCE: {
      switch(msg.type) {
      case MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE: {
        const uint16_t expected_sequence_number = (encoder->sysex_sequence_length & 0x1ff);
        if(msg.data.sysex_byte.sequence_number != expected_sequence_number) {
          LOG_STAT(STAT_WRN_RUNTIME,
                   "unexpected sequence number in sysex byte message. Expected %zu, received %zu. Byte: 0x%x.",
                   expected_sequence_number,
                   msg.data.sysex_byte.sequence_number,
                   msg.data.sysex_byte.byte);
        }

        push_byte(encoder, msg.data.sysex_byte.byte & 0x7f);
        encoder->sysex_sequence_length++;

        // msg finished, but more sysex messages may follow, so stay in same state
        msg_finished = true;
        break;
      }
      case MIDI_MSG_TYPE_SYSEX_STOP: {
        const uint16_t expected_length_value  = (encoder->sysex_sequence_length & 0x7fff);
        const bool     expected_overflow_flag = (encoder->sysex_sequence_length > 0x7fff);

        if((msg.data.sysex_stop.sequence_length != expected_length_value) ||
           (msg.data.sysex_stop.is_length_overflowed != expected_overflow_flag)) {
          LOG_STAT(STAT_WRN_RUNTIME,
                   "unexpected sequence length in sysex stop message. Total bytes encoded: %zu, resultant length value "
                   "%zu with overflow flag %s, received %zu with overflow flag %s",
                   encoder->sysex_sequence_length,
                   expected_length_value,
                   (expected_overflow_flag ? "true" : "false"),
                   msg.data.sysex_stop.sequence_length,
                   (msg.data.sysex_stop.is_length_overflowed ? "true" : "false"));
        }

        push_byte(encoder, get_system_status_byte(MIDI_MSG_TYPE_SYSEX_STOP));
        encoder->sysex_sequence_length = 0;

        msg_finished   = true;
        encoder->state = ST_INIT;
        break;
      }
      default: {
        LOG_STAT(STAT_WRN_RUNTIME,
                 "unexpected end of sysex sequence after %zu messages, non-sysex message of type %s encountered. "
                 "Sending sysex sequence stop byte and continuing.",
                 encoder->sysex_sequence_length,
                 MIDI_message_type_to_str(msg.type));

        push_byte(encoder, get_system_status_byte(MIDI_MSG_TYPE_SYSEX_STOP));
        encoder->sysex_sequence_length = 0;

        msg_finished   = false; // non-sysex message still must be handled
        encoder->state = ST_INIT;
        break;
      }
      }
      break;
    }
    }
  }

  return OK;
}