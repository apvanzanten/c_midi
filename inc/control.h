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

#ifndef MIDI_CONTROL_H
#define MIDI_CONTROL_H

#include <stdint.h>

typedef enum MIDI_Control {
  MIDI_CTRL_BANK_SELECT = 0,
  MIDI_CTRL_MOD_WHEEL,
  MIDI_CTRL_BREATH_CONTROL,

  MIDI_CTRL_UNDEFINED3,

  MIDI_CTRL_FOOT_PEDAL = 4,
  MIDI_CTRL_PORTAMENTO,
  MIDI_CTRL_DATA_ENTRY,
  MIDI_CTRL_VOLUME,
  MIDI_CTRL_BALANCE,
  MIDI_CTRL_UNDEFINED9,
  MIDI_CTRL_PAN,
  MIDI_CTRL_EXPRESSION,
  MIDI_CTRL_EFFECT1,
  MIDI_CTRL_EFFECT2,

  MIDI_CTRL_UNDEFINED14,
  MIDI_CTRL_UNDEFINED15,

  MIDI_CTRL_GENERAL_A = 16,
  MIDI_CTRL_GENERAL_B,
  MIDI_CTRL_GENERAL_C,
  MIDI_CTRL_GENERAL_D,

  MIDI_CTRL_UNDEFINED20,
  MIDI_CTRL_UNDEFINED21,
  MIDI_CTRL_UNDEFINED22,
  MIDI_CTRL_UNDEFINED23,
  MIDI_CTRL_UNDEFINED24,
  MIDI_CTRL_UNDEFINED25,
  MIDI_CTRL_UNDEFINED26,
  MIDI_CTRL_UNDEFINED27,
  MIDI_CTRL_UNDEFINED28,
  MIDI_CTRL_UNDEFINED29,
  MIDI_CTRL_UNDEFINED30,
  MIDI_CTRL_UNDEFINED31,

  MIDI_CTRL_BANK_SELECT_LSB = 32,
  MIDI_CTRL_MOD_WHEEL_LSB,
  MIDI_CTRL_BREATH_CONTROL_LSB,

  MIDI_CTRL_UNDEFINED35,

  MIDI_CTRL_FOOT_PEDAL_LSB = 36,
  MIDI_CTRL_PORTAMENTO_LSB,
  MIDI_CTRL_DATA_ENTRY_LSB,
  MIDI_CTRL_VOLUME_LSB,
  MIDI_CTRL_BALANCE_LSB,

  MIDI_CTRL_UNDEFINED41, // OFFICIALLY LSB OF UNDEFINED3

  MIDI_CTRL_PAN_LSB = 42,
  MIDI_CTRL_EXPRESSION_LSB,
  MIDI_CTRL_EFFECT1_LSB,
  MIDI_CTRL_EFFECT2_LSB,

  MIDI_CTRL_UNDEFINED46, // OFFICALLY LSB OF UNDEFINED14
  MIDI_CTRL_UNDEFINED47, // OFFICALLY LSB OF UNDEFINED15

  MIDI_CTRL_GENERAL_A_LSB = 48,
  MIDI_CTRL_GENERAL_B_LSB,
  MIDI_CTRL_GENERAL_C_LSB,
  MIDI_CTRL_GENERAL_D_LSB,

  MIDI_CTRL_UNDEFINED52, // OFFICIALLY LSB OF UNDEFINED20.
  MIDI_CTRL_UNDEFINED53, // OFFICIALLY LSB OF UNDEFINED21.
  MIDI_CTRL_UNDEFINED54, // OFFICIALLY LSB OF UNDEFINED22.
  MIDI_CTRL_UNDEFINED55, // OFFICIALLY LSB OF UNDEFINED23.
  MIDI_CTRL_UNDEFINED56, // OFFICIALLY LSB OF UNDEFINED24.
  MIDI_CTRL_UNDEFINED57, // OFFICIALLY LSB OF UNDEFINED25.
  MIDI_CTRL_UNDEFINED58, // OFFICIALLY LSB OF UNDEFINED26.
  MIDI_CTRL_UNDEFINED59, // OFFICIALLY LSB OF UNDEFINED27.
  MIDI_CTRL_UNDEFINED60, // OFFICIALLY LSB OF UNDEFINED28.
  MIDI_CTRL_UNDEFINED61, // OFFICIALLY LSB OF UNDEFINED29.
  MIDI_CTRL_UNDEFINED62, // OFFICIALLY LSB OF UNDEFINED30.
  MIDI_CTRL_UNDEFINED63, // OFFICIALLY LSB OF UNDEFINED31.

  MIDI_CTRL_DAMPER_PEDAL_ON_OFF = 64,
  MIDI_CTRL_PORTAMENTO_ON_OFF,
  MIDI_CTRL_SOSTENUTO_PEDAL_ON_OFF,
  MIDI_CTRL_SOFT_PEDAL_ON_OFF,
  MIDI_CTRL_LEGATO_ON_OFF,
  MIDI_CTRL_HOLD_ON_OFF,

  MIDI_CTRL_SOUND_VARIATION = 70, // AKA SOUND CONTROLLER 1.
  MIDI_CTRL_RESONANCE,            // AKA SOUND CONTROLLER 2.
  MIDI_CTRL_RELEASE_TIME,         // AKA SOUND CONTROLLER 3.
  MIDI_CTRL_ATTACK_TIME,          // AKA SOUND CONTROLLER 4.
  MIDI_CTRL_CUTOFF_FREQUENCY,     // AKA SOUND CONTROLLER 5.
  MIDI_CTRL_SOUND_CONTROLLER6,
  MIDI_CTRL_SOUND_CONTROLLER7,
  MIDI_CTRL_SOUND_CONTROLLER8,
  MIDI_CTRL_SOUND_CONTROLLER9,
  MIDI_CTRL_SOUND_CONTROLLER10,

  MIDI_CTRL_GENERAL_E = 80,
  MIDI_CTRL_GENERAL_F,
  MIDI_CTRL_GENERAL_G,
  MIDI_CTRL_GENERAL_H,
  MIDI_CTRL_PORTAMENTO_ALT,

  MIDI_CTRL_UNDEFINED85,
  MIDI_CTRL_UNDEFINED86,
  MIDI_CTRL_UNDEFINED87,

  MIDI_CTRL_VELOCITY_PREFIX = 88,

  MIDI_CTRL_UNDEFINED89,
  MIDI_CTRL_UNDEFINED90,

  MIDI_CTRL_EFFECT3 = 91, // AKA EFFECT DEPTH 1
  MIDI_CTRL_EFFECT4,      // AKA EFFECT DEPTH 2
  MIDI_CTRL_EFFECT5,      // AKA EFFECT DEPTH 3
  MIDI_CTRL_EFFECT6,      // AKA EFFECT DEPTH 4
  MIDI_CTRL_EFFECT7,      // AKA EFFECT DEPTH 5

  MIDI_CTRL_DATA_INCREMENT,                  // RELEVANT FOR RPN/NRPN
  MIDI_CTRL_DATA_DECREMENT,                  // RELEVANT FOR RPN/NRPN
  MIDI_CTRL_NON_REGISTERED_PARAM_NUMBER_LSB, // RELEVANT FOR RPN/NRPN
  MIDI_CTRL_NON_REGISTERED_PARAM_NUMBER_MSB, // RELEVANT FOR RPN/NRPN
  MIDI_CTRL_REGISTERED_PARAM_NUMBER_LSB,     // RELEVANT FOR RPN/NRPN
  MIDI_CTRL_REGISTERED_PARAM_NUMBER_MSB,     // RELEVANT FOR RPN/NRPN

  MIDI_CTRL_UNDEFINED102,
  MIDI_CTRL_UNDEFINED103,
  MIDI_CTRL_UNDEFINED104,
  MIDI_CTRL_UNDEFINED105,
  MIDI_CTRL_UNDEFINED106,
  MIDI_CTRL_UNDEFINED107,
  MIDI_CTRL_UNDEFINED108,
  MIDI_CTRL_UNDEFINED109,
  MIDI_CTRL_UNDEFINED110,
  MIDI_CTRL_UNDEFINED111,
  MIDI_CTRL_UNDEFINED112,
  MIDI_CTRL_UNDEFINED113,
  MIDI_CTRL_UNDEFINED114,
  MIDI_CTRL_UNDEFINED115,
  MIDI_CTRL_UNDEFINED116,
  MIDI_CTRL_UNDEFINED117,
  MIDI_CTRL_UNDEFINED118,
  MIDI_CTRL_UNDEFINED119,

  MIDI_CTRL_ALL_SOUND_OFF = 120,   // CHANNEL MODE MESSAGE
  MIDI_CTRL_RESET_ALL_CONTROLLERS, // CHANNEL MODE MESSAGE
  MIDI_CTRL_LOCAL_ON_OFF,          // CHANNEL MODE MESSAGE
  MIDI_CTRL_ALL_NOTES_OFF,         // CHANNEL MODE MESSAGE
  MIDI_CTRL_OMNI_MODE_ON,          // CHANNEL MODE MESSAGE
  MIDI_CTRL_OMNI_MODE_OFF,         // CHANNEL MODE MESSAGE
  MIDI_CTRL_MONO_MODE,             // CHANNEL MODE MESSAGE
  MIDI_CTRL_POLY_MODE              // CHANNEL MODE MESSAGE
} MIDI_Control;

static inline MIDI_Control MIDI_u8_to_ctrl(uint8_t u) { return (MIDI_Control)(u); }
static inline uint8_t      MIDI_ctrl_to_u8(MIDI_Control ctrl) { return (uint8_t)(ctrl); }

static inline const char * MIDI_ctrl_to_str(MIDI_Control ctrl) {
  switch(ctrl) {
  case MIDI_CTRL_BANK_SELECT: return "BANK_SELECT";
  case MIDI_CTRL_MOD_WHEEL: return "MOD_WHEEL";
  case MIDI_CTRL_PORTAMENTO: return "PORTAMENTO";
  case MIDI_CTRL_VOLUME: return "VOLUME";
  case MIDI_CTRL_PAN: return "PAN";
  case MIDI_CTRL_EXPRESSION: return "EXPRESSION";
  case MIDI_CTRL_EFFECT1: return "EFFECT1";
  case MIDI_CTRL_EFFECT2: return "EFFECT2";
  case MIDI_CTRL_EFFECT3: return "EFFECT3";
  case MIDI_CTRL_EFFECT4: return "EFFECT4";
  case MIDI_CTRL_EFFECT5: return "EFFECT5";
  case MIDI_CTRL_EFFECT6: return "EFFECT6";
  case MIDI_CTRL_EFFECT7: return "EFFECT7";
  case MIDI_CTRL_SOUND_VARIATION: return "SOUND_VARIATION";
  case MIDI_CTRL_RESONANCE: return "RESONANCE";
  case MIDI_CTRL_RELEASE_TIME: return "RELEASE_TIME";
  case MIDI_CTRL_ATTACK_TIME: return "ATTACK_TIME";
  case MIDI_CTRL_CUTOFF_FREQUENCY: return "CUTOFF_FREQUENCY";
  default: return "OTHER";
  }
}

#endif