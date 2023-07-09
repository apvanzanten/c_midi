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

int main(void) {

  Test tests[] = {
      tst_size,
  };

  return (run_tests(tests, sizeof(tests) / sizeof(Test)) == PASS) ? 0 : 1;
}