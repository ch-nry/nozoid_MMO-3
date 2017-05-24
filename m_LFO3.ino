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
//  LFO3
//----------------------------------------------------------------------------------
int32_t LFO3_out, LFO3_filter;
int32_t LFO3MOD_increment;
uint32_t LFO3_phase, LFO3_increment;
uint32_t LFO3b_phase, LFO3b_increment;


inline void init_LFO3() {
  LFO3_phase = 0;
  LFO3_increment = 0;
  LFO3b_phase = 0;
  LFO3b_increment = 0;
}

inline void LFO3_modulation() {
  uint32_t freq, sin1, sin2, tmp, phase, gain, mix, fader3;
  int32_t all, tmpS, tmpS2, phaseS;
  uint32_t symetrie, distortion, offset_gain, offset, tmp2;
  int32_t offset_signed, freqS, sin2S;

  switch(LFO3_mode) { 
  case 0: // PARAMETRIC
    freq = (adc_value16[LFO3_1] >> 1) + 350;
    LFO3_increment = ((freq * freq) >> 7);
    
    symetrie = ((0xFFFF)-(adc_value16[LFO3_3]))<<16; // 32 bits 
    symetrie = (0xFF000000 > symetrie)? symetrie: 0xFF000000 ; 
    symetrie = (symetrie > 0x00F00000)? symetrie: 0x00F00000 ;
    
    tmp = adc_value16[LFO3_2];
    distortion = ( tmp & 0x8000)? 0x7FFF : tmp; // only half of the fader
    gain = tmp - distortion; // 2nd half of the fader
    offset_gain = gain; // 15 bits
    gain *= gain; // 30 bits max
    gain >>= 15;
    gain *= offset_gain;
  
    offset_signed = 0x7FFF - (symetrie>>16); // from -1 to 1; 15 bit + sign
    offset_signed *= offset_gain; // 15 + 15 + sign
    offset_signed >>= 15; 
    
    phase = LFO3_phase; // 32 bits
    
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
    
    all = tmpS + (1<<15);    
    break;
  case 1 : // AM
    freq = (adc_value16[LFO3_1] >> 1) + 1111;
    LFO3_increment = ((freq * ((freq * freq) >> 16)) >> 6);
    
    freq = (adc_value16[LFO3_2] >> 1) + 1111;
    LFO3b_increment = ((freq * ((freq * freq) >> 16)) >> 6);
    
    fader3 = adc_value16[LFO3_3];
    mix = min(fader3, 0x7FFF); // 15 bit
    gain = fader3 - mix; // 15 bit
    gain *= gain;
    gain >>= 15;
    
    phaseS = LFO3b_phase; // - (1<<15); // passage en signed
    phaseS = -(1<<15) + (abs(phaseS)>>15); // triangle
    
    phaseS *= gain + (1<<12);
    phaseS = max(min((1<<27)-1, phaseS),1-(1<<27)); // clip

    phaseS <<= 3;
    phase = phaseS+0x40000000; // unsigned

    sin1 = fast_sin(LFO3_phase);
    sin2 = fast_cos(phase);  
    sin2 >>= 15;
    
    sin2 *= mix;
    sin2 >>= 16;    
    sin1 = (sin1 >> 16) * (0xFFFF-sin2);
    all = sin1>>16;
    break;
  case 2 : //FM
    fader3 = adc_value16[LFO3_3];
    gain = max(fader3, 0x7FFF); // 15 bit
    gain -= 0x7FFF;
    gain *= gain;
    gain >>= 16;
    sin2 = fast_cos(LFO3b_phase);
    sin2S = sin2^0x80000000;    
    sin2S >>= 16;
    sin2S *= (1 << 11) + gain;
    sin2S >>= 11;
    sin2S = (sin2S < -0x7FFF)? -0x7FFF: sin2S; // clip low
    sin2S = (sin2S >  0x7F00)?  0x7F00: sin2S; // clip high
    sin2S *= fader3;
    sin2S >>= 16;
  
    freq = (adc_value16[LFO3_1] >> 1);
    freqS = freq + sin2S;
    tmpS = freqS * (freqS>>1);
    tmpS = freqS * (tmpS>>16);
    LFO3_increment = tmpS>>6;
        
    freq = (adc_value16[LFO3_2] >> 1) + 1111;
    LFO3b_increment = ((freq * ((freq * freq) >> 16)) >> 6);
    
    sin1 = fast_sin(LFO3_phase);
    all = sin1>>16;
    break;
  case 3 : //MIX
    tmp = adc_value16[LFO3_3];
    mix = min(tmp, 0x7FFF);
    gain = tmp-mix;
    gain *= gain;
    gain >>= 15;
    
    freq = (adc_value16[LFO3_1] >> 1) + 1111;
    LFO3_increment = ((freq * ((freq * freq) >> 16)) >> 6);
    freq = (adc_value16[LFO3_2] >> 1) + 1111;
    LFO3b_increment = ((freq * ((freq * freq) >> 16)) >> 6);
  
    sin1 = abs((int32_t)LFO3_phase);
    sin2 = abs((int32_t)LFO3b_phase);
    sin1 = MIX15(sin1>>15, sin2>>15, mix>>1);
   
    tmpS = sin1-(1<<15); // passage en signed
    tmpS *= (1 << 10) + ((gain*gain)>>17);
    tmpS >>= 10;
    
    tmpS = (tmpS < -0x7F00)? -0x7F00: tmpS; // clip low
    tmpS = (tmpS >  0x7F00)?  0x7F00: tmpS; // clip high
  
    all = tmpS + (1<<15);
    //all = (1<<15) + (2*tmpS) - ((tmpS*abs(tmpS)) >> 15);   // calcul du sinus
    break;
  case 4 : // STEP
    freq = (adc_value16[LFO3_1] >> 1) + 800;
    LFO3_increment = (freq * freq) >> 9;
    LFO3b_increment = 0;
    
    tmp = (fast_sin(adc_value16[LFO3_3]*LFO3b_phase<<3)>>2) + 0x20000000;
    
    if (LFO3_phase > tmp) {
      LFO3_phase -= tmp;
      LFO3b_phase = (LFO3b_phase + adc_value16[LFO3_2]) & 0x1FFFF;
    }
    all = (LFO3b_phase < 0x10000)? LFO3b_phase:0x1FFFF-LFO3b_phase;
   break;  
  case 5 : //WS : not used
    freq = (adc_value16[LFO3_1] >> 1) + 1111;
    LFO3_increment = ((freq * ((freq * freq) >> 16)) >> 6);
    freq = (adc_value16[LFO3_2] >> 1) + 1111;
    LFO3b_increment = ((freq * ((freq * freq) >> 16)) >> 6);
    
    sin1 = fast_sin(LFO3_phase);
    //sin2 = fast_sin(LFO3b_phase);
    
    fader3 = adc_value16[LFO3_3];
    mix = min(fader3, 0x7FFF); // 15 bit
    gain = fader3 - mix; // 15 bit
    phaseS = LFO3b_phase; // - (1<<15); // passage en signed
    phaseS = -(1<<15) + (abs(phaseS)>>15); // triangle
    phaseS *= gain + (1<<12);
    phaseS = max(min((1<<27)-1, phaseS),1-(1<<27)); // clip
    phaseS <<= 3;
    phase = phaseS+0x40000000; // unsigned
    sin2 = fast_cos(phase);  
    sin2 >>= 15;
    sin2 *= mix;
    //sin2 >>= 16;    
    
    // sin2 = adc_value16[LFO3_3] * (sin2 >> 16);
    sin1 = waveshaper(sin1, sin2 >> 2);
    all = sin1>>16;
    break;
  case 6 : // CLIP : not used
    freq = (adc_value16[LFO3_1] >> 1) + 1111;
    LFO3_increment = ((freq * ((freq * freq) >> 16)) >> 6);
    freq = (adc_value16[LFO3_2] >> 1) + 1111;
    LFO3b_increment = ((freq * ((freq * freq) >> 16)) >> 6);
  
    sin1 = abs((int32_t)LFO3_phase);
    sin2 = abs((int32_t)LFO3b_phase);
    sin1 += sin2;
    sin1 >>= 16;
   
    tmpS = sin1-(1<<15); // passage en signed
    tmpS *= (1 << 10) + ((adc_value16[LFO3_3]*adc_value16[LFO3_3])>>17);
    tmpS >>= 11;
    
    tmpS = (tmpS < -0x7F00)? -0x7F00: tmpS; // clip low
    tmpS = (tmpS >  0x7F00)?  0x7F00: tmpS; // clip high
  
    // calcul du sinus
    all = (1<<15) + (2*tmpS) - ((tmpS*abs(tmpS)) >> 15);
    break;
  case 7 : //FM old
    fader3 = adc_value16[LFO3_3];
    mix = min(fader3, 0x7FFF); // 15 bit
    gain = fader3 - mix; // 15 bit
    gain *= gain;
    gain >>= 15;
    phaseS = LFO3b_phase; // - (1<<15); // passage en signed
    phaseS = -(1<<15) + (abs(phaseS)>>15); // triangle
    phaseS *= gain + (1<<12);
    phaseS = max(min((1<<27)-1, phaseS),1-(1<<27)); // clip
    phaseS <<= 3;
    phase = phaseS+0x40000000; // unsigned
    sin2 = fast_cos(phase);  

    freq = (adc_value16[LFO3_1] >> 1) + 1111;
    tmpS  = ((sin2 >> 16) -(1<<15)) * adc_value16[LFO3_3];
    tmpS >>= 18;

    freq += tmpS + 0x80000000;
    freq = (freq<0x80000000)? 0: freq-0x80000000;
    LFO3_increment = ((freq * ((freq * freq) >> 16)) >> 6);
    
    freq = (adc_value16[LFO3_2] >> 1) + 1111;
    LFO3b_increment = ((freq * ((freq * freq) >> 16)) >> 6);
    
    sin1 = fast_sin(LFO3_phase);
    all = sin1>>16;
    break;
  }
  
  //modulation_data[mod_CVG] = all - (1<<15);
  //modulation_data_AM[mod_CVG] = all;
  LFO3MOD_increment = (all - modulation_data_AM[mod_LFO3])/8; 
  //analog_out_1(all<<16);
}

inline void LFO3_audio() {
  int32_t tmp;
  LFO3_phase += LFO3_increment;
  LFO3b_phase += LFO3b_increment;
  
  tmp = modulation_data_AM[mod_LFO3] + LFO3MOD_increment;
  modulation_data[mod_LFO3] = tmp  - (1<<15);  // linear interpolation to smooth the data
  modulation_data_AM[mod_LFO3] = tmp;
}
