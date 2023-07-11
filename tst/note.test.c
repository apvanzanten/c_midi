#include <cfac/test_utils.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define OK STAT_OK

#include "note.h"

static Result tst_size(void) {
  Result r = PASS;

  EXPECT_EQ(&r, sizeof(MIDI_Note), 4);

  return r;
}

static Result tst_difference(void) {
  Result r = PASS;

  EXPECT_EQ(&r, MIDI_note_difference(MIDI_NOTE_A_0, MIDI_NOTE_A_0), 0);

  EXPECT_EQ(&r, MIDI_note_difference(MIDI_NOTE_C_0, MIDI_NOTE_C_1), 12);
  EXPECT_EQ(&r, MIDI_note_difference(MIDI_NOTE_C_1, MIDI_NOTE_C_2), 12);
  EXPECT_EQ(&r, MIDI_note_difference(MIDI_NOTE_C_2, MIDI_NOTE_C_3), 12);

  EXPECT_EQ(&r, MIDI_note_difference(MIDI_NOTE_C_2, MIDI_NOTE_G_2), 7);

  EXPECT_EQ(&r, MIDI_note_difference(MIDI_NOTE_A_3, MIDI_NOTE_G_3), -2);
  EXPECT_EQ(&r, MIDI_note_difference(MIDI_NOTE_A_4, MIDI_NOTE_G_3), -14);
  EXPECT_EQ(&r, MIDI_note_difference(MIDI_NOTE_A_2, MIDI_NOTE_G_3), 10);

  return r;
}

static Result tst_to_string(void) {
  Result r = PASS;

  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "A4";
    EXPECT_EQ(&r, strlen(expect_str), MIDI_note_to_str_buffer(str, 1024, MIDI_NOTE_A_4));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "D3";
    EXPECT_EQ(&r, strlen(expect_str), MIDI_note_to_str_buffer(str, 1024, MIDI_NOTE_D_3));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "Gb0";
    EXPECT_EQ(&r, strlen(expect_str), MIDI_note_to_str_buffer(str, 1024, MIDI_NOTE_G_B_0));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }
  {
    char       str[1024 + 1] = {0};
    const char expect_str[]  = "C-1";
    EXPECT_EQ(&r, strlen(expect_str), MIDI_note_to_str_buffer(str, 1024, MIDI_NOTE_C_N1));
    EXPECT_EQ(&r, strlen(expect_str), strlen(str));
    EXPECT_STREQ(&r, expect_str, str);
  }

  return r;
}

int main(void) {

  Test tests[] = {
      tst_size,
      tst_difference,
      tst_to_string,
  };

  return (run_tests(tests, sizeof(tests) / sizeof(Test)) == PASS) ? 0 : 1;
}