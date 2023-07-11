
#ifndef C_MIDI_NOTE_H
#define C_MIDI_NOTE_H

#include <stddef.h>
#include <stdint.h>

typedef enum MIDI_Note {
  // clang-format off
  MIDI_NOTE_C_N1 = 0,
  MIDI_NOTE_D_B_N1, MIDI_NOTE_D_N1,
  MIDI_NOTE_E_B_N1, MIDI_NOTE_E_N1,
  MIDI_NOTE_F_N1,
  MIDI_NOTE_G_B_N1, MIDI_NOTE_G_N1,
  MIDI_NOTE_A_B_N1, MIDI_NOTE_A_N1,
  MIDI_NOTE_B_B_N1, MIDI_NOTE_B_N1, // [ 0, 11]
  MIDI_NOTE_C_0,
  MIDI_NOTE_D_B_0, MIDI_NOTE_D_0,
  MIDI_NOTE_E_B_0, MIDI_NOTE_E_0,
  MIDI_NOTE_F_0,
  MIDI_NOTE_G_B_0, MIDI_NOTE_G_0,
  MIDI_NOTE_A_B_0, MIDI_NOTE_A_0,
  MIDI_NOTE_B_B_0, MIDI_NOTE_B_0, // [ 12, 23]
  MIDI_NOTE_C_1,
  MIDI_NOTE_D_B_1, MIDI_NOTE_D_1,
  MIDI_NOTE_E_B_1, MIDI_NOTE_E_1,
  MIDI_NOTE_F_1,
  MIDI_NOTE_G_B_1, MIDI_NOTE_G_1,
  MIDI_NOTE_A_B_1, MIDI_NOTE_A_1,
  MIDI_NOTE_B_B_1, MIDI_NOTE_B_1, // [ 24, 35]
  MIDI_NOTE_C_2,
  MIDI_NOTE_D_B_2, MIDI_NOTE_D_2,
  MIDI_NOTE_E_B_2, MIDI_NOTE_E_2,
  MIDI_NOTE_F_2,
  MIDI_NOTE_G_B_2, MIDI_NOTE_G_2,
  MIDI_NOTE_A_B_2, MIDI_NOTE_A_2,
  MIDI_NOTE_B_B_2, MIDI_NOTE_B_2, // [ 36, 47]
  MIDI_NOTE_C_3,
  MIDI_NOTE_D_B_3, MIDI_NOTE_D_3,
  MIDI_NOTE_E_B_3, MIDI_NOTE_E_3,
  MIDI_NOTE_F_3,
  MIDI_NOTE_G_B_3, MIDI_NOTE_G_3,
  MIDI_NOTE_A_B_3, MIDI_NOTE_A_3,
  MIDI_NOTE_B_B_3, MIDI_NOTE_B_3, // [ 48, 59]
  MIDI_NOTE_C_4,
  MIDI_NOTE_D_B_4, MIDI_NOTE_D_4,
  MIDI_NOTE_E_B_4, MIDI_NOTE_E_4,
  MIDI_NOTE_F_4,
  MIDI_NOTE_G_B_4, MIDI_NOTE_G_4,
  MIDI_NOTE_A_B_4, MIDI_NOTE_A_4,
  MIDI_NOTE_B_B_4, MIDI_NOTE_B_4, // [ 60, 71]
  MIDI_NOTE_C_5,
  MIDI_NOTE_D_B_5, MIDI_NOTE_D_5,
  MIDI_NOTE_E_B_5, MIDI_NOTE_E_5,
  MIDI_NOTE_F_5,
  MIDI_NOTE_G_B_5, MIDI_NOTE_G_5,
  MIDI_NOTE_A_B_5, MIDI_NOTE_A_5,
  MIDI_NOTE_B_B_5, MIDI_NOTE_B_5, // [ 72, 83]
  MIDI_NOTE_C_6,
  MIDI_NOTE_D_B_6, MIDI_NOTE_D_6,
  MIDI_NOTE_E_B_6, MIDI_NOTE_E_6,
  MIDI_NOTE_F_6,
  MIDI_NOTE_G_B_6, MIDI_NOTE_G_6,
  MIDI_NOTE_A_B_6, MIDI_NOTE_A_6,
  MIDI_NOTE_B_B_6, MIDI_NOTE_B_6, // [ 84, 95]
  MIDI_NOTE_C_7,
  MIDI_NOTE_D_B_7, MIDI_NOTE_D_7,
  MIDI_NOTE_E_B_7, MIDI_NOTE_E_7,
  MIDI_NOTE_F_7,
  MIDI_NOTE_G_B_7, MIDI_NOTE_G_7,
  MIDI_NOTE_A_B_7, MIDI_NOTE_A_7,
  MIDI_NOTE_B_B_7, MIDI_NOTE_B_7, // [ 96,107]
  MIDI_NOTE_C_8,
  MIDI_NOTE_D_B_8, MIDI_NOTE_D_8,
  MIDI_NOTE_E_B_8, MIDI_NOTE_E_8,
  MIDI_NOTE_F_8,
  MIDI_NOTE_G_B_8, MIDI_NOTE_G_8,
  MIDI_NOTE_A_B_8, MIDI_NOTE_A_8,
  MIDI_NOTE_B_B_8, MIDI_NOTE_B_8, // [108,119]
  MIDI_NOTE_C_9,
  MIDI_NOTE_D_B_9, MIDI_NOTE_D_9,
  MIDI_NOTE_E_B_9, MIDI_NOTE_E_9,
  MIDI_NOTE_F_9,
  MIDI_NOTE_G_B_9, MIDI_NOTE_G_9, // [120,127]
  // clang-format on

  MIDI_NOTE_LOWEST  = MIDI_NOTE_C_N1,
  MIDI_NOTE_HIGHEST = MIDI_NOTE_G_9,
  MIDI_NOTE_BEGIN   = MIDI_NOTE_LOWEST,
  MIDI_NOTE_END     = MIDI_NOTE_HIGHEST + 1,
} MIDI_Note;

static inline uint8_t MIDI_note_to_uint8(MIDI_Note n) { return (uint8_t)n; }
static inline uint8_t MIDI_uint8_to_note(uint8_t u) { return (MIDI_Note)u; }
static inline int8_t  MIDI_note_difference(MIDI_Note a, MIDI_Note b) {
  return MIDI_note_to_uint8(b) - MIDI_note_to_uint8(a);
}

static inline int8_t MIDI_note_get_octave(MIDI_Note n) { return ((int8_t)(n) / 12) - 1; }

static inline uint8_t MIDI_note_get_note_only(MIDI_Note n) { return ((uint8_t)(n) % 12); }

static inline const char * MIDI_note_get_note_only_str(MIDI_Note n) {
  switch(MIDI_note_get_note_only(n)) {
  case 0: return "C";
  case 1: return "Db";
  case 2: return "D";
  case 3: return "Eb";
  case 4: return "E";
  case 5: return "F";
  case 6: return "Gb";
  case 7: return "G";
  case 8: return "Ab";
  case 9: return "A";
  case 10: return "Bb";
  case 11: return "B";
  default: return "??";
  }
}

int MIDI_note_to_str_buffer(char * str, size_t max_len, MIDI_Note n);

#endif
