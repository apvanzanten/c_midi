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
} StateIdx;

const char * state_to_string(StateIdx state) {
  switch(state) {
  case ST_INIT: return "ST_INIT";
  case ST_RUNNING_STATUS: return "ST_RUNNING_STATUS";
  }
  return "UNKNOWN";
}

static void buff_init(MIDI_ByteBuffer * restrict buffer) { *buffer = (MIDI_ByteBuffer){0}; }

STAT_Val MIDI_encoder_init(MIDI_Encoder * restrict encoder) {
  if(encoder == NULL) return LOG_STAT(STAT_ERR_ARGS, "encoder is NULL");

  *encoder = (MIDI_Encoder){0};

  buff_init(&encoder->out_buffer);

  encoder->state = ST_INIT;

  return OK;
}

static uint8_t get_channel_status_byte(MIDI_MessageType type, MIDI_Channel channel) {
  return 0x80                    // status bit
         | (type & 0x70)         // type bits, channel types only use bits 4,5,6
         | ((channel - 1) & 0xf) // channel bits
      ;
}

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

STAT_Val MIDI_encoder_push_message(MIDI_Encoder * restrict encoder, MIDI_Message msg) {
  if(encoder == NULL) return LOG_STAT(STAT_ERR_ARGS, "encoder is NULL");
  if(!MIDI_encoder_is_ready(encoder)) return LOG_STAT(STAT_ERR_PRECONDITION, "encoder not ready");

  bool msg_finished = false;

  if(MIDI_is_real_time_msg(msg)) {
    // real time messages are sent immediately, as they are allowed to be interleaved with everything
    push_byte(encoder, 0x80 | msg.type);

    if(msg.type == MIDI_MSG_TYPE_SYSTEM_RESET) {
      encoder->state           = ST_INIT;
      encoder->running_channel = 0;
      encoder->running_type    = 0;
    }

    msg_finished = true;
  }

  while(!msg_finished) {
    switch(encoder->state) {

    case ST_INIT: {
      if(MIDI_is_channel_msg(msg)) {
        push_byte(encoder, get_channel_status_byte(msg.type, msg.channel));
        encoder->running_type    = msg.type;
        encoder->running_channel = msg.channel;

        // status byte pushed, handle rest in ST_RUNNING_STATUS
        msg_finished   = false;
        encoder->state = ST_RUNNING_STATUS;
      } else {
        return LOG_STAT(STAT_ERR_ARGS,
                        "invalid or unsupported message type: %u (%s)",
                        msg.type,
                        MIDI_message_type_to_str(msg.type));
      }
      break;
    }

    case ST_RUNNING_STATUS: {
      if((msg.type != encoder->running_type) || (msg.channel != encoder->running_channel)) {
        encoder->running_channel = 0;
        encoder->running_type    = 0;

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
    }
  }

  return OK;
}