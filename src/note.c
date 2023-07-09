#include "note.h"
#include <stdio.h>

int MIDI_note_to_str_buffer(char * str, size_t max_len, MIDI_Note n) {
  if(str == NULL) return 0;
  if(max_len == 0) return 0;

  return snprintf(str, max_len, "%s%d", MIDI_note_get_note_only_str(n), MIDI_note_get_octave(n));
}