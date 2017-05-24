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
// LFO1
//----------------------------------------------------------------------------------
int32_t LFO1_out, LFO1_filter, symetrie_LFO1, gain_LFO1, offset_signed_LFO1, distortion_LFO1;
int32_t LFO1MOD_increment;

inline void init_LFO1() {
  LFO1_phase = 0x80000000;
}

inline void LFO1_freq() { // TODO latter : tester ce qui est le plus rapide : LFO1 ou LFO3
  uint32_t tmp, freq, symetrie, distortion, gain, offset_gain;
  int32_t offset_signed;
  
  freq = (adc_value16[LFO1_FQ] >> 1) + 350;
  LFO1_increment = ((freq * freq) >> 7);
  
  symetrie = ((0xFFFF)-(adc_value16[LFO1_SYM]))<<16; // 32 bits 
  symetrie = (0xFF000000 > symetrie)? symetrie: 0xFF000000 ; 
  symetrie = (symetrie > 0x00F00000)? symetrie: 0x00F00000 ;
  symetrie_LFO1 = symetrie;
  
  tmp = adc_value16[LFO1_WF];
  distortion = ( tmp & 0x8000)? 0x7FFF : tmp; // only half of the fader
  gain = tmp - distortion; // 2nd half of the fader
  offset_gain = gain; // 15 bits
  gain *= gain; // 30 bits max
  gain >>= 15;
  gain *= offset_gain;
  gain_LFO1 = gain;
  distortion_LFO1 = distortion;

  offset_signed = 0x7FFF - (symetrie>>16); // from -1 to 1; 15 bit + sign
  offset_signed *= offset_gain; // 15 + 15 + sign
  offset_signed >>= 15; 
  offset_signed_LFO1 = offset_signed;
}

inline void LFO1_modulation() {
  uint32_t tmp, phase, symetrie, distortion, gain, offset, tmp2;
  int32_t offset_signed, tmpS, tmpS2;
  
  phase = LFO1_phase; // 32 bits
  
  symetrie = symetrie_LFO1;
  gain =  gain_LFO1;
  offset_signed = offset_signed_LFO1;
  distortion = distortion_LFO1;

  // calcul de la symetrie
  tmp = (phase > (symetrie))? -phase / (-symetrie >> 16): phase / (symetrie >> 16); 
  // gain pour passage sin -> square
  tmpS = tmp - (1<<15) + offset_signed; // passage en signed
  tmpS *= (1 << 10) + (gain >> 16);
  tmpS >>= 10;
  tmpS = (tmpS < -0x7FFF)? -0x7FFF: tmpS; // clip low
  tmpS = (tmpS >  0x7F00)?  0x7F00: tmpS; // clip high
  
  // calcul du sinus
  tmpS2 = (2*tmpS) - ((tmpS*abs(tmpS)) >> 15);
  //mix tri -> sinus
  tmpS = MIX16U(tmpS, tmpS2, distortion*2); // 15 bit + sign
  
  //modulation_data[mod_LFO1] = tmpS;
  //modulation_data_AM[mod_LFO1] = tmpS + (1<<15);
  // compute linear interpolation beween curent value and computed value
  tmpS2 = modulation_data[mod_LFO1];
  LFO1MOD_increment = (tmpS - tmpS2 )/8;
  
  #ifdef LFO_out
    analog_out_1((tmpS+(1<<15))<<16);
  #endif
}

inline void LFO1_audio() {
  int32_t tmp;
  LFO1_phase += LFO1_increment;
  tmp = modulation_data[mod_LFO1] + LFO1MOD_increment;
  modulation_data[mod_LFO1] = tmp;  // linear interpolation to smooth the data
  modulation_data_AM[mod_LFO1] = tmp + (1<<15);  
}
