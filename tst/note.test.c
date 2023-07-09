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

  EXPECT_EQ(&r, MIDI_note_difference(MIDI_NOTE_0_A, MIDI_NOTE_0_A), 0);

  EXPECT_EQ(&r, MIDI_note_difference(MIDI_NOTE_0_C, MIDI_NOTE_1_C), 12);
  EXPECT_EQ(&r, MIDI_note_difference(MIDI_NOTE_1_C, MIDI_NOTE_2_C), 12);
  EXPECT_EQ(&r, MIDI_note_difference(MIDI_NOTE_2_C, MIDI_NOTE_3_C), 12);

  EXPECT_EQ(&r, MIDI_note_difference(MIDI_NOTE_2_C, MIDI_NOTE_2_G), 7);

  EXPECT_EQ(&r, MIDI_note_difference(MIDI_NOTE_3_A, MIDI_NOTE_3_G), -2);
  EXPECT_EQ(&r, MIDI_note_difference(MIDI_NOTE_4_A, MIDI_NOTE_3_G), -14);
  EXPECT_EQ(&r, MIDI_note_difference(MIDI_NOTE_2_A, MIDI_NOTE_3_G), 10);

  return r;
}

int main(void) {

  Test tests[] = {
      tst_size,
      tst_difference,
  };

  return (run_tests(tests, sizeof(tests) / sizeof(Test)) == PASS) ? 0 : 1;
}