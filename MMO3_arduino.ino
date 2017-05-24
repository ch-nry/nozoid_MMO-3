// MMO-3
// based on arduino duo
//
// main file
// --------------------------------------------------------------------------
// This file is part of the MMO-3 firmware.
//
//    MMO-3 firmware is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    MMO-3 firmware is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with MMO-3 firmware. If not, see <http://www.gnu.org/licenses/>.
// --------------------------------------------------------------------------

#include <Arduino.h>
#include "ARS.h"
#include "conf.h"

#pragma GCC optimize ("-O3")

// global variables
int32_t audio_outR, audio_outL, audio_out2, audio_inR2, audio_inL, audio_inR;

// in_ADC
uint32_t adc_value16[33];

// in keyboard
uint32_t KEY_LOCAL_goal, NOTE_ON;
uint32_t KEY_LOCAL, KEY_global;
uint32_t flash_lock_bit;

// in MIDI
uint32_t MIDI_gate, MIDI_pitch;
uint32_t MIDI_fader[33];

// portamento
uint32_t portamento;

// module VCO

// module VCA
int32_t  MIX_outL, MIX_outR, VCA_outL, VCA_outR;

// module ADSR
uint32_t ADSR_out;

// module LFO
uint32_t LFO1_phase, LFO1_increment;

// module LFO2 et 3
uint32_t LFO3_mode;

// modulation
int32_t modulation_data[16]; // 
int32_t modulation_data_AM[16]; // positif only : seulement pour la modulation AM et WS
uint32_t modulation_index[16]; // pointeur vers les données des 9 faders de modulations + joystick
uint32_t modulation_type_all;

// leds 
uint32_t led1_time; // time in data loop number
uint32_t blink_time, blink_led;

// config
uint32_t GATE_mode, ADSR_mode, audio_out_mode;

//joystick
int32_t MIDI_joystick_X, MIDI_joystick_Y;

// env follower
uint32_t envelope;

// DAC
uint32_t dac_on;

void setup() {
  uint32_t i;

  REG_PMC_PCER0 = 1 << 11; // enable la clock du port PIO A pour les entree
  REG_PMC_PCER0 = 1 << 12; // enable la clock du port PIO B pour les entree
  REG_PMC_PCER0 = 1 << 13; // enable la clock du port PIO C pour les entree
  REG_PMC_PCER0 = 1 << 14; // enable la clock du port PIO D pour les entree

  REG_SUPC_SMMR = 0x0000110B; // suply monitor reset at 3V
  REG_SUPC_MR = 0xA5005A00;

  //EFC0->EEFC_FMR = 0X00000400; // mandatory to keep program speed when loading the dueFlashStorage library. go wonder why.

  init_dac();

  init_debug();
  init_led();
  init_analog_out();
  init_random();
  init_analog_in();
  init_keyboard();
  init_joystick(); 
  init_midi();
  init_save();
  
  init_VCO();
  init_LFO1();
  init_LFO2();
  init_LFO3();
  init_ADSR();
  init_VCA();
  //init_ENV();
  
  VCO1_freq();
  VCO2_freq();
  VCO3_freq();
  PORTAMENTO_update();
  VCA_update();
  
  test(); // hardware test mode
 
  start_dac();
  
  while (true) main_loop(); // faster than arduino loop
}

inline void main_loop() { // as fast as possible
  uint32_t compteur, tmpU32;
  int32_t tmp32;
  
  #ifdef syncro_out
    test2_on();
  #endif
  analog_start_1();
  
  if (flash_lock_bit == 0) 
    keyboard_in();
  else
    if (efc_perform_command_is_ready(EFC1))
      save_conf0();
        
  VCO1_freq();
  VCO2_freq();    
  VCO3_freq();    
  #ifdef syncro_out
    test2_off();
  #endif  
  analog_in();
  MIDI_in();
  LFO1_modulation();
  LFO2_modulation();
  LFO3_modulation();
  PORTAMENTO_update();
  ADSR_update();
  update_ext(); // modulation from external analog value
  // MIX_update(); // put in the analog convertion
  // VCA_update(); // idem
  // joystick();   // idem
  update_leds(); // gate and midi leds
  
  //analog_out_1((modulation_data[modulation_index[index_VCO1_MOD1]]<<16)^0x80000000);
  //analog_out_2((modulation_data_AM[modulation_index[index_VCO1_MOD1]]<<16));
}

void loop() {
  //not used
}

inline void compute_audio_sample() {
  uint32_t VCO1_out, VCO2_out, VCO3_out, VCO4_out;
  uint32_t ADSR_tmp;
  sound stereo;
  uint32_t modulation_type_local;  
  modulation_type_local = modulation_type_all;
  
  modulation_audio();
  PORTAMENTO();
  VCO1_out = VCO1(modulation_type_local); // moins de 2µs
  VCO2_out = VCO2(modulation_type_local);
  VCO3_out = VCO3(modulation_type_local);
  ADSR_tmp = ADSR();
  stereo = MIX(VCO1_out, VCO2_out, VCO3_out, ADSR_tmp); // 2.4 µs
  stereo = VCA(stereo); // 0.6µs
  LFO1_audio(); // 0.3µs
  LFO2_audio(); // 0.3µs
  LFO3_audio(); 
  //ENVELOPE();
  joystick_audio(); // 1.8µs
  audio_outR = stereo.U32[0];
  audio_outL = stereo.U32[1];
}

void SSC_Handler(void){
uint32_t status;

  #ifdef syncro_out
    test1_on();
  #endif
    
  if (!(REG_SSC_SR & SSC_IER_TXSYN)) {
    REG_SSC_THR = REG_SSC_RHR; // just to initialise properlly (not to invert R and L)
    NVIC_ClearPendingIRQ(SSC_IRQn); 
  }
  else {
    audio_inL = REG_SSC_RHR;
    REG_SSC_THR = audio_out2;
    audio_out2 = audio_outL; // Why is that mandatory to have the L and R in sync???
    
    compute_audio_sample();
    
    NVIC_ClearPendingIRQ(SSC_IRQn); // next sample is allready here, no need to go to an other interuption to get it (it save time)
    
    status = REG_SSC_SR;
    audio_inR = audio_inR2; // to get the L and R in phase
    audio_inR2 = REG_SSC_RHR;
    REG_SSC_THR = audio_outR;
  }
  #ifdef syncro_out
    test1_off();
  #endif
}

