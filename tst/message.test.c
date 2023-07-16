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

#include <cfac/test_utils.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define OK STAT_OK

#include "message.h"

static Result tst_size(void) {
  Result r = PASS;

  EXPECT_EQ(&r, sizeof(MIDI_NoteOff), 2);
  EXPECT_EQ(&r, sizeof(MIDI_NoteOn), 2);
  EXPECT_EQ(&r, sizeof(MIDI_ControlChange), 2);
  EXPECT_EQ(&r, sizeof(MIDI_PitchBend), 2);

  EXPECT_EQ(&r, sizeof(MIDI_Message), 4);

  return r;
}

static Result tst_to_string(void) {
  Result r = PASS;

  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{.type=NOTE_OFF, .data=MIDI_NoteOff{.note=A4, .velocity=100}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.type          = MIDI_MSG_TYPE_NOTE_OFF,
                                                        .data.note_off = {.note = MIDI_NOTE_A_4, .velocity = 100}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{.type=NOTE_ON, .data=MIDI_NoteOn{.note=D5, .velocity=27}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_ON,
                                                        .data.note_on = {.note = MIDI_NOTE_D_5, .velocity = 27}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[] =
        "MIDI_Message{.type=CONTROL_CHANGE, .data=MIDI_ControlChange{.control=EFFECT1, .value=101}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.type                = MIDI_MSG_TYPE_CONTROL_CHANGE,
                                                        .data.control_change = {.control = MIDI_CTRL_EFFECT1,
                                                                                .value   = 101}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{.type=PITCH_BEND, .data=MIDI_PitchBend{.value=-1023}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.type            = MIDI_MSG_TYPE_PITCH_BEND,
                                                        .data.pitch_bend = {.value = -1023}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }

  return r;
}

static Result tst_to_string_short(void) {
  Result r = PASS;

  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "OFF{A4,100}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.type          = MIDI_MSG_TYPE_NOTE_OFF,
                                                              .data.note_off = {.note     = MIDI_NOTE_A_4,
                                                                                .velocity = 100}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "ON{D5,27}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_ON,
                                                              .data.note_on = {.note     = MIDI_NOTE_D_5,
                                                                               .velocity = 27}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "CC{EFFECT1,101}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.type                = MIDI_MSG_TYPE_CONTROL_CHANGE,
                                                              .data.control_change = {.control = MIDI_CTRL_EFFECT1,
                                                                                      .value   = 101}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "PB{-1023}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.type            = MIDI_MSG_TYPE_PITCH_BEND,
                                                              .data.pitch_bend = {.value = -1023}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }

  return r;
}

int main(void) {
  Test tests[] = {
      tst_size,
      tst_to_string,
      tst_to_string_short,
  };

  return (run_tests(tests, sizeof(tests) / sizeof(Test)) == PASS) ? 0 : 1;
}