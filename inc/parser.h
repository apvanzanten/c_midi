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

#ifndef C_MIDI_PARSER_H
#define C_MIDI_PARSER_H

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

typedef uint8_t MIDI_Channel;

typedef struct MIDI_Parser {
  MIDI_Channel channel;

  uint8_t        state;
  MIDI_MsgBuffer msg_buffer;

  MIDI_Note    current_note;
  MIDI_Control current_control;
  uint8_t      pitch_bend_lsb;
} MIDI_Parser;

STAT_Val MIDI_parser_init(MIDI_Parser * restrict parser, MIDI_Channel channel);

STAT_Val MIDI_parse_byte(MIDI_Parser * restrict parser, uint8_t byte);

static inline bool         MIDI_parser_has_output(const MIDI_Parser * restrict parser);
static inline MIDI_Message MIDI_parser_peek_msg(const MIDI_Parser * restrict parser);
static inline MIDI_Message MIDI_parser_pop_msg(MIDI_Parser * restrict parser);
static inline bool         MIDI_parser_is_ready(const MIDI_Parser * restrict parser);

static inline bool         MIDI_INT_buff_is_empty(const MIDI_MsgBuffer * restrict buffer);
static inline bool         MIDI_INT_buff_is_full(const MIDI_MsgBuffer * restrict buffer);
static inline MIDI_Message MIDI_INT_buff_pop(MIDI_MsgBuffer * restrict buffer);
static inline void         MIDI_INT_buff_push(MIDI_MsgBuffer * restrict buffer, MIDI_Message msg);
static inline MIDI_Message MIDI_INT_buff_peek(const MIDI_MsgBuffer * restrict buffer);

static inline bool MIDI_parser_has_output(const MIDI_Parser * restrict parser) {
  return (parser != NULL) && !MIDI_INT_buff_is_empty(&(parser->msg_buffer));
}

static inline MIDI_Message MIDI_parser_peek_msg(const MIDI_Parser * restrict parser) {
  if(parser == NULL) return (MIDI_Message){0};
  return MIDI_INT_buff_peek(&(parser->msg_buffer));
}

static inline MIDI_Message MIDI_parser_pop_msg(MIDI_Parser * restrict parser) {
  if(parser == NULL) return (MIDI_Message){0};
  return MIDI_INT_buff_pop(&(parser->msg_buffer));
}

static inline bool MIDI_parser_is_ready(const MIDI_Parser * restrict parser) {
  return (parser != NULL) && (!MIDI_INT_buff_is_full(&(parser->msg_buffer)));
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