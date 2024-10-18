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
  EXPECT_EQ(&r, sizeof(MIDI_ProgramChange), 1);
  EXPECT_EQ(&r, sizeof(MIDI_PitchBend), 2);
  EXPECT_EQ(&r, sizeof(MIDI_AftertouchMono), 1);
  EXPECT_EQ(&r, sizeof(MIDI_AftertouchPoly), 2);
  EXPECT_EQ(&r, sizeof(MIDI_Message), 4);

  if(HAS_FAILED(&r)) {
    printf("%s=%zu\n", "sizeof(MIDI_NoteOff)", sizeof(MIDI_NoteOff));
    printf("%s=%zu\n", "sizeof(MIDI_NoteOn)", sizeof(MIDI_NoteOn));
    printf("%s=%zu\n", "sizeof(MIDI_ControlChange)", sizeof(MIDI_ControlChange));
    printf("%s=%zu\n", "sizeof(MIDI_ProgramChange)", sizeof(MIDI_ProgramChange));
    printf("%s=%zu\n", "sizeof(MIDI_PitchBend)", sizeof(MIDI_PitchBend));
    printf("%s=%zu\n", "sizeof(MIDI_AftertouchMono)", sizeof(MIDI_AftertouchMono));
    printf("%s=%zu\n", "sizeof(MIDI_AftertouchPoly)", sizeof(MIDI_AftertouchPoly));
    printf("%s=%zu\n", "sizeof(MIDI_Message)", sizeof(MIDI_Message));
  }

  return r;
}

static Result tst_to_string(void) {
  Result r = PASS;

  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=NOTE_OFF, channel=4, data=MIDI_NoteOff{note=A4, velocity=100}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.status_data =
                                                            MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_OFF, 4),
                                                        .data.note_off = {.note = MIDI_NOTE_A_4, .velocity = 100}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=NOTE_ON, channel=5, data=MIDI_NoteOn{note=D5, velocity=27}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.status_data =
                                                            MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_ON, 5),
                                                        .data.note_on = {.note = MIDI_NOTE_D_5, .velocity = 27}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=CONTROL_CHANGE, channel=3, "
                               "data=MIDI_ControlChange{control=EFFECT1, value=101}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.status_data =
                                                            MIDI_make_channel_status_data(MIDI_MSG_TYPE_CONTROL_CHANGE,
                                                                                          3),
                                                        .data.control_change = {.control = MIDI_CTRL_EFFECT1,
                                                                                .value   = 101}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=PITCH_BEND, channel=2, data=MIDI_PitchBend{value=-1023}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.status_data =
                                                            MIDI_make_channel_status_data(MIDI_MSG_TYPE_PITCH_BEND, 2),
                                                        .data.pitch_bend = {.value = -1023}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=PROGRAM_CHANGE, channel=3, data=MIDI_ProgramChange{program_id=12}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.status_data =
                                                            MIDI_make_channel_status_data(MIDI_MSG_TYPE_PROGRAM_CHANGE,
                                                                                          3),
                                                        .data.program_change.program_id = 12}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=AFTERTOUCH_MONO, channel=4, data=MIDI_AftertouchMono{value=13}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.status_data =
                                                            MIDI_make_channel_status_data(MIDI_MSG_TYPE_AFTERTOUCH_MONO,
                                                                                          4),
                                                        .data.aftertouch_mono.value = 13}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=AFTERTOUCH_POLY, channel=5, "
                               "data=MIDI_AftertouchPoly{note=D5, value=23}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.status_data =
                                                            MIDI_make_channel_status_data(MIDI_MSG_TYPE_AFTERTOUCH_POLY,
                                                                                          5),
                                                        .data.aftertouch_poly = {.note = MIDI_NOTE_D_5, .value = 23}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=TIMING_CLOCK, data=N/A}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.status_data =
                                                            MIDI_make_system_status_data(MIDI_MSG_TYPE_TIMING_CLOCK)}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=START, data=N/A}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.status_data =
                                                            MIDI_make_system_status_data(MIDI_MSG_TYPE_START)}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=CONTINUE, data=N/A}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.status_data =
                                                            MIDI_make_system_status_data(MIDI_MSG_TYPE_CONTINUE)}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=STOP, data=N/A}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.status_data =
                                                            MIDI_make_system_status_data(MIDI_MSG_TYPE_STOP)}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=ACTIVE_SENSING, data=N/A}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.status_data = MIDI_make_system_status_data(
                                                            MIDI_MSG_TYPE_ACTIVE_SENSING)}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=SYSTEM_RESET, data=N/A}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.status_data =
                                                            MIDI_make_system_status_data(MIDI_MSG_TYPE_SYSTEM_RESET)}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }

  return r;
}

static Result tst_to_string_short(void) {
  Result r = PASS;

  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "2:OFF{A4,100}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.status_data =
                                                                  MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_OFF,
                                                                                                2),
                                                              .data.note_off = {.note     = MIDI_NOTE_A_4,
                                                                                .velocity = 100}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "3:ON{D5,27}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.status_data =
                                                                  MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_ON,
                                                                                                3),
                                                              .data.note_on = {.note     = MIDI_NOTE_D_5,
                                                                               .velocity = 27}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "8:CC{EFFECT1,101}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(
                  str,
                  1024,
                  (MIDI_Message){.status_data         = MIDI_make_channel_status_data(MIDI_MSG_TYPE_CONTROL_CHANGE, 8),
                                 .data.control_change = {.control = MIDI_CTRL_EFFECT1, .value = 101}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "5:PC{101}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(
                  str,
                  1024,
                  (MIDI_Message){.status_data         = MIDI_make_channel_status_data(MIDI_MSG_TYPE_PROGRAM_CHANGE, 5),
                                 .data.program_change = {.program_id = 101}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "4:PB{-1023}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(
                  str,
                  1024,
                  (MIDI_Message){.status_data     = MIDI_make_channel_status_data(MIDI_MSG_TYPE_PITCH_BEND, 4),
                                 .data.pitch_bend = {.value = -1023}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "4:ATM{123}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(
                  str,
                  1024,
                  (MIDI_Message){.status_data = MIDI_make_channel_status_data(MIDI_MSG_TYPE_AFTERTOUCH_MONO, 4),
                                 .data.aftertouch_mono = {.value = 123}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "7:ATP{A5,123}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(
                  str,
                  1024,
                  (MIDI_Message){.status_data = MIDI_make_channel_status_data(MIDI_MSG_TYPE_AFTERTOUCH_POLY, 7),
                                 .data.aftertouch_poly = {.note = MIDI_NOTE_A_5, .value = 123}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "TCLK";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.status_data = MIDI_make_system_status_data(
                                                                  MIDI_MSG_TYPE_TIMING_CLOCK)}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "START";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.status_data =
                                                                  MIDI_make_system_status_data(MIDI_MSG_TYPE_START)}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "CONT";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.status_data = MIDI_make_system_status_data(
                                                                  MIDI_MSG_TYPE_CONTINUE)}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "STOP";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.status_data =
                                                                  MIDI_make_system_status_data(MIDI_MSG_TYPE_STOP)}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "ASENS";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.status_data = MIDI_make_system_status_data(
                                                                  MIDI_MSG_TYPE_ACTIVE_SENSING)}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "RESET";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.status_data = MIDI_make_system_status_data(
                                                                  MIDI_MSG_TYPE_SYSTEM_RESET)}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }

  return r;
}

#define EXPECT_MSG_EQUAL_TO_COPY(r_p, msg)                                                                             \
  do {                                                                                                                 \
    MIDI_Message copy_a = (msg);                                                                                       \
    MIDI_Message copy_b = copy_a;                                                                                      \
    EXPECT_TRUE((r_p), MIDI_message_equals(&copy_a, &copy_b));                                                         \
  } while(false);

static Result tst_equals_to_copy(void) {
  Result r = PASS;

  EXPECT_MSG_EQUAL_TO_COPY(&r, ((MIDI_Message){0}));

  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.status_data   = MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_OFF, 2),
                                           .data.note_off = {.note = MIDI_NOTE_A_4, .velocity = 12}}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.status_data   = MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_OFF, 4),
                                           .data.note_off = {.note = MIDI_NOTE_B_4, .velocity = 22}}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.status_data  = MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_ON, 1),
                                           .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 52}}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.status_data  = MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_ON, 3),
                                           .data.note_on = {.note = MIDI_NOTE_C_5, .velocity = 75}}));

  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.status_data =
                                               MIDI_make_channel_status_data(MIDI_MSG_TYPE_AFTERTOUCH_POLY, 2),
                                           .data.aftertouch_poly = {.note = MIDI_NOTE_E_3, .value = 23}}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.status_data =
                                               MIDI_make_channel_status_data(MIDI_MSG_TYPE_AFTERTOUCH_POLY, 5),
                                           .data.aftertouch_poly = {.note = MIDI_NOTE_F_8, .value = 3}}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.status_data =
                                               MIDI_make_channel_status_data(MIDI_MSG_TYPE_CONTROL_CHANGE, 7),
                                           .data.control_change = {.control = MIDI_CTRL_ATTACK_TIME, .value = 12}}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.status_data =
                                               MIDI_make_channel_status_data(MIDI_MSG_TYPE_CONTROL_CHANGE, 9),
                                           .data.control_change = {.control = MIDI_CTRL_BALANCE, .value = 15}}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.status_data =
                                               MIDI_make_channel_status_data(MIDI_MSG_TYPE_PROGRAM_CHANGE, 2),
                                           .data.program_change.program_id = 27}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.status_data =
                                               MIDI_make_channel_status_data(MIDI_MSG_TYPE_PROGRAM_CHANGE, 3),
                                           .data.program_change.program_id = 17}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.status_data =
                                               MIDI_make_channel_status_data(MIDI_MSG_TYPE_AFTERTOUCH_MONO, 5),
                                           .data.aftertouch_mono.value = 88}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.status_data =
                                               MIDI_make_channel_status_data(MIDI_MSG_TYPE_AFTERTOUCH_MONO, 9),
                                           .data.aftertouch_mono.value = 99}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.status_data = MIDI_make_channel_status_data(MIDI_MSG_TYPE_PITCH_BEND, 10),
                                           .data.pitch_bend.value = 10}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.status_data = MIDI_make_channel_status_data(MIDI_MSG_TYPE_PITCH_BEND, 11),
                                           .data.pitch_bend.value = 3}));

  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.status_data = MIDI_make_system_status_data(MIDI_MSG_TYPE_TIMING_CLOCK)}));

  return r;
}

static Result tst_note_not_equals(void) {
  Result r = PASS;

  EXPECT_FALSE(&r,
               MIDI_message_equals(&(MIDI_Message){.status_data =
                                                       MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_ON, 1)},
                                   &(MIDI_Message){.status_data =
                                                       MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_OFF, 1)}));
  EXPECT_FALSE(&r,
               MIDI_message_equals(&(MIDI_Message){.status_data =
                                                       MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_OFF, 1)},
                                   &(MIDI_Message){.status_data =
                                                       MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_ON, 1)}));

  EXPECT_FALSE(&r,
               MIDI_message_equals(&(MIDI_Message){.status_data =
                                                       MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_ON, 2),
                                                   .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}},
                                   &(MIDI_Message){.status_data =
                                                       MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_ON, 2),
                                                   .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 28}}));
  EXPECT_FALSE(&r,
               MIDI_message_equals(&(MIDI_Message){.status_data =
                                                       MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_ON, 2),
                                                   .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}},
                                   &(MIDI_Message){.status_data =
                                                       MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_ON, 3),
                                                   .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}}));
  EXPECT_FALSE(&r,
               MIDI_message_equals(&(MIDI_Message){.status_data =
                                                       MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_ON, 3),
                                                   .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 27}},
                                   &(MIDI_Message){.status_data =
                                                       MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_ON, 3),
                                                   .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}}));
  EXPECT_FALSE(&r,
               MIDI_message_equals(&(MIDI_Message){.status_data =
                                                       MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_OFF, 2),
                                                   .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}},
                                   &(MIDI_Message){.status_data =
                                                       MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_OFF, 2),
                                                   .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 28}}));
  EXPECT_FALSE(&r,
               MIDI_message_equals(&(MIDI_Message){.status_data =
                                                       MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_OFF, 2),
                                                   .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}},
                                   &(MIDI_Message){.status_data =
                                                       MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_OFF, 3),
                                                   .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}}));
  EXPECT_FALSE(&r,
               MIDI_message_equals(&(MIDI_Message){.status_data =
                                                       MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_OFF, 3),
                                                   .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 27}},
                                   &(MIDI_Message){.status_data =
                                                       MIDI_make_channel_status_data(MIDI_MSG_TYPE_NOTE_OFF, 3),
                                                   .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}}));

  return r;
}

static Result tst_equals_many(void) {
  Result r = PASS;

  MIDI_Message msgs[] = {

      MIDI_make_note_on_msg(1, (MIDI_NoteOn){.note = MIDI_NOTE_A_2, .velocity = 17}),
      MIDI_make_note_on_msg(1, (MIDI_NoteOn){.note = MIDI_NOTE_A_2, .velocity = 27}),
      MIDI_make_note_on_msg(1, (MIDI_NoteOn){.note = MIDI_NOTE_A_3, .velocity = 27}),
      MIDI_make_note_on_msg(2, (MIDI_NoteOn){.note = MIDI_NOTE_A_3, .velocity = 27}),

      MIDI_make_note_off_msg(3, (MIDI_NoteOff){.note = MIDI_NOTE_A_2, .velocity = 27}),
      MIDI_make_note_off_msg(3, (MIDI_NoteOff){.note = MIDI_NOTE_A_2, .velocity = 17}),
      MIDI_make_note_off_msg(3, (MIDI_NoteOff){.note = MIDI_NOTE_A_3, .velocity = 17}),
      MIDI_make_note_off_msg(4, (MIDI_NoteOff){.note = MIDI_NOTE_A_3, .velocity = 17}),

      MIDI_make_aftertouch_poly_msg(2, (MIDI_AftertouchPoly){.note = MIDI_NOTE_E_3, .value = 23}),
      MIDI_make_aftertouch_poly_msg(1, (MIDI_AftertouchPoly){.note = MIDI_NOTE_E_3, .value = 23}),
      MIDI_make_aftertouch_poly_msg(1, (MIDI_AftertouchPoly){.note = MIDI_NOTE_E_4, .value = 23}),
      MIDI_make_aftertouch_poly_msg(1, (MIDI_AftertouchPoly){.note = MIDI_NOTE_E_4, .value = 24}),

      MIDI_make_control_change_msg(7, (MIDI_ControlChange){.control = MIDI_CTRL_ATTACK_TIME, .value = 12}),
      MIDI_make_control_change_msg(2, (MIDI_ControlChange){.control = MIDI_CTRL_ATTACK_TIME, .value = 12}),
      MIDI_make_control_change_msg(2, (MIDI_ControlChange){.control = MIDI_CTRL_PAN, .value = 12}),
      MIDI_make_control_change_msg(2, (MIDI_ControlChange){.control = MIDI_CTRL_PAN, .value = 14}),

      MIDI_make_program_change_msg(2, (MIDI_ProgramChange){.program_id = 27}),
      MIDI_make_program_change_msg(1, (MIDI_ProgramChange){.program_id = 27}),
      MIDI_make_program_change_msg(1, (MIDI_ProgramChange){.program_id = 23}),

      MIDI_make_aftertouch_mono_msg(5, (MIDI_AftertouchMono){.value = 88}),
      MIDI_make_aftertouch_mono_msg(8, (MIDI_AftertouchMono){.value = 88}),
      MIDI_make_aftertouch_mono_msg(8, (MIDI_AftertouchMono){.value = 99}),

      MIDI_make_pitch_bend_msg(10, (MIDI_PitchBend){.value = 10}),
      MIDI_make_pitch_bend_msg(12, (MIDI_PitchBend){.value = 10}),
      MIDI_make_pitch_bend_msg(12, (MIDI_PitchBend){.value = 15}),

      MIDI_make_real_time_msg(MIDI_MSG_TYPE_TIMING_CLOCK),
      MIDI_make_real_time_msg(MIDI_MSG_TYPE_START),
      MIDI_make_real_time_msg(MIDI_MSG_TYPE_CONTINUE),
      MIDI_make_real_time_msg(MIDI_MSG_TYPE_STOP),
      MIDI_make_real_time_msg(MIDI_MSG_TYPE_ACTIVE_SENSING),
      MIDI_make_real_time_msg(MIDI_MSG_TYPE_SYSTEM_RESET),
  };

  const size_t num_msgs = sizeof(msgs) / sizeof(msgs[0]);

  for(size_t i = 0; i < num_msgs; i++) {
    MIDI_Message lhs = msgs[i];
    for(size_t j = 0; j < num_msgs; j++) {
      MIDI_Message rhs = msgs[j];

      EXPECT_EQ(&r, (i == j), MIDI_message_equals(&lhs, &rhs));

      if(HAS_FAILED(&r)) {
        printf("fail %zu ?= %zu\n", i, j);
        return r;
      }
    }
  }

  return r;
}

int main(void) {
  Test tests[] = {
      tst_size,
      tst_to_string,
      tst_to_string_short,
      tst_equals_to_copy,
      tst_note_not_equals,
      tst_equals_many,
  };

  return (run_tests(tests, sizeof(tests) / sizeof(Test)) == PASS) ? 0 : 1;
}