
#ifndef C_MIDI_NOTE_H
#define C_MIDI_NOTE_H

#include <stddef.h>
#include <stdint.h>

typedef enum MIDI_Note {
  // clang-format off
  MIDI_NOTE_N1_C = 0,
  MIDI_NOTE_N1_D_B, MIDI_NOTE_N1_D,
  MIDI_NOTE_N1_E_B, MIDI_NOTE_N1_E,
  MIDI_NOTE_N1_F,
  MIDI_NOTE_N1_G_B, MIDI_NOTE_N1_G,
  MIDI_NOTE_N1_A_B, MIDI_NOTE_N1_A,
  MIDI_NOTE_N1_B_B, MIDI_NOTE_N1_B, // [ 0, 11]
  MIDI_NOTE_0_C,
  MIDI_NOTE_0_D_B, MIDI_NOTE_0_D,
  MIDI_NOTE_0_E_B, MIDI_NOTE_0_E,
  MIDI_NOTE_0_F,
  MIDI_NOTE_0_G_B, MIDI_NOTE_0_G,
  MIDI_NOTE_0_A_B, MIDI_NOTE_0_A,
  MIDI_NOTE_0_B_B, MIDI_NOTE_0_B, // [ 12, 23]
  MIDI_NOTE_1_C,
  MIDI_NOTE_1_D_B, MIDI_NOTE_1_D,
  MIDI_NOTE_1_E_B, MIDI_NOTE_1_E,
  MIDI_NOTE_1_F,
  MIDI_NOTE_1_G_B, MIDI_NOTE_1_G,
  MIDI_NOTE_1_A_B, MIDI_NOTE_1_A,
  MIDI_NOTE_1_B_B, MIDI_NOTE_1_B, // [ 24, 35]
  MIDI_NOTE_2_C,
  MIDI_NOTE_2_D_B, MIDI_NOTE_2_D,
  MIDI_NOTE_2_E_B, MIDI_NOTE_2_E,
  MIDI_NOTE_2_F,
  MIDI_NOTE_2_G_B, MIDI_NOTE_2_G,
  MIDI_NOTE_2_A_B, MIDI_NOTE_2_A,
  MIDI_NOTE_2_B_B, MIDI_NOTE_2_B, // [ 36, 47]
  MIDI_NOTE_3_C,
  MIDI_NOTE_3_D_B, MIDI_NOTE_3_D,
  MIDI_NOTE_3_E_B, MIDI_NOTE_3_E,
  MIDI_NOTE_3_F,
  MIDI_NOTE_3_G_B, MIDI_NOTE_3_G,
  MIDI_NOTE_3_A_B, MIDI_NOTE_3_A,
  MIDI_NOTE_3_B_B, MIDI_NOTE_3_B, // [ 48, 59]
  MIDI_NOTE_4_C,
  MIDI_NOTE_4_D_B, MIDI_NOTE_4_D,
  MIDI_NOTE_4_E_B, MIDI_NOTE_4_E,
  MIDI_NOTE_4_F,
  MIDI_NOTE_4_G_B, MIDI_NOTE_4_G,
  MIDI_NOTE_4_A_B, MIDI_NOTE_4_A,
  MIDI_NOTE_4_B_B, MIDI_NOTE_4_B, // [ 60, 71]
  MIDI_NOTE_5_C,
  MIDI_NOTE_5_D_B, MIDI_NOTE_5_D,
  MIDI_NOTE_5_E_B, MIDI_NOTE_5_E,
  MIDI_NOTE_5_F,
  MIDI_NOTE_5_G_B, MIDI_NOTE_5_G,
  MIDI_NOTE_5_A_B, MIDI_NOTE_5_A,
  MIDI_NOTE_5_B_B, MIDI_NOTE_5_B, // [ 72, 83]
  MIDI_NOTE_6_C,
  MIDI_NOTE_6_D_B, MIDI_NOTE_6_D,
  MIDI_NOTE_6_E_B, MIDI_NOTE_6_E,
  MIDI_NOTE_6_F,
  MIDI_NOTE_6_G_B, MIDI_NOTE_6_G,
  MIDI_NOTE_6_A_B, MIDI_NOTE_6_A,
  MIDI_NOTE_6_B_B, MIDI_NOTE_6_B, // [ 84, 95]
  MIDI_NOTE_7_C,
  MIDI_NOTE_7_D_B, MIDI_NOTE_7_D,
  MIDI_NOTE_7_E_B, MIDI_NOTE_7_E,
  MIDI_NOTE_7_F,
  MIDI_NOTE_7_G_B, MIDI_NOTE_7_G,
  MIDI_NOTE_7_A_B, MIDI_NOTE_7_A,
  MIDI_NOTE_7_B_B, MIDI_NOTE_7_B, // [ 96,107]
  MIDI_NOTE_8_C,
  MIDI_NOTE_8_D_B, MIDI_NOTE_8_D,
  MIDI_NOTE_8_E_B, MIDI_NOTE_8_E,
  MIDI_NOTE_8_F,
  MIDI_NOTE_8_G_B, MIDI_NOTE_8_G,
  MIDI_NOTE_8_A_B, MIDI_NOTE_8_A,
  MIDI_NOTE_8_B_B, MIDI_NOTE_8_B, // [108,119]
  MIDI_NOTE_9_C,
  MIDI_NOTE_9_D_B, MIDI_NOTE_9_D,
  MIDI_NOTE_9_E_B, MIDI_NOTE_9_E,
  MIDI_NOTE_9_F,
  MIDI_NOTE_9_G_B, MIDI_NOTE_9_G, // [120,127]
  // clang-format on

  MIDI_NOTE_LOWEST  = MIDI_NOTE_N1_C,
  MIDI_NOTE_HIGHEST = MIDI_NOTE_9_G,
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
