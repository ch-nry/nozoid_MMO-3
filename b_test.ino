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

uint32_t lastKEY;

inline void test(){ // hardware test
  uint32_t tmp32, KEY_ROWL_tmp, test, tmp2;
  
  all_key();
  test = (KEY_global & 1<<5);
    
  if(test) {
    analogWrite(8, 255);  // lazy PWM configuration
}

  while(test) { // do not exit test mode

    analog_start_1();
    all_key();
    analog_in();
       
    if (KEY_global & 1<<5)  KEY_ROWL_tmp = 1;
    if (KEY_global & 1<<11) KEY_ROWL_tmp = 2;
    if (KEY_global & 1<<17) KEY_ROWL_tmp = 3;
    if (KEY_global & 1<<20) KEY_ROWL_tmp = 4;
    if (KEY_global & 1<<23) KEY_ROWL_tmp = 5;
    if (KEY_global & 1<<4)  KEY_ROWL_tmp = 6;
    if (KEY_global & 1<<10) KEY_ROWL_tmp = 7;
    if (KEY_global & 1<<16) KEY_ROWL_tmp = 8;
    if (KEY_global & 1<<19) KEY_ROWL_tmp = 9;
    if (KEY_global & 1<<9)  KEY_ROWL_tmp = 10;
    if (KEY_global & 1<<3)  KEY_ROWL_tmp = 11;
    if (KEY_global & 1<<12) KEY_ROWL_tmp = 12;
    if (KEY_global & 1<<6)  KEY_ROWL_tmp = 13;
    if (KEY_global & 1<<18) KEY_ROWL_tmp = 14;
    if (KEY_global & 1<<0)  KEY_ROWL_tmp = 15;

    if (KEY_ROWL_tmp == 0) KEY_ROWL_tmp = lastKEY;
    lastKEY = KEY_ROWL_tmp;

    switch (KEY_ROWL_tmp) {
    case 1: // all led on
      led1_on();
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, 255);
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, 255);
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, 255);
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, 255);
      led_VCO1_on();
      led_VCO2_on();
    break;
    case 2: // all led off
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, 0);
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, 0);
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, 0);
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, 0);
      led1_off();
      led_VCO1_off();
      led_VCO2_off();
    break;
    case 3: // analog value
      tmp32 = adc_value16[VCO1_FQ] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);
      tmp32 = adc_value16[VCO1_MOD1] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value16[VCO1_MOD2] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value16[VCO1_MOD3] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);      
    break;
    case 4:
      tmp32 = adc_value16[VCO2_FQ] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);
      tmp32 = adc_value16[VCO2_MOD1] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value16[VCO2_MOD2] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value16[VCO2_MOD3] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);       
    break;
    case 5:
      tmp32 = adc_value16[VCO3_FQ] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);
      tmp32 = adc_value16[VCO3_MOD1] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value16[VCO3_MOD2] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value16[VCO3_MOD3] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);        
    break;
    case 6:
      tmp32 = adc_value16[LFO1_FQ] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);
      tmp32 = adc_value16[LFO1_WF] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value16[LFO1_SYM] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value16[ADSR_A] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);        
    break;
    case 7:
      tmp32 = adc_value16[LFO2_1] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);
      tmp32 = adc_value16[LFO2_2] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value16[LFO2_3] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value16[ADSR_D] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);        
    break;
    case 8:
      tmp32 = adc_value16[LFO3_1] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);
      tmp32 = adc_value16[LFO3_2] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value16[LFO3_3] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value16[ADSR_S] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);        
    break;
    case 9:
      tmp32 = adc_value16[MIX_1] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);
      tmp32 = adc_value16[MIX_2] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value16[MIX_3] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value16[ADSR_R] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);         
    break;
    case 10:
      tmp32 = adc_value16[VCA_GAIN] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);
      tmp32 = adc_value16[PORTAMENTO_VALUE] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value16[XY_X] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value16[XY_Y] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);  
    break;
    case 11: // version number
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, 0);
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, 0);
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, 0);
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, 255);
    break;
    case 12: // joystick center 
      if (adc_value16[XY_X] < 0x7000) PWMC_SetDutyCycle(PWM_INTERFACE, 5, 255);
      else PWMC_SetDutyCycle(PWM_INTERFACE, 5, 0);
      if (adc_value16[XY_X] > 0x9000) PWMC_SetDutyCycle(PWM_INTERFACE, 4, 255);
      else PWMC_SetDutyCycle(PWM_INTERFACE, 4, 0);
      if (adc_value16[XY_Y] < 0x7000) PWMC_SetDutyCycle(PWM_INTERFACE, 6, 255);
      else PWMC_SetDutyCycle(PWM_INTERFACE, 6, 0);
      if (adc_value16[XY_Y] > 0x9000) PWMC_SetDutyCycle(PWM_INTERFACE, 7, 255);
      else PWMC_SetDutyCycle(PWM_INTERFACE, 7, 0);      
    break;
    case 13:
      start_dac();
      tmp32 = 2; 
      if (GATE_EXT()) tmp32 = 255;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, tmp32);
      tmp32 = adc_value16[EXT_1] >> 7;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = ((int32_t)abs(audio_inR))>>22;
      tmp32 = min(tmp32, 0xFF);
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, tmp32);
      tmp32 = ((int32_t)abs(audio_inL))>>22;;
      tmp32 = min(tmp32, 0xFF);
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, tmp32); 
    break;
    case 14: // keyboard : up row
      if (KEY_global & 1<<2)  led_VCO1_on(); else led_VCO1_off(); 
      if (KEY_global & 1<<8)  led_VCO2_on(); else led_VCO2_off(); 
      if (KEY_global & 1<<14) tmp32 = 255; else tmp32 = 0; 
        PWMC_SetDutyCycle(PWM_INTERFACE, 5, tmp32);
      if (KEY_global & 1<<1)  tmp32 = 255; else tmp32 = 0; 
        PWMC_SetDutyCycle(PWM_INTERFACE, 4, tmp32);
      if (KEY_global & 1<<7)  tmp32 = 255; else tmp32 = 0; 
        PWMC_SetDutyCycle(PWM_INTERFACE, 6, tmp32);
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, 0);
    break;
    case 15: // key + inverseur
      if (KEY_global & 1<<13)  led_VCO1_on(); else led_VCO1_off(); 
      if (KEY_global & 1<<22)  led_VCO2_on(); else led_VCO2_off(); 
      if (KEY_global & 1<<21) tmp32 = 255; else tmp32 = 0; 
        PWMC_SetDutyCycle(PWM_INTERFACE, 5, tmp32);
      if (KEY_global & 1<<24)  tmp32 = 255; else tmp32 = 0; 
        PWMC_SetDutyCycle(PWM_INTERFACE, 4, tmp32);
      if (KEY_global & 1<<15)  tmp32 = 255; else tmp32 = 0; 
        PWMC_SetDutyCycle(PWM_INTERFACE, 6, tmp32);
        PWMC_SetDutyCycle(PWM_INTERFACE, 7, 0);
      if (KEY_MENU()) led1_on(); else led1_off();
    break;
    }
  }
}
