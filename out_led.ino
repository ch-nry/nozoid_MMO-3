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

#define PORTLED1 PIOC
#define LED1 12 // MIDI LED
#define PORTLEDVCO1 PIOA
#define LEDVCO1 13
#define PORTLEDVCO2 PIOA
#define LEDVCO2 12
#define PORTLEDVCO3 PIOC
#define LEDVCO3 22
#define PORTLEDVCO4 PIOC
#define LEDVCO4 21

uint32_t led_MIDI;

inline void init_led() {
  PORTLED1->PIO_PER = 1 << LED1;
  PORTLED1->PIO_OER = 1 << LED1; 

  PORTLEDVCO1->PIO_PER = 1 << LEDVCO1;
  PORTLEDVCO1->PIO_OER = 1 << LEDVCO1;
  PORTLEDVCO2->PIO_PER = 1 << LEDVCO2;
  PORTLEDVCO2->PIO_OER = 1 << LEDVCO2;
  PORTLEDVCO3->PIO_PER = 1 << LEDVCO3;
  PORTLEDVCO3->PIO_OER = 1 << LEDVCO3;
  PORTLEDVCO4->PIO_PER = 1 << LEDVCO4;
  PORTLEDVCO4->PIO_OER = 1 << LEDVCO4;
  led1_off();

  led1_time = 300; // small flash at initialisation

  analogWrite(7, 0);  // lazy PWM configuration
  analogWrite(6, 0);
  analogWrite(9, 0);

  blink_time = 1;
}


inline void led1_on() {
  PORTLED1->PIO_SODR =  1 << LED1;
}

inline void led1_off() {
  PORTLED1->PIO_CODR =  1 << LED1;
}

inline void led_VCO1_on() {
  PORTLEDVCO1->PIO_SODR =  1 << LEDVCO1;
}

inline void led_VCO1_off() {
  PORTLEDVCO1->PIO_CODR =  1 << LEDVCO1;
}

inline void led_VCO2_on() {
  PORTLEDVCO2->PIO_SODR =  1 << LEDVCO2;
}

inline void led_VCO2_off() {
  PORTLEDVCO2->PIO_CODR =  1 << LEDVCO2;
}

inline void led_VCO3_on() {
  PORTLEDVCO3->PIO_SODR =  1 << LEDVCO3;
}

inline void led_VCO3_off() {
  PORTLEDVCO3->PIO_CODR =  1 << LEDVCO3;
}

inline void led_VCO4_on() {
  PORTLEDVCO4->PIO_SODR =  1 << LEDVCO4;
}

inline void led_VCO4_off() {
  PORTLEDVCO4->PIO_CODR =  1 << LEDVCO4;
}

inline void update_leds() { 
  uint32_t time_tmp, tmp32, led, tmp;
  bool led1;
 
  if (blink_led > 0) {
    blink_time = (++blink_time) & 0x0FFF;
    tmp = blink_led-1; // 0 1 2 3
    tmp = (1<<tmp)-1; // 0 1 3 7
    tmp *= 0x3FF;
    led = (tmp > blink_time)?1:0;
  } 
  else {
    if (led1_time > 0) led1_time--;
    led = led1_time;
  }
  if ((led != 0) ){ 
    if (MODE_MENU) 
      led1_off();
    else 
      led1_on();
  }
  else {
    if (MODE_MENU) 
      led1_on();
    else 
      led1_off();
  }


  // todo : optimized, and more precision PWM, 
  tmp32 = (masque_led==0)? and_led2:(modulation_data_AM[mod_LFO1])>>7;
  PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
  tmp32 = (masque_led==0)? and_led3:(modulation_data_AM[mod_LFO2])>>7;
  PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
  tmp32 = (masque_led==0)? and_led4:(modulation_data_AM[mod_LFO3])>>7;
  PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);

}
