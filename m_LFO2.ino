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

//----------------------------------------------------------------------------------
//  LFO2
//----------------------------------------------------------------------------------
int32_t LFO2_out, LFO2_filter;
int32_t LFO2MOD_increment;
uint32_t LFO2_phase, LFO2_increment;
uint32_t LFO2b_phase, LFO2b_increment;


inline void init_LFO2() {
  LFO2_phase = 0;
  LFO2_increment = 0;
  LFO2b_phase = 0;
  LFO2b_increment = 0;
}

inline void LFO2_modulation() {
  uint32_t freq, sin1, sin2, tmp, phase, gain, mix, fader3;
  int32_t all, tmpS, tmpS2, phaseS;

  freq = (adc_value16[LFO2_1] >> 1) + 1111;
  LFO2_increment = ((freq * ((freq * freq) >> 16)) >> 6);
  
  freq = (adc_value16[LFO2_2] >> 1) + 1111;
  LFO2b_increment = ((freq * ((freq * freq) >> 16)) >> 6);
  
  fader3 = adc_value16[LFO2_3];
  mix = min(fader3, 0x7FFF); // 15 bit
  gain = fader3 - mix; // 15 bit
  gain *= gain;
  gain >>= 15;

  phaseS = LFO2b_phase; // - (1<<15); // passage en signed
  phaseS = -(1<<15) + (abs(phaseS)>>15); // triangle
  
  phaseS *= gain + (1<<12);
  phaseS = max(min((1<<27)-1, phaseS),1-(1<<27)); // clip

  phaseS <<= 3;
  phase = phaseS+0x40000000; // unsigned

  sin1 = fast_sin(LFO2_phase);
  sin2 = fast_cos(phase);  
  sin2 >>= 15;
  
  sin2 *= mix;
  sin2 >>= 16;    
  sin1 = (sin1 >> 16) * (0xFFFF-sin2);
  all = sin1>>16;
  
  //modulation_data[mod_CVG] = all - (1<<15);
  //modulation_data_AM[mod_CVG] = all;
  LFO2MOD_increment = (all - modulation_data_AM[mod_LFO2])/8; 
  //analog_out_1(all<<16);
  #ifdef LFO_out
    analog_out_2(all<<16);
  #endif
}

inline void LFO2_audio() {
  int32_t tmp;
  LFO2_phase += LFO2_increment;
  LFO2b_phase += LFO2b_increment;
  
  tmp = modulation_data_AM[mod_LFO2] + LFO2MOD_increment;
  modulation_data[mod_LFO2] = tmp  - (1<<15);  // linear interpolation to smooth the data
  modulation_data_AM[mod_LFO2] = tmp;
}
