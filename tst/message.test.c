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

int main(void) {
  Test tests[] = {
      tst_size,
      tst_to_string,
  };

  return (run_tests(tests, sizeof(tests) / sizeof(Test)) == PASS) ? 0 : 1;
}