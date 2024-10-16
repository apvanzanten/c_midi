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

#define NOTE_OFF_DEFAULT_VELOCITY 63

typedef enum State {
  ST_INIT,
  ST_RUNNING_NOTE_ON,
  ST_NOTE_ON_WITH_VALID_NOTE,
  ST_RUNNING_NOTE_OFF,
  ST_NOTE_OFF_WITH_VALID_NOTE,
  ST_RUNNING_CONTROL_CHANGE,
  ST_CONTROL_CHANGE_WITH_VALID_CONTROL,
  ST_RUNNING_PITCH_BEND,
  ST_PITCH_BEND_WITH_VALID_LSB
} State;

static bool    is_supported(uint8_t byte);
static uint8_t channel_to_byte(MIDI_Channel channel);
static uint8_t get_status_bit(uint8_t byte);
static uint8_t get_type_bits(uint8_t byte);
static uint8_t get_channel_bits(uint8_t byte);
static bool    is_status(uint8_t byte);
static bool    is_of_type(uint8_t byte, MIDI_MessageType type);
static bool    is_note_on(uint8_t byte);
static bool    is_note_off(uint8_t byte);
static bool    is_control_change(uint8_t byte);
static bool    is_pitch_bend(uint8_t byte);
static bool    is_on_channel(uint8_t byte, MIDI_Channel channel);
static bool    is_data_byte(uint8_t byte);

static int16_t make_pitch_bend_value(uint8_t lsb, uint8_t high_byte);

static void buff_init(MIDI_MsgBuffer * restrict buffer) { *buffer = (MIDI_MsgBuffer){0}; }

STAT_Val MIDI_decoder_init(MIDI_Decoder * restrict decoder, MIDI_Channel channel) {
  if(decoder == NULL) return LOG_STAT(STAT_ERR_ARGS, "decoder pointer is NULL");
  if(!(channel >= 1 && channel <= 16)) return LOG_STAT(STAT_ERR_ARGS, "invalid channel %d, should be in range [1,16]");

  *decoder = (MIDI_Decoder){0};
  buff_init(&(decoder->msg_buffer));

  decoder->channel = channel;
  decoder->state   = ST_INIT;

  return OK;
}

STAT_Val MIDI_push_byte(MIDI_Decoder * restrict decoder, uint8_t byte) {
  if(decoder == NULL) return LOG_STAT(STAT_ERR_ARGS, "decoder pointer is NULL");
  if(!MIDI_decoder_is_ready(decoder)) return LOG_STAT(STAT_ERR_PRECONDITION, "decoder not ready");
  if(!is_supported(byte)) return OK; // silently skip unsupported bytes

  if(is_status(byte) && !is_on_channel(byte, decoder->channel)) {
    // regardless of what state we're in, if we get a message for another channel, we reset to init, as a new status
    // message must come in to indicate we're back on the correct channel
    decoder->state = ST_INIT;
    return OK;
  }

  bool try_byte_again = false;

  do {
    switch(decoder->state) {
    case ST_INIT: {
      if(is_note_on(byte)) {
        decoder->state = ST_RUNNING_NOTE_ON;
      } else if(is_note_off(byte)) {
        decoder->state = ST_RUNNING_NOTE_OFF;
      } else if(is_control_change(byte)) {
        decoder->state = ST_RUNNING_CONTROL_CHANGE;
      } else if(is_pitch_bend(byte)) {
        decoder->state = ST_RUNNING_PITCH_BEND;
      } else {
        // do nothing, maintain the init state and move to next byte, as this is an unparseable byte
        // probably it belongs to message for another channel
      }
      try_byte_again = false; // we never try again after going through the init state, as there would be no improvement
      break;
    }

    // states specific to NOTE_ON
    case ST_RUNNING_NOTE_ON: {
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
      if(is_data_byte(byte)) {
        const uint8_t      velocity = byte;
        const MIDI_Message msg =
            ((velocity > 0)
                 ? ((MIDI_Message){.type                        = MIDI_MSG_TYPE_NOTE_ON,
                                   .as.channel_msg.data.note_on = {.note     = decoder->current_note,
                                                                   .velocity = velocity}})
                 : ((MIDI_Message){.type                         = MIDI_MSG_TYPE_NOTE_OFF,
                                   .as.channel_msg.data.note_off = {.note     = decoder->current_note,
                                                                    .velocity = NOTE_OFF_DEFAULT_VELOCITY}}));
        MIDI_INT_buff_push(&(decoder->msg_buffer), msg);

        decoder->state = ST_RUNNING_NOTE_ON; // succesfully parsed note, we may get another
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parseable, try again from init state
      }
      break;
    }

    // states specific to NOTE_OFF
    case ST_RUNNING_NOTE_OFF: {
      if(is_data_byte(byte)) {
        decoder->current_note = MIDI_byte_to_note(byte);

        decoder->state = ST_NOTE_OFF_WITH_VALID_NOTE;
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parseable, try again from init state
      }
      break;
    }
    case ST_NOTE_OFF_WITH_VALID_NOTE: {
      if(is_data_byte(byte)) {
        MIDI_INT_buff_push(&(decoder->msg_buffer),
                           (MIDI_Message){.type                         = MIDI_MSG_TYPE_NOTE_OFF,
                                          .as.channel_msg.data.note_off = {.note     = decoder->current_note,
                                                                           .velocity = byte}});

        decoder->state = ST_RUNNING_NOTE_OFF; // succesfully parsed note, we may get another
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parseable, try again from init state
      }
      break;
    }

    // states specific to CONTROL_CHANGE
    case ST_RUNNING_CONTROL_CHANGE: {
      if(is_data_byte(byte)) {
        decoder->current_control = byte;

        decoder->state = ST_CONTROL_CHANGE_WITH_VALID_CONTROL;
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parseable, try again from init state
      }
      break;
    }
    case ST_CONTROL_CHANGE_WITH_VALID_CONTROL: {
      if(is_data_byte(byte)) {
        MIDI_INT_buff_push(&(decoder->msg_buffer),
                           (MIDI_Message){.type                               = MIDI_MSG_TYPE_CONTROL_CHANGE,
                                          .as.channel_msg.data.control_change = {.control = decoder->current_control,
                                                                                 .value   = byte}});

        decoder->state = ST_RUNNING_CONTROL_CHANGE;
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parseable, try again from init state
      }
      break;
    }

    // states specific to PITCH_BEND
    case ST_RUNNING_PITCH_BEND: {
      if(is_data_byte(byte)) {
        decoder->pitch_bend_lsb = byte;

        decoder->state = ST_PITCH_BEND_WITH_VALID_LSB;
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parseable, try again from init state
      }
      break;
    }
    case ST_PITCH_BEND_WITH_VALID_LSB: {
      if(is_data_byte(byte)) {
        MIDI_INT_buff_push(&(decoder->msg_buffer),
                           (MIDI_Message){.type                           = MIDI_MSG_TYPE_PITCH_BEND,
                                          .as.channel_msg.data.pitch_bend = {
                                              .value = make_pitch_bend_value(decoder->pitch_bend_lsb, byte)}});

        decoder->state = ST_RUNNING_PITCH_BEND; // pitch bend parsed OK, maybe we get another
      } else {
        try_byte_again = true;
        decoder->state = ST_INIT; // byte not parseable, try again from init state
      }
      break;
    }
    default: decoder->state = ST_INIT; // should never get here, best effort fix is to go back to init
    }

  } while(try_byte_again);

  return OK;
}

static bool is_supported(uint8_t byte) {
  return !is_status(byte) ||
         (is_of_type(byte, MIDI_MSG_TYPE_NOTE_ON) || is_of_type(byte, MIDI_MSG_TYPE_NOTE_OFF) ||
          is_of_type(byte, MIDI_MSG_TYPE_CONTROL_CHANGE) || is_of_type(byte, MIDI_MSG_TYPE_PITCH_BEND));
}

static uint8_t channel_to_byte(MIDI_Channel channel) { return ((uint8_t)(channel)-1); }

static uint8_t get_status_bit(uint8_t byte) { return byte & (1 << 7) /* 0b1000'0000 */; }
static uint8_t get_type_bits(uint8_t byte) { return byte & (0x7 << 4) /* 0b0111'0000 */; }
static uint8_t get_channel_bits(uint8_t byte) { return byte & 0xf /* 0b0000'1111 */; }

static bool is_status(uint8_t byte) { return get_status_bit(byte) != 0; }
static bool is_of_type(uint8_t byte, MIDI_MessageType type) {
  return is_status(byte) && (get_type_bits(byte) == (MIDI_type_to_byte(type) << 4));
}

static bool is_note_on(uint8_t byte) { return is_of_type(byte, MIDI_MSG_TYPE_NOTE_ON); }
static bool is_note_off(uint8_t byte) { return is_of_type(byte, MIDI_MSG_TYPE_NOTE_OFF); }
static bool is_control_change(uint8_t byte) { return is_of_type(byte, MIDI_MSG_TYPE_CONTROL_CHANGE); }
static bool is_pitch_bend(uint8_t byte) { return is_of_type(byte, MIDI_MSG_TYPE_PITCH_BEND); }

static bool is_on_channel(uint8_t byte, MIDI_Channel channel) {
  return get_channel_bits(byte) == channel_to_byte(channel);
}

static bool is_data_byte(uint8_t byte) { return !is_status(byte); }

static int16_t make_pitch_bend_value(uint8_t lsb, uint8_t msb) {
  const int16_t mid = 0x40 << 7;
  return (((int16_t)(msb) << 7) | (int16_t)lsb) - mid;
}