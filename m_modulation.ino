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

uint32_t JXp, JXm, JYp, JYm;
uint32_t JXp_filter, JYp_filter, JXm_filter, JYm_filter;
uint32_t CV_filter;
 

inline void mod_cv() {
  uint32_t tmp;
  tmp = hysteresis16(adc_value16[EXT_1], CV_filter);
  CV_filter = tmp;
}

// external CV
inline void update_ext() {
  uint32_t tmp;
  int32_t tmp2;
  
  #ifdef EXT_ANA 
    tmp = CV_filter;
    modulation_data[mod_EXT1] = (tmp-(1<<15))<<2;
    modulation_data_AM[mod_EXT1] = tmp;
  #endif
  #ifdef EXT_MIDI
    modulation_data[mod_EXT1] = MIDI_PITCHWHEEL-(1<<15);
    modulation_data[mod_EXT1] = MIDI_PITCHWHEEL;
  #endif
  #ifdef EXT_AUTO
    if (use_midi_pitchwheel) {
      tmp = MIDI_PITCHWHEEL;
      modulation_data[mod_EXT1] = tmp-(1<<15);
      modulation_data_AM[mod_EXT1] = tmp;
    }
    else {
      tmp = CV_filter;
      tmp2 = tmp -(1<<15);
      tmp2 <<= 2;
      modulation_data[mod_EXT1] = tmp2;
      modulation_data_AM[mod_EXT1] = tmp;
    }
  #endif
  }

inline void init_joystick() {
  modulation_data[mod_1] = (1<<15)-1;
  modulation_data_AM[mod_1] = (1<<16)-1;
}

inline void joystick() {  
  int32_t joystickX, joystickY;
  
  joystickX = adc_value16[XY_X];
  joystickX += MIDI_joystick_X; // valeur MIDI
  joystickX = (joystickX > 0)? joystickX:0; // clip low/high
  joystickX = (joystickX > 0xFFFF)? 0xFFFF:joystickX;
   
  joystickY = adc_value16[XY_Y];
  joystickY += MIDI_joystick_Y; // valeur MIDI
  joystickY = (joystickY > 0)? joystickY:0; // clip low/high
  joystickY = (joystickY > 0xFFFF)? 0xFFFF:joystickY;
  
  JXp = (0x7000 > joystickX)? (0x7000 - joystickX): 0; // analog value are reversed (+3.3 for - and 0 for +)
  JXm = (joystickX > 0x9000)? (joystickX - 0x9000): 0;
  JYp = (0x7000 > joystickY)? (0x7000 - joystickY): 0;
  JYm = (joystickY > 0x9000)? (joystickY - 0x9000): 0;      
}

inline void joystick_audio() {
  int32_t JStick;
  uint32_t local_somme;
  uint32_t tmpXm, tmpYm, tmpXp, tmpYp;  
  
  tmpXm = filter(JXm << 10, JXm_filter, 10);
  JXm_filter = tmpXm;
  tmpXm /= 0x380;
  
  tmpXp = filter(JXp << 10, JXp_filter, 10);
  JXp_filter = tmpXp;
  tmpXp /= 0x380;
  
  tmpYm = filter(JYm << 10, JYm_filter, 10);
  JYm_filter = tmpYm;
  tmpYm /= 0x380;
  
  tmpYp = filter(JYp << 10, JYp_filter, 10);
  JYp_filter = tmpYp;
  tmpYp /= 0x380;
  
  JStick  = tmpXm * modulation_data[modulation_index[index_Xm]];
  JStick += tmpXp * modulation_data[modulation_index[index_Xp]];
  JStick += tmpYm * modulation_data[modulation_index[index_Ym]];
  JStick += tmpYp * modulation_data[modulation_index[index_Yp]];
  local_somme = tmpXm + tmpXp + tmpYm + tmpYp;
  
  JStick >>= 15;
  modulation_data[mod_XY] = JStick;
  modulation_data_AM[mod_XY] = JStick + local_somme;
  
  //analog_out_1((modulation_data[mod_XY]<<16));
  //analog_out_2((modulation_data_AM[mod_XY]<<16)^0x80000000);
}

inline void modulation_audio() {
  uint32_t tmp, tmp2; 
  
  tmp =  (audio_inR^0x80000000);
  tmp >>= 16;
  modulation_data[mod_L] = tmp - (1<<15);
  modulation_data_AM[mod_L] = tmp;
  
  tmp2 =  (audio_inL^0x80000000);
  tmp2 >>= 16;
  modulation_data[mod_R] = tmp2 - (1<<15);
  modulation_data_AM[mod_R] = tmp2;  
  
  tmp = (tmp + tmp2) >> 1;
  modulation_data[mod_LR] = tmp - (1<<15);
  modulation_data_AM[mod_LR] = tmp;  
  
}
