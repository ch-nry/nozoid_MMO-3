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

uint32_t VCO1_phase, VCO2_phase, VCO3_phase;
uint32_t freq1_old, freq2_old, freq3_old, freq1, freq2, freq3;
uint32_t VC01_mod1_filter, VC01_mod2_filter, VC01_mod3_filter;
uint32_t VC02_mod1_filter, VC02_mod2_filter, VC02_mod3_filter;
uint32_t VC03_mod1_filter, VC03_mod2_filter, VC03_mod3_filter;
uint32_t mod_1_OSC1_hysteresys, mod_2_OSC1_hysteresys, mod_3_OSC1_hysteresys;
uint32_t mod_1_OSC2_hysteresys, mod_2_OSC2_hysteresys, mod_3_OSC2_hysteresys;
uint32_t mod_1_OSC3_hysteresys, mod_2_OSC3_hysteresys, mod_3_OSC3_hysteresys;


inline void init_VCO() {
}


inline uint32_t fast_sin(const uint32_t phase) {
  uint32_t sinus, from_table;
  int32_t diff;
  
  from_table = table_sinus_diff[phase>>19];
  sinus = (from_table & 0xFFFFF800);
  from_table <<= 21;
  diff = (int32_t)from_table >> 21; // passage en signed
  diff *= (int32_t)(((phase>>3) & 0xFFFF)>>5);
  return (sinus + diff);
}

inline uint32_t fast_cos(uint32_t const phase) {
  return(fast_sin(phase+0x40000000));
}

/*
inline uint32_t fast_cos(uint32_t const phase) {
  uint32_t phaseH, phaseL, tmp1, tmp2;
  int32_t tmp32S;
  
  // linear interpolation of a table
  #ifdef quality
    phaseH = phase>>19; // 13 bits
    phaseL = phase & 0x7FFFF; // 19 bits
    tmp1 = table_cos[phaseH];
    tmp2 = table_cos[phaseH + 1];
    tmp32S = (tmp2-tmp1); // tmp2-tmp sur 21 bits max
    tmp32S = (tmp32S >> 6) * (phaseL >> 3); // 15 + 16 + sign
    tmp32S = (tmp32S >> 10) + tmp1;
    return(tmp32S);
  #endif
  #ifndef quality
    phaseH = phase>>23; // 9 bits
    phaseL = phase & 0x7FFFFF; // 23 bits
    tmp1 = table_cos_small[phaseH];
    tmp2 = table_cos_small[(phaseH + 1)];
    tmp32S = (tmp2-tmp1); // tmp2-tmp sur 25 bits max
    tmp32S = (tmp32S >> 10) * (phaseL >> 7); // 15 + 16 + sign
    tmp32S = (tmp32S >> 6) + tmp1;
    return(tmp32S);
  #endif
}

inline uint32_t fast_sin(uint32_t const phase) {
  return(fast_cos(phase+0xC0000000));
}
*/


inline int32_t waveshaper(uint32_t const wave, uint32_t const shape) { 
  int32_t input, waveshape, tmp;
 
  input = wave^0x80000000; // switch to signed
  tmp = shape;
  tmp += 1<<26;
  tmp = (input >> 14) * ((uint32_t)tmp >> 14);
  waveshape = fast_sin(tmp);
  return(waveshape);
}

inline uint32_t freq2increment(uint32_t const freq) {
  uint32_t fq, freq_MSB, freq_LSB, increment1, increment2;
    
  fq = (freq>0X0FFFFFFF)? 0X0FFFFFFF: freq;
  //fq = clip_U32_28(freq);
  freq_MSB = fq >> 18; // keep the 1st 10 bits
  freq_LSB = fq & 0x3FFFF; // other 18 bits
  increment1 = table_CV2increment[freq_MSB];
  increment2 = table_CV2increment[freq_MSB+1];
  increment2 -= increment1;
  increment1 += ((increment2>>8)*(freq_LSB>>2))>>8;
  return(increment1<<3);
}

inline void VCO1_freq() { // in data loop
  uint32_t tmp, test;
  uint32_t modulation_type_local;
  modulation_type_local = modulation_type_all;
      
  tmp = VC01_mod1_filter;
  tmp = filter(clip_ana_low16(mod_1_OSC1_hysteresys)<<10, tmp, 9);
  VC01_mod1_filter = tmp;
  
  tmp = VC01_mod2_filter;
  tmp = filter(clip_ana_low16(mod_2_OSC1_hysteresys)<<10, tmp, 9);
  VC01_mod2_filter = tmp;
 
  tmp = VC01_mod3_filter;
  tmp = filter(clip_ana_low16(mod_3_OSC1_hysteresys)<<10, tmp, 9);
  VC01_mod3_filter = tmp;
}

inline void VCO1_freq_1() { // in analogue loop (==DATA / 30)
  uint32_t tmp;

  tmp = hysteresis16(adc_value16[VCO1_FQ], freq1_old);
  freq1_old = tmp;
  tmp *= 1536;
  tmp += (1<<26);
  freq1 = tmp;
}

inline void VCO1_freq_2() { // in analogue loop
  uint32_t tmp, mod1, mod2, mod3;

  mod1 = hysteresis16(adc_value16[VCO1_MOD1], mod_1_OSC1_hysteresys);
  mod_1_OSC1_hysteresys = mod1;
  mod2 = hysteresis16(adc_value16[VCO1_MOD2], mod_2_OSC1_hysteresys);
  mod_2_OSC1_hysteresys = mod2;
  mod3 = hysteresis16(adc_value16[VCO1_MOD3], mod_3_OSC1_hysteresys);
  mod_3_OSC1_hysteresys = mod3;
}

inline void VCO2_freq() {
  uint32_t tmp, test;
  uint32_t modulation_type_local;
  modulation_type_local = modulation_type_all;
  
  tmp = VC02_mod1_filter;
  tmp = filter(clip_ana_low16(mod_1_OSC2_hysteresys)<<10, tmp, 9);
  VC02_mod1_filter = tmp;

  tmp = VC02_mod2_filter;
  tmp = filter(clip_ana_low16(mod_2_OSC2_hysteresys)<<10, tmp, 9);
  VC02_mod2_filter = tmp;

  tmp = VC02_mod3_filter;
  tmp = filter(clip_ana_low16(mod_3_OSC2_hysteresys)<<10, tmp, 9);
  VC02_mod3_filter = tmp;
}

inline void VCO2_freq_1() {
  uint32_t tmp;
  
  tmp = hysteresis16(adc_value16[VCO2_FQ], freq2_old);
  freq2_old = tmp;
  tmp *= 1536;
  tmp += (1<<26);
  freq2 = tmp;
}
  
inline void VCO2_freq_2() {
  uint32_t tmp, mod1, mod2, mod3;
  
  mod1 = hysteresis16(adc_value16[VCO2_MOD1], mod_1_OSC2_hysteresys);
  mod_1_OSC2_hysteresys = mod1;
  mod2 = hysteresis16(adc_value16[VCO2_MOD2], mod_2_OSC2_hysteresys);
  mod_2_OSC2_hysteresys = mod2;
  mod3 = hysteresis16(adc_value16[VCO2_MOD3], mod_3_OSC2_hysteresys);
  mod_3_OSC2_hysteresys = mod3;
}
  
inline void VCO3_freq() {
  uint32_t tmp, test;
  uint32_t modulation_type_local;
  modulation_type_local = modulation_type_all;
  
  tmp = VC03_mod1_filter;
  tmp = filter(clip_ana_low16(mod_1_OSC3_hysteresys)<<10, tmp, 9);
  VC03_mod1_filter = tmp;

  tmp = VC03_mod2_filter;
  tmp = filter(clip_ana_low16(mod_2_OSC3_hysteresys)<<10, tmp, 9);
  VC03_mod2_filter = tmp;

  tmp = VC03_mod3_filter;
  tmp = filter(clip_ana_low16(mod_3_OSC3_hysteresys)<<10, tmp, 9);
  VC03_mod3_filter = tmp;
}

inline void VCO3_freq_1() {
  uint32_t tmp;
  
  tmp = hysteresis16(adc_value16[VCO3_FQ], freq3_old);
  freq3_old = tmp;
  tmp *= 1536;
  tmp += (1<<26);
  freq3 = tmp;
}
  
inline void VCO3_freq_2() {
  uint32_t tmp, mod1, mod2, mod3;
  
  mod1 = hysteresis16(adc_value16[VCO3_MOD1], mod_1_OSC3_hysteresys);
  mod_1_OSC3_hysteresys = mod1;
  mod2 = hysteresis16(adc_value16[VCO3_MOD2], mod_2_OSC3_hysteresys);
  mod_2_OSC3_hysteresys = mod2;
  mod3 = hysteresis16(adc_value16[VCO3_MOD3], mod_3_OSC3_hysteresys);
  mod_3_OSC3_hysteresys = mod3;
}

inline uint32_t VCO1(const uint32_t modulation_type_local) {
  uint32_t fq, WS, tmp, tmp2, AM;
  int32_t PM;
  uint32_t index_mod1, index_mod2, index_mod3;
  
  index_mod1 = modulation_index[index_VCO1_MOD1];
  index_mod2 = modulation_index[index_VCO1_MOD2];
  index_mod3 = modulation_index[index_VCO1_MOD3];
  
  // OSC frequence
  fq = (freq1 + KEY_LOCAL);
  WS = 0;
  AM = 0;
  PM = 0;

  switch ((modulation_type_local >> 0) & 0b11) {
    case 0: AM +=  (VC01_mod1_filter >> 12) * modulation_data_AM[index_mod1]; break;
    case 1: fq +=  (int32_t)((VC01_mod1_filter >> 12) * modulation_data[index_mod1])>>4; break;
    case 2: PM += ((VC01_mod1_filter >> 10) * (VC01_mod1_filter >> 10 ) >> 14 ) * modulation_data[index_mod1]; break;
    case 3: WS +=  (VC01_mod1_filter >> 12) * (modulation_data_AM[index_mod1]); break;
  }
  switch ((modulation_type_local >> 2) & 0b11) {
    case 0: AM +=  (VC01_mod2_filter >> 12) * modulation_data_AM[index_mod2]; break;
    case 1: fq +=  (int32_t)((VC01_mod2_filter >> 12) * modulation_data[index_mod2])>>4; break;
    case 2: PM += ((VC01_mod2_filter >> 10) * (VC01_mod2_filter >> 10 ) >> 14 ) * modulation_data[index_mod2]; break;
    case 3: WS +=  (VC01_mod2_filter >> 12) * (modulation_data_AM[index_mod2]); break;
  }
  switch ((modulation_type_local >> 4) & 0b11) {
    case 0: AM +=  (VC01_mod3_filter >> 12) * modulation_data_AM[index_mod3]; break;
    case 1: fq +=  (int32_t)((VC01_mod3_filter >> 12) * modulation_data[index_mod3])>>4; break;
    case 2: PM += ((VC01_mod3_filter >> 10) * (VC01_mod3_filter >> 10 ) >> 14 ) * modulation_data[index_mod3]; break;
    case 3: WS +=  (VC01_mod3_filter >> 12) * (modulation_data_AM[index_mod3]); break;
  }
  
  VCO1_phase += freq2increment(fq);
  tmp = waveshaper(fast_cos(VCO1_phase + PM), WS);
  AM = 0X3FFFFFFF - min(AM, 0X3FFFFFFF);
  tmp = (tmp >> 16) * (AM >> 14);
  
  //modulation
  tmp2 = tmp>>16; // keep only 16bit
  modulation_data_AM[mod_VCO1] = tmp2;
  tmp2 -= (1<<15); // switch to signed
  modulation_data[mod_VCO1] = tmp2;
  return(tmp);
}

inline uint32_t VCO2(const uint32_t modulation_type_local) {
  uint32_t fq, WS, tmp, tmp2, AM;
  int32_t PM;
  uint32_t index_mod1, index_mod2, index_mod3;
  
  index_mod1 = modulation_index[index_VCO2_MOD1];
  index_mod2 = modulation_index[index_VCO2_MOD2];
  index_mod3 = modulation_index[index_VCO2_MOD3];
  
  // OSC frequence
  fq = freq2 + KEY_LOCAL;
  WS = 0;
  AM = 0;
  PM = 0;

  switch ((modulation_type_local >> 6) & 0b11) {
    case 0: AM +=  (VC02_mod1_filter >> 12) * modulation_data_AM[index_mod1]; break;
    case 1: fq +=  (int32_t)((VC02_mod1_filter >> 12) * modulation_data[index_mod1])>>4; break;
    case 2: PM += ((VC02_mod1_filter >> 10) * (VC02_mod1_filter >> 10 ) >> 14 ) * modulation_data[index_mod1]; break;
    case 3: WS +=  (VC02_mod1_filter >> 12) * (modulation_data_AM[index_mod1]); break;
  }
  switch ((modulation_type_local >> 8) & 0b11) {
    case 0: AM +=  (VC02_mod2_filter >> 12) * modulation_data_AM[index_mod2]; break;
    case 1: fq +=  (int32_t)((VC02_mod2_filter >> 12) * modulation_data[index_mod2])>>4; break;
    case 2: PM += ((VC02_mod2_filter >> 10) * (VC02_mod2_filter >> 10 ) >> 14 ) * modulation_data[index_mod2]; break;
    case 3: WS +=  (VC02_mod2_filter >> 12) * (modulation_data_AM[index_mod2]); break;
  }
  switch ((modulation_type_local >> 10) & 0b11) {
    case 0: AM +=  (VC02_mod3_filter >> 12) * modulation_data_AM[index_mod3]; break;
    case 1: fq +=  (int32_t)((VC02_mod3_filter >> 12) * modulation_data[index_mod3])>>4; break;
    case 2: PM += ((VC02_mod3_filter >> 10) * (VC02_mod3_filter >> 10 ) >> 14 ) * modulation_data[index_mod3]; break;
    case 3: WS +=  (VC02_mod3_filter >> 12) * (modulation_data_AM[index_mod3]); break;
  }
  
  VCO2_phase += freq2increment(fq);
  tmp = waveshaper(fast_cos(VCO2_phase + PM), WS);
  AM = 0X3FFFFFFF - min(AM, 0X3FFFFFFF);
  tmp = (tmp >> 16) * (AM >> 14);
  
  //modulation
  tmp2 = tmp>>16; // keep only 16bit
  modulation_data_AM[mod_VCO2] = tmp2;
  tmp2 -= (1<<15); // switch to signed
  modulation_data[mod_VCO2] = tmp2;

  return(tmp);
}

inline uint32_t VCO3(const uint32_t modulation_type_local) {
  uint32_t fq, WS, tmp, tmp2, AM;
  int32_t PM;
  uint32_t index_mod1, index_mod2, index_mod3;
  
  index_mod1 = modulation_index[index_VCO3_MOD1];
  index_mod2 = modulation_index[index_VCO3_MOD2];
  index_mod3 = modulation_index[index_VCO3_MOD3];
  
  // OSC frequence
  fq = freq3 + KEY_LOCAL;
  WS = 0;
  AM = 0;
  PM = 0;

  switch ((modulation_type_local >> 12) & 0b11) {
    case 0: AM +=  (VC03_mod1_filter >> 12) * modulation_data_AM[index_mod1]; break;
    case 1: fq +=  (int32_t)((VC03_mod1_filter >> 12) * modulation_data[index_mod1])>>4; break;
    case 2: PM += ((VC03_mod1_filter >> 10) * (VC03_mod1_filter >> 10 ) >> 14 ) * modulation_data[index_mod1]; break;
    case 3: WS +=  (VC03_mod1_filter >> 12) * (modulation_data_AM[index_mod1]); break;
  }
  switch ((modulation_type_local >> 14) & 0b11) {
    case 0: AM +=  (VC03_mod2_filter >> 12) * modulation_data_AM[index_mod2]; break;
    case 1: fq +=  (int32_t)((VC03_mod2_filter >> 12) * modulation_data[index_mod2])>>4; break;
    case 2: PM += ((VC03_mod2_filter >> 10) * (VC03_mod2_filter >> 10 ) >> 14 ) * modulation_data[index_mod2]; break;
    case 3: WS +=  (VC03_mod2_filter >> 12) * (modulation_data_AM[index_mod2]); break;
  }
  switch ((modulation_type_local >> 16) & 0b11) {
    case 0: AM +=  (VC03_mod3_filter >> 12) * modulation_data_AM[index_mod3]; break;
    case 1: fq +=  (int32_t)((VC03_mod3_filter >> 12) * modulation_data[index_mod3])>>4; break;
    case 2: PM += ((VC03_mod3_filter >> 10) * (VC03_mod3_filter >> 10 ) >> 14 ) * modulation_data[index_mod3]; break;
    case 3: WS +=  (VC03_mod3_filter >> 12) * (modulation_data_AM[index_mod3]); break;
  }
  
  VCO3_phase += freq2increment(fq);
  tmp = waveshaper(fast_cos(VCO3_phase + PM), WS);
  AM = 0X3FFFFFFF - min(AM, 0X3FFFFFFF);
  tmp = (tmp >> 16) * (AM >> 14);
  
  //modulation
  tmp2 = tmp>>16; // keep only 16bit
  modulation_data_AM[mod_VCO3] = tmp2;
  tmp2 -= (1<<15); // switch to signed
  modulation_data[mod_VCO3] = tmp2;

  return(tmp);
}
