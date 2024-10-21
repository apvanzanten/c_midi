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

#ifndef C_MIDI_ENCODER_H
#define C_MIDI_ENCODER_H

#include <stdint.h>

#include "message.h"
#include "note.h"

#include <cfac/stat.h>

#define MIDI_ENCODER_OUT_BUFFER_CAPACITY 32

#define MIDI_ENCODER_MAX_GENERATED_BYTES_PER_MESSAGE 3

typedef struct MIDI_ByteBuffer {
  uint8_t data[MIDI_ENCODER_OUT_BUFFER_CAPACITY];
  uint8_t begin_idx;
  uint8_t end_idx;
  bool    is_full;
} MIDI_ByteBuffer;

// TODO rt priority

typedef struct MIDI_Encoder {
  uint8_t state;

  MIDI_ByteBuffer out_buffer;

  MIDI_MessageType running_type;
  MIDI_Channel     running_channel;
} MIDI_Encoder;

STAT_Val MIDI_encoder_init(MIDI_Encoder * restrict encoder);
STAT_Val MIDI_encoder_push_message(MIDI_Encoder * restrict encoder, MIDI_Message message);

static inline bool    MIDI_encoder_has_output(const MIDI_Encoder * restrict encoder);
static inline uint8_t MIDI_encoder_peek_byte(const MIDI_Encoder * restrict encoder);
static inline uint8_t MIDI_encoder_pop_byte(MIDI_Encoder * restrict encoder);
static inline bool    MIDI_encoder_is_ready(const MIDI_Encoder * restrict encoder);

static inline bool    MIDI_IMPL_encoder_buff_is_empty(const MIDI_ByteBuffer * restrict buffer);
static inline bool    MIDI_IMPL_encoder_buff_is_full(const MIDI_ByteBuffer * restrict buffer);
static inline uint8_t MIDI_IMPL_encoder_buff_get_size(const MIDI_ByteBuffer * restrict buffer);
static inline uint8_t MIDI_IMPL_encoder_buff_get_space_available(const MIDI_ByteBuffer * restrict buffer);
static inline uint8_t MIDI_IMPL_encoder_buff_pop(MIDI_ByteBuffer * restrict buffer);
static inline void    MIDI_IMPL_encoder_buff_push(MIDI_ByteBuffer * restrict buffer, uint8_t byte);
static inline uint8_t MIDI_IMPL_encoder_buff_peek(const MIDI_ByteBuffer * restrict buffer);
static inline void    MIDI_IMPL_encoder_buff_clear(MIDI_ByteBuffer * restrict buffer);

static inline bool MIDI_encoder_has_output(const MIDI_Encoder * restrict encoder) {
  return (encoder != NULL) && !MIDI_IMPL_encoder_buff_is_empty(&encoder->out_buffer);
}

static inline uint8_t MIDI_encoder_peek_byte(const MIDI_Encoder * restrict encoder) {
  if(encoder == NULL) return 0;
  return MIDI_IMPL_encoder_buff_peek(&(encoder->out_buffer));
}

static inline uint8_t MIDI_encoder_pop_byte(MIDI_Encoder * restrict encoder) {
  if(encoder == NULL) return 0;
  return MIDI_IMPL_encoder_buff_pop(&(encoder->out_buffer));
}

static inline bool MIDI_encoder_is_ready(const MIDI_Encoder * restrict encoder) {
  return (encoder != NULL) && (MIDI_IMPL_encoder_buff_get_space_available(&encoder->out_buffer) >=
                               MIDI_ENCODER_MAX_GENERATED_BYTES_PER_MESSAGE);
}

static inline bool MIDI_IMPL_encoder_buff_is_empty(const MIDI_ByteBuffer * restrict buffer) {
  return (buffer->begin_idx == buffer->end_idx) && !buffer->is_full;
}
static inline bool MIDI_IMPL_encoder_buff_is_full(const MIDI_ByteBuffer * restrict buffer) { return buffer->is_full; }

static inline uint8_t MIDI_IMPL_encoder_buff_get_size(const MIDI_ByteBuffer * restrict buffer) {
  if(MIDI_IMPL_encoder_buff_is_empty(buffer)) return 0;
  return ((MIDI_ENCODER_OUT_BUFFER_CAPACITY - buffer->begin_idx) + buffer->end_idx);
}

static inline uint8_t MIDI_IMPL_encoder_buff_get_space_available(const MIDI_ByteBuffer * restrict buffer) {
  return MIDI_ENCODER_OUT_BUFFER_CAPACITY - MIDI_IMPL_encoder_buff_get_size(buffer);
}

static inline uint8_t MIDI_IMPL_encoder_buff_pop(MIDI_ByteBuffer * restrict buffer) {
  if(!MIDI_IMPL_encoder_buff_is_empty(buffer)) {
    const uint8_t byte = buffer->data[buffer->begin_idx++];

    if(buffer->begin_idx == MIDI_ENCODER_OUT_BUFFER_CAPACITY) buffer->begin_idx = 0;
    buffer->is_full = false;

    return byte;
  }
  return (uint8_t){0};
}
static inline void MIDI_IMPL_encoder_buff_push(MIDI_ByteBuffer * restrict buffer, uint8_t byte) {
  if(buffer->is_full) MIDI_IMPL_encoder_buff_pop(buffer);

  buffer->data[buffer->end_idx++] = byte;

  if(buffer->end_idx == MIDI_ENCODER_OUT_BUFFER_CAPACITY) buffer->end_idx = 0;
  if(buffer->end_idx == buffer->begin_idx) buffer->is_full = true;
}
static inline uint8_t MIDI_IMPL_encoder_buff_peek(const MIDI_ByteBuffer * restrict buffer) {
  return buffer->data[buffer->begin_idx];
}

static inline void MIDI_IMPL_encoder_buff_clear(MIDI_ByteBuffer * restrict buffer) { *buffer = (MIDI_ByteBuffer){0}; }

#endif
