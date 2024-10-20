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

  // we don't static assert this, as this test can give quicker feedback on what the size actually is.

  EXPECT_EQ(&r, sizeof(MIDI_NoteOff), 2);
  EXPECT_EQ(&r, sizeof(MIDI_NoteOn), 2);
  EXPECT_EQ(&r, sizeof(MIDI_ControlChange), 2);
  EXPECT_EQ(&r, sizeof(MIDI_ProgramChange), 1);
  EXPECT_EQ(&r, sizeof(MIDI_PitchBend), 2);
  EXPECT_EQ(&r, sizeof(MIDI_AftertouchMono), 1);
  EXPECT_EQ(&r, sizeof(MIDI_AftertouchPoly), 2);
  EXPECT_EQ(&r, sizeof(MIDI_QuarterFrame), 1);
  EXPECT_EQ(&r, sizeof(MIDI_SongPositionPointer), 2);
  EXPECT_EQ(&r, sizeof(MIDI_SysexByte), 2);
  EXPECT_EQ(&r, sizeof(MIDI_SysexStop), 2);
  EXPECT_EQ(&r, sizeof(MIDI_Message), 4);

  if(HAS_FAILED(&r)) {
    printf("%s=%zu\n", "sizeof(MIDI_NoteOff)", sizeof(MIDI_NoteOff));
    printf("%s=%zu\n", "sizeof(MIDI_NoteOn)", sizeof(MIDI_NoteOn));
    printf("%s=%zu\n", "sizeof(MIDI_ControlChange)", sizeof(MIDI_ControlChange));
    printf("%s=%zu\n", "sizeof(MIDI_ProgramChange)", sizeof(MIDI_ProgramChange));
    printf("%s=%zu\n", "sizeof(MIDI_PitchBend)", sizeof(MIDI_PitchBend));
    printf("%s=%zu\n", "sizeof(MIDI_AftertouchMono)", sizeof(MIDI_AftertouchMono));
    printf("%s=%zu\n", "sizeof(MIDI_AftertouchPoly)", sizeof(MIDI_AftertouchPoly));
    printf("%s=%zu\n", "sizeof(MIDI_QuarterFrame)", sizeof(MIDI_QuarterFrame));
    printf("%s=%zu\n", "sizeof(MIDI_SongPositionPointer)", sizeof(MIDI_SongPositionPointer));
    printf("%s=%zu\n", "sizeof(MIDI_SysexByte)", sizeof(MIDI_SysexByte));
    printf("%s=%zu\n", "sizeof(MIDI_SysexStop)", sizeof(MIDI_SysexStop));
    printf("%s=%zu\n", "sizeof(MIDI_Message)", sizeof(MIDI_Message));
  }

  return r;
}

static Result tst_to_string(void) {
  Result r = PASS;

  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=NOTE_OFF, channel=4, data=NoteOff{note=A4, velocity=100}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.type          = MIDI_MSG_TYPE_NOTE_OFF,
                                                        .channel       = 4,
                                                        .data.note_off = {.note = MIDI_NOTE_A_4, .velocity = 100}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=NOTE_ON, channel=5, data=NoteOn{note=D5, velocity=27}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_ON,
                                                        .channel      = 5,
                                                        .data.note_on = {.note = MIDI_NOTE_D_5, .velocity = 27}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=CONTROL_CHANGE, channel=3, "
                               "data=ControlChange{control=EFFECT1, value=101}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.type                = MIDI_MSG_TYPE_CONTROL_CHANGE,
                                                        .channel             = 3,
                                                        .data.control_change = {.control = MIDI_CTRL_EFFECT1,
                                                                                .value   = 101}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=PITCH_BEND, channel=2, data=PitchBend{value=-1023}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.type            = MIDI_MSG_TYPE_PITCH_BEND,
                                                        .channel         = 2,
                                                        .data.pitch_bend = {.value = -1023}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=PROGRAM_CHANGE, channel=3, data=ProgramChange{program_id=12}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.type                           = MIDI_MSG_TYPE_PROGRAM_CHANGE,
                                                        .channel                        = 3,
                                                        .data.program_change.program_id = 12}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=AFTERTOUCH_MONO, channel=4, data=AftertouchMono{value=13}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.type                       = MIDI_MSG_TYPE_AFTERTOUCH_MONO,
                                                        .channel                    = 4,
                                                        .data.aftertouch_mono.value = 13}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=AFTERTOUCH_POLY, channel=5, "
                               "data=AftertouchPoly{note=D5, value=23}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.type                 = MIDI_MSG_TYPE_AFTERTOUCH_POLY,
                                                        .channel              = 5,
                                                        .data.aftertouch_poly = {.note = MIDI_NOTE_D_5, .value = 23}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=TIMING_CLOCK, data=N/A}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str, 1024, (MIDI_Message){.type = MIDI_MSG_TYPE_TIMING_CLOCK}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=START, data=N/A}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str, 1024, (MIDI_Message){.type = MIDI_MSG_TYPE_START}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=CONTINUE, data=N/A}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str, 1024, (MIDI_Message){.type = MIDI_MSG_TYPE_CONTINUE}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=STOP, data=N/A}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str, 1024, (MIDI_Message){.type = MIDI_MSG_TYPE_STOP}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=ACTIVE_SENSING, data=N/A}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str, 1024, (MIDI_Message){.type = MIDI_MSG_TYPE_ACTIVE_SENSING}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=SYSTEM_RESET, data=N/A}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str, 1024, (MIDI_Message){.type = MIDI_MSG_TYPE_SYSTEM_RESET}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[] =
        "MIDI_Message{type=MTC_QUARTER_FRAME, data=QuarterFrame{type=SECONDS_LOW_NIBBLE, value=13}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.type               = MIDI_MSG_TYPE_MTC_QUARTER_FRAME,
                                                        .data.quarter_frame = {.type  = MIDI_QF_TYPE_SECONDS_LOW_NIBBLE,
                                                                               .value = 13}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "MIDI_Message{type=SONG_POSITION_POINTER, data=SongPositionPointer{value=12345}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.type = MIDI_MSG_TYPE_SONG_POSITION_POINTER,
                                                        .data.song_position_pointer = {.value = 12345}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[] =
        "MIDI_Message{type=SYSEX_STOP, data=SysexStop{sequence_length=123, is_length_overflowed=false}}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer(str,
                                         1024,
                                         (MIDI_Message){.type            = MIDI_MSG_TYPE_SYSEX_STOP,
                                                        .data.sysex_stop = {.sequence_length      = 123,
                                                                            .is_length_overflowed = false}}));
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
                                               (MIDI_Message){.type          = MIDI_MSG_TYPE_NOTE_OFF,
                                                              .channel       = 2,
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
                                               (MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_ON,
                                                              .channel      = 3,
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
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.type                = MIDI_MSG_TYPE_CONTROL_CHANGE,
                                                              .channel             = 8,
                                                              .data.control_change = {.control = MIDI_CTRL_EFFECT1,
                                                                                      .value   = 101}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "5:PC{101}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.type                = MIDI_MSG_TYPE_PROGRAM_CHANGE,
                                                              .channel             = 5,
                                                              .data.program_change = {.program_id = 101}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "4:PB{-1023}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.type            = MIDI_MSG_TYPE_PITCH_BEND,
                                                              .channel         = 4,
                                                              .data.pitch_bend = {.value = -1023}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "4:ATM{123}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.type                 = MIDI_MSG_TYPE_AFTERTOUCH_MONO,
                                                              .channel              = 4,
                                                              .data.aftertouch_mono = {.value = 123}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "7:ATP{A5,123}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.type                 = MIDI_MSG_TYPE_AFTERTOUCH_POLY,
                                                              .channel              = 7,
                                                              .data.aftertouch_poly = {.note  = MIDI_NOTE_A_5,
                                                                                       .value = 123}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "TCLK";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str, 1024, (MIDI_Message){.type = MIDI_MSG_TYPE_TIMING_CLOCK}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "START";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str, 1024, (MIDI_Message){.type = MIDI_MSG_TYPE_START}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "CONT";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str, 1024, (MIDI_Message){.type = MIDI_MSG_TYPE_CONTINUE}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "STOP";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str, 1024, (MIDI_Message){.type = MIDI_MSG_TYPE_STOP}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "ASENS";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str, 1024, (MIDI_Message){.type = MIDI_MSG_TYPE_ACTIVE_SENSING}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "RESET";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str, 1024, (MIDI_Message){.type = MIDI_MSG_TYPE_SYSTEM_RESET}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "QF{MIN_L,10}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.type = MIDI_MSG_TYPE_MTC_QUARTER_FRAME,
                                                              .data.quarter_frame =
                                                                  {.type  = MIDI_QF_TYPE_MINUTES_LOW_NIBBLE,
                                                                   .value = 10}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "SPP{12345}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.type = MIDI_MSG_TYPE_SONG_POSITION_POINTER,
                                                              .data.song_position_pointer = {.value = 12345}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "SSX_STOP{4334+}";
    EXPECT_EQ(&r,
              strlen(expect_str),
              MIDI_message_to_str_buffer_short(str,
                                               1024,
                                               (MIDI_Message){.type            = MIDI_MSG_TYPE_SYSEX_STOP,
                                                              .data.sysex_stop = {.sequence_length      = 4334,
                                                                                  .is_length_overflowed = true}}));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }

  return r;
}

#define EXPECT_MSG_EQUAL_TO_COPY(r_p, msg)                                                                             \
  do {                                                                                                                 \
    MIDI_Message copy = (msg);                                                                                         \
    EXPECT_TRUE((r_p), MIDI_message_equals(copy, copy));                                                               \
  } while(false);

static Result tst_equals_to_copy(void) {
  Result r = PASS;

  EXPECT_MSG_EQUAL_TO_COPY(&r, ((MIDI_Message){0}));

  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.type          = MIDI_MSG_TYPE_NOTE_OFF,
                                           .channel       = 2,
                                           .data.note_off = {.note = MIDI_NOTE_A_4, .velocity = 12}}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.type          = MIDI_MSG_TYPE_NOTE_OFF,
                                           .channel       = 4,
                                           .data.note_off = {.note = MIDI_NOTE_B_4, .velocity = 22}}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_ON,
                                           .channel      = 1,
                                           .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 52}}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_ON,
                                           .channel      = 3,
                                           .data.note_on = {.note = MIDI_NOTE_C_5, .velocity = 75}}));

  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.type                 = MIDI_MSG_TYPE_AFTERTOUCH_POLY,
                                           .channel              = 2,
                                           .data.aftertouch_poly = {.note = MIDI_NOTE_E_3, .value = 23}}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.type                 = MIDI_MSG_TYPE_AFTERTOUCH_POLY,
                                           .channel              = 5,
                                           .data.aftertouch_poly = {.note = MIDI_NOTE_F_8, .value = 3}}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.type                = MIDI_MSG_TYPE_CONTROL_CHANGE,
                                           .channel             = 7,
                                           .data.control_change = {.control = MIDI_CTRL_ATTACK_TIME, .value = 12}}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.type                = MIDI_MSG_TYPE_CONTROL_CHANGE,
                                           .channel             = 9,
                                           .data.control_change = {.control = MIDI_CTRL_BALANCE, .value = 15}}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.type                           = MIDI_MSG_TYPE_PROGRAM_CHANGE,
                                           .channel                        = 2,
                                           .data.program_change.program_id = 27}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.type                           = MIDI_MSG_TYPE_PROGRAM_CHANGE,
                                           .channel                        = 3,
                                           .data.program_change.program_id = 17}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.type                       = MIDI_MSG_TYPE_AFTERTOUCH_MONO,
                                           .channel                    = 5,
                                           .data.aftertouch_mono.value = 88}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.type                       = MIDI_MSG_TYPE_AFTERTOUCH_MONO,
                                           .channel                    = 9,
                                           .data.aftertouch_mono.value = 99}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.type                  = MIDI_MSG_TYPE_PITCH_BEND,
                                           .channel               = 10,
                                           .data.pitch_bend.value = 10}));
  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.type                  = MIDI_MSG_TYPE_PITCH_BEND,
                                           .channel               = 11,
                                           .data.pitch_bend.value = 3}));

  EXPECT_MSG_EQUAL_TO_COPY(&r, ((MIDI_Message){.type = MIDI_MSG_TYPE_TIMING_CLOCK}));

  EXPECT_MSG_EQUAL_TO_COPY(&r,
                           ((MIDI_Message){.type               = MIDI_MSG_TYPE_MTC_QUARTER_FRAME,
                                           .data.quarter_frame = {.type  = MIDI_QF_TYPE_SECONDS_HIGH_NIBBLE,
                                                                  .value = 3}}));

  return r;
}

static Result tst_note_not_equals(void) {
  Result r = PASS;

  EXPECT_FALSE(&r,
               MIDI_message_equals((MIDI_Message){.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1},
                                   (MIDI_Message){.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 1}));
  EXPECT_FALSE(&r,
               MIDI_message_equals((MIDI_Message){.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 1},
                                   (MIDI_Message){.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1}));

  EXPECT_FALSE(&r,
               MIDI_message_equals((MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_ON,
                                                  .channel      = 2,
                                                  .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}},
                                   (MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_ON,
                                                  .channel      = 2,
                                                  .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 28}}));
  EXPECT_FALSE(&r,
               MIDI_message_equals((MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_ON,
                                                  .channel      = 2,
                                                  .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}},
                                   (MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_ON,
                                                  .channel      = 3,
                                                  .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}}));
  EXPECT_FALSE(&r,
               MIDI_message_equals((MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_ON,
                                                  .channel      = 3,
                                                  .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 27}},
                                   (MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_ON,
                                                  .channel      = 3,
                                                  .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}}));
  EXPECT_FALSE(&r,
               MIDI_message_equals((MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_OFF,
                                                  .channel      = 2,
                                                  .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}},
                                   (MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_OFF,
                                                  .channel      = 2,
                                                  .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 28}}));
  EXPECT_FALSE(&r,
               MIDI_message_equals((MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_OFF,
                                                  .channel      = 2,
                                                  .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}},
                                   (MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_OFF,
                                                  .channel      = 3,
                                                  .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}}));
  EXPECT_FALSE(&r,
               MIDI_message_equals((MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_OFF,
                                                  .channel      = 3,
                                                  .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 27}},
                                   (MIDI_Message){.type         = MIDI_MSG_TYPE_NOTE_OFF,
                                                  .channel      = 3,
                                                  .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}}));

  return r;
}

static Result tst_equals_many(void) {
  Result r = PASS;

  MIDI_Message msgs[] = {

      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1, .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 17}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1, .data.note_on = {.note = MIDI_NOTE_A_2, .velocity = 27}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 1, .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 27}},
      {.type = MIDI_MSG_TYPE_NOTE_ON, .channel = 2, .data.note_on = {.note = MIDI_NOTE_A_3, .velocity = 27}},

      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 3, .data.note_off = {.note = MIDI_NOTE_A_2, .velocity = 27}},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 3, .data.note_off = {.note = MIDI_NOTE_A_2, .velocity = 17}},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 3, .data.note_off = {.note = MIDI_NOTE_A_3, .velocity = 17}},
      {.type = MIDI_MSG_TYPE_NOTE_OFF, .channel = 4, .data.note_off = {.note = MIDI_NOTE_A_3, .velocity = 17}},

      {.type                 = MIDI_MSG_TYPE_AFTERTOUCH_POLY,
       .channel              = 2,
       .data.aftertouch_poly = {.note = MIDI_NOTE_E_3, .value = 23}},
      {.type                 = MIDI_MSG_TYPE_AFTERTOUCH_POLY,
       .channel              = 1,
       .data.aftertouch_poly = {.note = MIDI_NOTE_E_3, .value = 23}},
      {.type                 = MIDI_MSG_TYPE_AFTERTOUCH_POLY,
       .channel              = 1,
       .data.aftertouch_poly = {.note = MIDI_NOTE_E_4, .value = 23}},
      {.type                 = MIDI_MSG_TYPE_AFTERTOUCH_POLY,
       .channel              = 1,
       .data.aftertouch_poly = {.note = MIDI_NOTE_E_4, .value = 24}},

      {.type                = MIDI_MSG_TYPE_CONTROL_CHANGE,
       .channel             = 7,
       .data.control_change = {.control = MIDI_CTRL_ATTACK_TIME, .value = 12}},
      {.type                = MIDI_MSG_TYPE_CONTROL_CHANGE,
       .channel             = 2,
       .data.control_change = {.control = MIDI_CTRL_ATTACK_TIME, .value = 12}},
      {.type                = MIDI_MSG_TYPE_CONTROL_CHANGE,
       .channel             = 2,
       .data.control_change = {.control = MIDI_CTRL_PAN, .value = 12}},
      {.type                = MIDI_MSG_TYPE_CONTROL_CHANGE,
       .channel             = 2,
       .data.control_change = {.control = MIDI_CTRL_PAN, .value = 14}},

      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = 2, .data.program_change = {.program_id = 27}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = 1, .data.program_change = {.program_id = 27}},
      {.type = MIDI_MSG_TYPE_PROGRAM_CHANGE, .channel = 1, .data.program_change = {.program_id = 23}},

      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .channel = 5, .data.aftertouch_mono = {.value = 88}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .channel = 8, .data.aftertouch_mono = {.value = 88}},
      {.type = MIDI_MSG_TYPE_AFTERTOUCH_MONO, .channel = 8, .data.aftertouch_mono = {.value = 99}},

      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = 10, .data.pitch_bend = {.value = 10}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = 12, .data.pitch_bend = {.value = 10}},
      {.type = MIDI_MSG_TYPE_PITCH_BEND, .channel = 12, .data.pitch_bend = {.value = 15}},

      {.type = MIDI_MSG_TYPE_TIMING_CLOCK},
      {.type = MIDI_MSG_TYPE_START},
      {.type = MIDI_MSG_TYPE_CONTINUE},
      {.type = MIDI_MSG_TYPE_STOP},
      {.type = MIDI_MSG_TYPE_ACTIVE_SENSING},
      {.type = MIDI_MSG_TYPE_SYSTEM_RESET},

      {.type               = MIDI_MSG_TYPE_MTC_QUARTER_FRAME,
       .data.quarter_frame = {.type = MIDI_QF_TYPE_HOURS_HIGH_NIBBLE, .value = 14}},
      {.type               = MIDI_MSG_TYPE_MTC_QUARTER_FRAME,
       .data.quarter_frame = {.type = MIDI_QF_TYPE_FRAME_HIGH_NIBBLE, .value = 14}},
      {.type               = MIDI_MSG_TYPE_MTC_QUARTER_FRAME,
       .data.quarter_frame = {.type = MIDI_QF_TYPE_FRAME_HIGH_NIBBLE, .value = 8}},

      {.type = MIDI_MSG_TYPE_SONG_POSITION_POINTER, .data.song_position_pointer = {.value = 4567}},
      {.type = MIDI_MSG_TYPE_SONG_POSITION_POINTER, .data.song_position_pointer = {.value = 12345}},

      {.type = MIDI_MSG_TYPE_SONG_SELECT, .data.song_select = {.value = 119}},
      {.type = MIDI_MSG_TYPE_SONG_SELECT, .data.song_select = {.value = 47}},

      {.type = MIDI_MSG_TYPE_SYSEX_START},

      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x3a, .sequence_number = 0}},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x3a, .sequence_number = 1}},
      {.type = MIDI_MSG_TYPE_NON_STD_SYSEX_BYTE, .data.sysex_byte = {.byte = 0x2c, .sequence_number = 1}},

      {.type = MIDI_MSG_TYPE_SYSEX_STOP, .data.sysex_stop = {.sequence_length = 100, .is_length_overflowed = false}},
      {.type = MIDI_MSG_TYPE_SYSEX_STOP, .data.sysex_stop = {.sequence_length = 101, .is_length_overflowed = false}},
      {.type = MIDI_MSG_TYPE_SYSEX_STOP, .data.sysex_stop = {.sequence_length = 101, .is_length_overflowed = true}},
  };

  const size_t num_msgs = sizeof(msgs) / sizeof(msgs[0]);

  for(size_t i = 0; i < num_msgs; i++) {
    MIDI_Message lhs = msgs[i];
    for(size_t j = 0; j < num_msgs; j++) {
      MIDI_Message rhs = msgs[j];

      EXPECT_EQ(&r, (i == j), MIDI_message_equals(lhs, rhs));

      if(HAS_FAILED(&r)) {
        printf("fail %zu ?= %zu\n", i, j);

        char lhs_str[1024 + 1] = {0};
        char rhs_str[1024 + 1] = {0};

        MIDI_message_to_str_buffer_short(lhs_str, 1024, lhs);
        MIDI_message_to_str_buffer_short(rhs_str, 1024, rhs);

        printf("%s %s %s\n", lhs_str, (i == j) ? "!=" : "==", rhs_str);

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