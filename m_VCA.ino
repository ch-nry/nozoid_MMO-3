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

uint32_t VCA_GAIN_value, VCA_GAIN_value_old, gain_filter;
uint32_t MIX1_old, MIX1_filter, MIX1_value;
uint32_t MIX2_old, MIX2_filter, MIX2_value;
uint32_t MIX3_old, MIX3_filter, MIX3_value;

inline void init_VCA() {
}

inline uint32_t disto(int32_t in) { 
  int32_t tmp;
  tmp = in - (in >> 12);
  tmp = tmp-m_s32xs32_s32H(tmp, abs(tmp));
  tmp <<= 1;
  #ifdef double_disto
    tmp = tmp - (tmp >> 12);
    tmp = tmp-m_s32xs32_s32H(tmp, abs(tmp));
    tmp <<= 1;
  #endif
  return(tmp); 
}

inline void MIX_update1() { // started from analog_in
  uint32_t MIX1_local;
   
  MIX1_local = hysteresis16(clip_ana_low16(adc_value16[MIX_1]), MIX1_old);
  MIX1_old = MIX1_local;
  MIX1_local *= MIX1_local; // pow 2 for a nice amplitude curve
  MIX1_value = MIX1_local;
}

inline void MIX_update2() {
  uint32_t MIX2_local;

  MIX2_local = hysteresis16(clip_ana_low16(adc_value16[MIX_2]), MIX2_old);
  MIX2_old = MIX2_local;
  MIX2_local *= MIX2_local; // pow 2 for a nice amplitude curve
  MIX2_value = MIX2_local;
}  

inline void MIX_update3() {
  uint32_t MIX3_local;

  MIX3_local = hysteresis16(clip_ana_low16(adc_value16[MIX_3]), MIX3_old);
  MIX3_old = MIX3_local;
  MIX3_local *= MIX3_local; // pow 2 for a nice amplitude curve
  MIX3_value = MIX3_local;
}

inline sound MIX(uint32_t VCO1_out, uint32_t VCO2_out, uint32_t VCO3_out, uint32_t ADSR_out) {
  int32_t OSC1_local, OSC2_local, OSC3_local;
  uint32_t MIX1_local, MIX2_local, MIX3_local;
  int32_t MIX_localL, MIX_localR, MIX_local;
  uint32_t tmp;
  sound stereo;
  uint32_t ADSR_local = ADSR_out;

  // filter des 3 gains
  MIX1_local = MIX1_filter;
  MIX1_local += (int32_t)(MIX1_value - MIX1_local) >> 10;
  MIX1_filter = MIX1_local; // 32 bit unsigned

  MIX2_local = MIX2_filter;
  MIX2_local += (int32_t)(MIX2_value - MIX2_local) >> 10;
  MIX2_filter = MIX2_local; // 32 bit unsigned

  MIX3_local = MIX3_filter;
  MIX3_local += (int32_t)(MIX3_value - MIX3_local) >> 10;
  MIX3_filter = MIX3_local; // 32 bit unsigned
  
  // gain des 3 OSC
  OSC1_local = VCO1_out^0x80000000; // switch to signed
  OSC1_local = (OSC1_local >> 16) * (MIX1_local >> 17);

  OSC2_local = VCO2_out^0x80000000; // switch to signed
  OSC2_local = (OSC2_local >> 16) * (MIX2_local >> 17);

  OSC3_local = VCO3_out^0x80000000; // switch to signed
  OSC3_local = (OSC3_local >> 16) * (MIX3_local >> 17);


  // ADSR GAIN PRE
  if((ADSR_mode == 0) && (GATE_mode < 3)) { // ADSR en pre MIX
    
    ADSR_local >>= 15;
    OSC1_local = (OSC1_local >> 15) * ADSR_local;
    OSC2_local = (OSC2_local >> 15) * ADSR_local;
    OSC3_local = (OSC3_local >> 15) * ADSR_local;
  }

  // switch audio mode : somation + clip + disto
  switch (audio_out_mode) {
  case 0:
    MIX_local  = OSC1_local >> 1;
    MIX_local += OSC2_local >> 1;
    MIX_local += OSC3_local >> 1;
    
    MIX_local = clip_S32_30(MIX_local);
    MIX_local <<= 2;
    MIX_localL = disto(MIX_local);
    MIX_localR = MIX_localL;
  break;
  case 1:
    MIX_local  = OSC1_local >> 2;
    MIX_local += OSC2_local >> 1;
    MIX_local += OSC3_local >> 2;

    MIX_localL  = MIX_local;
    MIX_localL += OSC1_local >> 2;

    MIX_localR  = OSC3_local >> 2;
    MIX_localR += MIX_local;
    
    MIX_localL = clip_S32_30(MIX_localL);
    MIX_localL <<= 2;
    MIX_localR = clip_S32_30(MIX_localR);
    MIX_localR <<= 2;
        
    MIX_localL = disto(MIX_localL);
    MIX_localR = disto(MIX_localR);
  break;
  case 2:
    MIX_localL  = OSC1_local;
    MIX_localL += OSC2_local;
    MIX_localR  = OSC3_local;
    MIX_localR += OSC2_local;

    MIX_localL = clip_S32_30(MIX_localL);
    MIX_localL <<= 2;
    MIX_localR = clip_S32_30(MIX_localR);
    MIX_localR <<= 2;

    MIX_localL = disto(MIX_localL);
    MIX_localR = disto(MIX_localR);
  break;  
  }

  // ADSR GAIN POST
  if((ADSR_mode == 1) && (GATE_mode < 3)) { // ADSR en post MIX
    tmp = (ADSR_local >> 14);
    MIX_localL = (MIX_localL >> 16) * tmp;
    MIX_localR = (MIX_localR >> 16) * tmp;
  }
  stereo.U32[0] = MIX_localL;
  stereo.U32[1] = MIX_localR;
  return(stereo);
}

inline void VCA_update(){
  uint32_t VCA_MIX_value_local, VCA_GAIN_value_local;
    
  VCA_GAIN_value_local = hysteresis16(clip_ana_low16(adc_value16[VCA_GAIN]), VCA_GAIN_value_old);
  VCA_GAIN_value_old = VCA_GAIN_value_local;
  
  VCA_GAIN_value_local *= VCA_GAIN_value_local; // pow 2 for a nice amplitude curve
  VCA_GAIN_value_local >>= 1;
  VCA_GAIN_value = VCA_GAIN_value_local;
}

inline sound VCA(sound stereo) {
  uint32_t gain, tmp;
  int32_t tmp32, env;
  sound stereo_out;
  
  tmp = gain_filter;
  tmp += (int32_t)(VCA_GAIN_value - tmp) >> 10;
  gain_filter = tmp;
  tmp >>= 15;
  stereo_out.U32[0] = (stereo.U32[0] >>16) * tmp;
  stereo_out.U32[1] = (stereo.U32[1] >>16) * tmp;
  
  //analog_out_1( stereo_out.U32[0]^0x80000000);
  //analog_out_2( stereo_out.U32[1]^0x80000000);
  
  return(stereo_out);
}

