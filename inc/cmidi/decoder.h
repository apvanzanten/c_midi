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

#ifndef C_MIDI_DECODER_H
#define C_MIDI_DECODER_H

#include <stdint.h>

#include "message.h"
#include "note.h"

#include <cfac/stat.h>

#define MIDI_OUT_BUFFER_SIZE 32

typedef struct MIDI_MsgBuffer {
  MIDI_Message data[MIDI_OUT_BUFFER_SIZE];
  uint16_t     begin_idx;
  uint16_t     end_idx;
  bool         is_full;
} MIDI_MsgBuffer;

typedef struct MIDI_Decoder {
  uint8_t        state;
  MIDI_MsgBuffer msg_buffer;

  MIDI_Note    current_note;
  MIDI_Control current_control;
  MIDI_Channel current_channel;

  uint8_t pitch_bend_lsb;

  uint8_t running_status;
} MIDI_Decoder;

STAT_Val MIDI_decoder_init(MIDI_Decoder * restrict decoder);

STAT_Val MIDI_push_byte(MIDI_Decoder * restrict decoder, uint8_t byte);

static inline bool         MIDI_decoder_has_output(const MIDI_Decoder * restrict decoder);
static inline MIDI_Message MIDI_decoder_peek_msg(const MIDI_Decoder * restrict decoder);
static inline MIDI_Message MIDI_decoder_pop_msg(MIDI_Decoder * restrict decoder);
static inline bool         MIDI_decoder_is_ready(const MIDI_Decoder * restrict decoder);

static inline bool         MIDI_INT_buff_is_empty(const MIDI_MsgBuffer * restrict buffer);
static inline bool         MIDI_INT_buff_is_full(const MIDI_MsgBuffer * restrict buffer);
static inline MIDI_Message MIDI_INT_buff_pop(MIDI_MsgBuffer * restrict buffer);
static inline void         MIDI_INT_buff_push(MIDI_MsgBuffer * restrict buffer, MIDI_Message msg);
static inline MIDI_Message MIDI_INT_buff_peek(const MIDI_MsgBuffer * restrict buffer);

static inline bool MIDI_decoder_has_output(const MIDI_Decoder * restrict decoder) {
  return (decoder != NULL) && !MIDI_INT_buff_is_empty(&(decoder->msg_buffer));
}

static inline MIDI_Message MIDI_decoder_peek_msg(const MIDI_Decoder * restrict decoder) {
  if(decoder == NULL) return (MIDI_Message){0};
  return MIDI_INT_buff_peek(&(decoder->msg_buffer));
}

static inline MIDI_Message MIDI_decoder_pop_msg(MIDI_Decoder * restrict decoder) {
  if(decoder == NULL) return (MIDI_Message){0};
  return MIDI_INT_buff_pop(&(decoder->msg_buffer));
}

static inline bool MIDI_decoder_is_ready(const MIDI_Decoder * restrict decoder) {
  return (decoder != NULL) && (!MIDI_INT_buff_is_full(&(decoder->msg_buffer)));
}

static inline bool MIDI_INT_buff_is_empty(const MIDI_MsgBuffer * restrict buffer) {
  return (buffer->begin_idx == buffer->end_idx) && !buffer->is_full;
}
static inline bool         MIDI_INT_buff_is_full(const MIDI_MsgBuffer * restrict buffer) { return buffer->is_full; }
static inline MIDI_Message MIDI_INT_buff_pop(MIDI_MsgBuffer * restrict buffer) {
  if(!MIDI_INT_buff_is_empty(buffer)) {
    const MIDI_Message m = buffer->data[buffer->begin_idx++];

    if(buffer->begin_idx == MIDI_OUT_BUFFER_SIZE) buffer->begin_idx = 0;
    buffer->is_full = false;

    return m;
  }
  return (MIDI_Message){0};
}
static inline void MIDI_INT_buff_push(MIDI_MsgBuffer * restrict buffer, MIDI_Message msg) {
  if(buffer->is_full) MIDI_INT_buff_pop(buffer);

  buffer->data[buffer->end_idx++] = msg;

  if(buffer->end_idx == MIDI_OUT_BUFFER_SIZE) buffer->end_idx = 0;
  if(buffer->end_idx == buffer->begin_idx) buffer->is_full = true;
}
static inline MIDI_Message MIDI_INT_buff_peek(const MIDI_MsgBuffer * restrict buffer) {
  return buffer->data[buffer->begin_idx];
}

#endif