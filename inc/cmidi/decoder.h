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

// TODO shrink?
#define MIDI_OUT_BUFFER_CAPACITY 32

/* if a non-sysex non-realtime byte comes in during a sysex sequence, a sysex stop msg will be inserted, resulting in 2
 * messages generated for just 1 byte  */
#define MIDI_MAX_GENERATED_MESSAGES_PER_BYTE 2

typedef struct MIDI_MsgBuffer {
  MIDI_Message data[MIDI_OUT_BUFFER_CAPACITY];
  uint16_t     begin_idx;
  uint16_t     end_idx;
  bool         is_full;
} MIDI_MsgBuffer;

typedef enum MIDI_DecoderPriorityMode {
  MIDI_DECODE_PRIO_MODE_FIFO = 0,
  MIDI_DECODE_PRIO_MODE_REALTIME_FIRST,
} MIDI_DecoderPriorityMode;

typedef struct MIDI_Decoder {
  MIDI_DecoderPriorityMode prio_mode;

  uint8_t state;

  MIDI_MsgBuffer msg_buffer;
  MIDI_MsgBuffer prio_msg_buffer;

  MIDI_Note    current_note;
  MIDI_Channel current_channel;
  MIDI_Control current_control;

  uint8_t  pitch_bend_lsb;
  uint8_t  song_position_lsb;
  uint32_t sysex_sequence_length;

  uint8_t running_status;
} MIDI_Decoder;

STAT_Val MIDI_decoder_init(MIDI_Decoder * restrict decoder, MIDI_DecoderPriorityMode prio_mode);
STAT_Val MIDI_decoder_set_prio_mode(MIDI_Decoder * restrict decoder, MIDI_DecoderPriorityMode prio_mode);

STAT_Val MIDI_push_byte(MIDI_Decoder * restrict decoder, uint8_t byte);

static inline bool         MIDI_decoder_has_output(const MIDI_Decoder * restrict decoder);
static inline MIDI_Message MIDI_decoder_peek_msg(const MIDI_Decoder * restrict decoder);
static inline MIDI_Message MIDI_decoder_pop_msg(MIDI_Decoder * restrict decoder);
static inline bool         MIDI_decoder_is_ready(const MIDI_Decoder * restrict decoder);

static inline bool         MIDI_INT_buff_is_empty(const MIDI_MsgBuffer * restrict buffer);
static inline bool         MIDI_INT_buff_is_full(const MIDI_MsgBuffer * restrict buffer);
static inline uint8_t      MIDI_INT_buff_get_size(const MIDI_MsgBuffer * restrict buffer);
static inline uint8_t      MIDI_INT_buff_get_space_available(const MIDI_MsgBuffer * restrict buffer);
static inline MIDI_Message MIDI_INT_buff_pop(MIDI_MsgBuffer * restrict buffer);
static inline void         MIDI_INT_buff_push(MIDI_MsgBuffer * restrict buffer, MIDI_Message msg);
static inline MIDI_Message MIDI_INT_buff_peek(const MIDI_MsgBuffer * restrict buffer);

static inline bool MIDI_decoder_is_in_realtime_prio_mode(const MIDI_Decoder * restrict decoder) {
  return (decoder != NULL) && (decoder->prio_mode == MIDI_DECODE_PRIO_MODE_REALTIME_FIRST);
}

static inline bool MIDI_decoder_has_output(const MIDI_Decoder * restrict decoder) {
  return (decoder != NULL) &&
         (!MIDI_INT_buff_is_empty(&(decoder->msg_buffer)) || !MIDI_INT_buff_is_empty(&(decoder->prio_msg_buffer)));
}

static inline MIDI_Message MIDI_decoder_peek_msg(const MIDI_Decoder * restrict decoder) {
  if(decoder == NULL) return (MIDI_Message){0};
  if(!MIDI_INT_buff_is_empty(&(decoder->prio_msg_buffer))) return MIDI_INT_buff_peek(&(decoder->prio_msg_buffer));
  return MIDI_INT_buff_peek(&(decoder->msg_buffer));
}

static inline MIDI_Message MIDI_decoder_pop_msg(MIDI_Decoder * restrict decoder) {
  if(decoder == NULL) return (MIDI_Message){0};
  if(!MIDI_INT_buff_is_empty(&(decoder->prio_msg_buffer))) return MIDI_INT_buff_pop(&(decoder->prio_msg_buffer));
  return MIDI_INT_buff_pop(&(decoder->msg_buffer));
}

static inline bool MIDI_decoder_is_ready(const MIDI_Decoder * restrict decoder) {
  return (decoder != NULL) &&
         (MIDI_INT_buff_get_space_available(&decoder->msg_buffer) >= MIDI_MAX_GENERATED_MESSAGES_PER_BYTE) &&
         !MIDI_INT_buff_is_full(&(decoder->prio_msg_buffer));
}

static inline bool MIDI_INT_buff_is_empty(const MIDI_MsgBuffer * restrict buffer) {
  return (buffer->begin_idx == buffer->end_idx) && !buffer->is_full;
}
static inline bool MIDI_INT_buff_is_full(const MIDI_MsgBuffer * restrict buffer) { return buffer->is_full; }

static inline uint8_t MIDI_INT_buff_get_size(const MIDI_MsgBuffer * restrict buffer) {
  if(MIDI_INT_buff_is_empty(buffer)) return 0;
  return ((MIDI_OUT_BUFFER_CAPACITY - buffer->begin_idx) + buffer->end_idx);
}

static inline uint8_t MIDI_INT_buff_get_space_available(const MIDI_MsgBuffer * restrict buffer) {
  return MIDI_OUT_BUFFER_CAPACITY - MIDI_INT_buff_get_size(buffer);
}

static inline MIDI_Message MIDI_INT_buff_pop(MIDI_MsgBuffer * restrict buffer) {
  if(!MIDI_INT_buff_is_empty(buffer)) {
    const MIDI_Message m = buffer->data[buffer->begin_idx++];

    if(buffer->begin_idx == MIDI_OUT_BUFFER_CAPACITY) buffer->begin_idx = 0;
    buffer->is_full = false;

    return m;
  }
  return (MIDI_Message){0};
}
static inline void MIDI_INT_buff_push(MIDI_MsgBuffer * restrict buffer, MIDI_Message msg) {
  if(buffer->is_full) MIDI_INT_buff_pop(buffer);

  buffer->data[buffer->end_idx++] = msg;

  if(buffer->end_idx == MIDI_OUT_BUFFER_CAPACITY) buffer->end_idx = 0;
  if(buffer->end_idx == buffer->begin_idx) buffer->is_full = true;
}
static inline MIDI_Message MIDI_INT_buff_peek(const MIDI_MsgBuffer * restrict buffer) {
  return buffer->data[buffer->begin_idx];
}

#endif