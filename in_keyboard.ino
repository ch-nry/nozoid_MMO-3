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

#define PORTKEYOUT1 PIOD
#define KEYOUT1 7
#define PORTKEYOUT2 PIOC
#define KEYOUT2 29
#define PORTKEYOUT3 PIOC
#define KEYOUT3 25
#define PORTKEYIN1 PIOA
#define KEYIN1 17
#define PORTKEYIN2 PIOC
#define KEYIN2 3
#define PORTKEYIN3 PIOB
#define KEYIN3 26
#define PORTKEY25 PIOB
#define KEY25 27
#define PORTKEYMENU PIOD
#define KEYMENU 8
#define PORTGATEEXT PIOA
#define GATEEXT 20
#define PORTGATEEXT1 PIOA
#define GATEEXT1 18

uint32_t keyboard_state;
uint32_t KEY_old, KEY;
bool MODE_MENU; 
uint32_t masque_led, and_led2, and_led3, and_led4;

inline void init_keyboard() {
  PORTKEYOUT1->PIO_PER  = 1 << KEYOUT1; // out to control demux bus
  PORTKEYOUT1->PIO_OER  = 1 << KEYOUT1; 
  PORTKEYOUT2->PIO_PER  = 1 << KEYOUT2;
  PORTKEYOUT2->PIO_OER  = 1 << KEYOUT2; 
  PORTKEYOUT3->PIO_PER  = 1 << KEYOUT3; // out to control demux bus
  PORTKEYOUT3->PIO_OER  = 1 << KEYOUT3; 
  
  PORTKEYIN1->PIO_PER   = 1 << KEYIN1; // input with pull up to get switch value
  PORTKEYIN1->PIO_PUER  = 1 << KEYIN1;
  PORTKEYIN1->PIO_ODR   = 1 << KEYIN1;
  PORTKEYIN1->PIO_IDR   = 1 << KEYIN1;
  PORTKEYIN1->PIO_ABSR  = 1 << KEYIN1;
  PORTKEYIN1->PIO_AIMDR  = 1 << KEYIN1;
  
  PORTKEYIN2->PIO_PER   = 1 << KEYIN2;
  PORTKEYIN2->PIO_PUER  = 1 << KEYIN2;
  PORTKEYIN3->PIO_PER   = 1 << KEYIN3; 
  PORTKEYIN3->PIO_PUER  = 1 << KEYIN3;
  PORTKEY25->PIO_PER    = 1 << KEY25;
  PORTKEY25->PIO_PUER   = 1 << KEY25;
  PORTKEYMENU->PIO_PER  = 1 << KEYMENU;
  PORTKEYMENU->PIO_PUER = 1 << KEYMENU;
  PORTGATEEXT->PIO_PER  = 1 << GATEEXT;
  PORTGATEEXT->PIO_PUER = 1 << GATEEXT;
  PORTGATEEXT1->PIO_PER  = 1 << GATEEXT1;
  PORTGATEEXT1->PIO_PUER = 1 << GATEEXT1;


  keyboard_state = 0;
  KEY_LOCAL = 64 << 20;
  KEY_LOCAL_goal = KEY_LOCAL;
  MODE_MENU = 0;
  masque_led = 0xFFFFFFFF;
}

inline bool KEY_MENU(){
  return(!(PORTKEYMENU->PIO_PDSR & (1<<KEYMENU)));
}

inline bool GATE_EXT(){
  return((PORTGATEEXT->PIO_PDSR & (1<<GATEEXT)) || (!(PORTGATEEXT1->PIO_PDSR & (1<<GATEEXT1))));
}

inline void all_key() { // only used in test mode
    PORTKEYOUT1->PIO_CODR = 1<<KEYOUT1; 
    PORTKEYOUT2->PIO_CODR = 1<<KEYOUT2; 
    PORTKEYOUT3->PIO_CODR = 1<<KEYOUT3;
    delayMicroseconds(10);
    KEY = 0;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY++;
    KEY <<= 1;

    PORTKEYOUT1->PIO_SODR = 1<<KEYOUT1; 
    //PORTKEYOUT2->PIO_CODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_CODR = 1<<KEYOUT3;
    delayMicroseconds(10);
    
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY++; 
    KEY <<= 1;
    
    PORTKEYOUT1->PIO_CODR = 1<<KEYOUT1; 
    PORTKEYOUT2->PIO_SODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_CODR = 1<<KEYOUT3;
    delayMicroseconds(10);

    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY++;  
    KEY <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY++;
    KEY <<= 1;
    
    PORTKEYOUT1->PIO_SODR = 1<<KEYOUT1; 
    //PORTKEYOUT2->PIO_SODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_CODR = 1<<KEYOUT3;
    delayMicroseconds(10);

    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY++;  
    KEY <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY++;  
    KEY <<= 1;

    PORTKEYOUT1->PIO_CODR = 1<<KEYOUT1; 
    PORTKEYOUT2->PIO_CODR = 1<<KEYOUT2; 
    PORTKEYOUT3->PIO_SODR = 1<<KEYOUT3;
    delayMicroseconds(10);

    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY++;  
    KEY <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY++;  
    KEY <<= 1;

    PORTKEYOUT1->PIO_SODR = 1<<KEYOUT1; 
    //PORTKEYOUT2->PIO_CODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_SODR = 1<<KEYOUT3;
    delayMicroseconds(10);

    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY++;  
    KEY <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY++;  
    KEY <<= 1;

    PORTKEYOUT1->PIO_CODR = 1<<KEYOUT1; 
    PORTKEYOUT2->PIO_SODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_SODR = 1<<KEYOUT3;
    delayMicroseconds(10);

    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY++;  
    KEY <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY++;  
    KEY <<= 1;

    PORTKEYOUT1->PIO_SODR = 1<<KEYOUT1; 
    //PORTKEYOUT2->PIO_SODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_SODR = 1<<KEYOUT3;
    delayMicroseconds(10);

    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY++;  
    KEY <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY++;  
    KEY <<= 1;
    if (PORTKEY25->PIO_PDSR & (1<<KEY25)) KEY++;  

    KEY_global = KEY^0x1FFFFFF; // invert value
}

inline void keyboard_in() { 
  uint32_t test_Y_N, test_module, tmp;
  uint32_t KEY_tmp;
  uint32_t KEY_LOCAL_tmp, KEY_ROWL_tmp, KEY_ROWH_tmp, KEY_ROW3_tmp, KEY_ROWL1_tmp, KEY_ROWL2_tmp;
  uint32_t index1, index2;
  
  switch (keyboard_state) {
  case 0 :
    // get value in no specific order
    KEY_tmp = 0;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY_tmp++; 
    KEY = KEY_tmp;
    
    keyboard_state++;
    PORTKEYOUT1->PIO_SODR = 1<<KEYOUT1; 
    //PORTKEYOUT2->PIO_CODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_CODR = 1<<KEYOUT3;
    break;
    
  case 1 :
    KEY_tmp = KEY << 1;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY_tmp++; 
    KEY = KEY_tmp;

    keyboard_state++;
    PORTKEYOUT1->PIO_CODR = 1<<KEYOUT1; 
    PORTKEYOUT2->PIO_SODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_CODR = 1<<KEYOUT3;
    break;

  case 2 :
    KEY_tmp = KEY << 1;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY_tmp++;  
    KEY_tmp <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY_tmp++;  
    KEY = KEY_tmp;

    keyboard_state++;
    PORTKEYOUT1->PIO_SODR = 1<<KEYOUT1; 
    //PORTKEYOUT2->PIO_SODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_CODR = 1<<KEYOUT3;
    break;

  case 3 :
    KEY_tmp = KEY << 1;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY_tmp++;  
    KEY_tmp <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY_tmp++;  
    KEY = KEY_tmp;

    keyboard_state++;
    PORTKEYOUT1->PIO_CODR = 1<<KEYOUT1; 
    PORTKEYOUT2->PIO_CODR = 1<<KEYOUT2; 
    PORTKEYOUT3->PIO_SODR = 1<<KEYOUT3;
    break;
    
  case 4 :
    KEY_tmp = KEY << 1;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY_tmp++;  
    KEY_tmp <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY_tmp++;  
    KEY = KEY_tmp;

    keyboard_state++;
    PORTKEYOUT1->PIO_SODR = 1<<KEYOUT1; 
    //PORTKEYOUT2->PIO_CODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_SODR = 1<<KEYOUT3;
    break;
    
  case 5 :
    KEY_tmp = KEY << 1;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY_tmp++;  
    KEY_tmp <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY_tmp++;  
    KEY = KEY_tmp;

    keyboard_state++;
    PORTKEYOUT1->PIO_CODR = 1<<KEYOUT1; 
    PORTKEYOUT2->PIO_SODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_SODR = 1<<KEYOUT3;
    break;
    
  case 6 :
    KEY_tmp = KEY << 1;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY_tmp++;  
    KEY_tmp <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY_tmp++;  
    KEY = KEY_tmp;

    keyboard_state++;
    PORTKEYOUT1->PIO_SODR = 1<<KEYOUT1; 
    //PORTKEYOUT2->PIO_SODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_SODR = 1<<KEYOUT3;
    break;
    
  case 7 :
    KEY_tmp = KEY << 1;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY_tmp++;  
    KEY_tmp <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY_tmp++;  
    KEY = KEY_tmp;

    keyboard_state++;
    PORTKEYOUT1->PIO_CODR = 1<<KEYOUT1; 
    PORTKEYOUT2->PIO_CODR = 1<<KEYOUT2; 
    PORTKEYOUT3->PIO_CODR = 1<<KEYOUT3;
    break;
    
  case 8 :
    // get last key
    KEY_tmp = KEY << 1;
    if (PORTKEY25->PIO_PDSR & (1<<KEY25)) KEY_tmp++;  
  
    KEY = KEY_tmp^0x1FFFFFF; // invert value
    keyboard_state = 0;
    MODE_MENU = KEY_MENU();
    
    if (KEY == KEY_old) break; 

    if (!MODE_MENU){ // menu          
      KEY_tmp = KEY;
      KEY_ROWH_tmp = 0;
      KEY_ROWL_tmp = 0;
      KEY_ROWL1_tmp = 0;
      KEY_ROWL2_tmp = 0;
      KEY_ROW3_tmp = 0;

      
      if (KEY_tmp & 1<<2)  KEY_ROWH_tmp = 1; 
      if (KEY_tmp & 1<<8)  KEY_ROWH_tmp = 2; 
      if (KEY_tmp & 1<<14) KEY_ROWH_tmp = 3; 
      if (KEY_tmp & 1<<1)  KEY_ROWH_tmp = 4;
      if (KEY_tmp & 1<<7)  KEY_ROWH_tmp = 5;
      if (KEY_tmp & 1<<13) KEY_ROWH_tmp = 6;
      if (KEY_tmp & 1<<22) KEY_ROWH_tmp = 7;
      if (KEY_tmp & 1<<21) KEY_ROWH_tmp = 8;
      if (KEY_tmp & 1<<24) KEY_ROWH_tmp = 9;
      if (KEY_tmp & 1<<15) KEY_ROWH_tmp = 10;
      
      if (KEY_tmp & 1<<5)  KEY_ROWL1_tmp = 1;
      if (KEY_tmp & 1<<11) KEY_ROWL1_tmp = 2;
      if (KEY_tmp & 1<<17) KEY_ROWL1_tmp = 3;
      if (KEY_tmp & 1<<20) KEY_ROWL1_tmp = 4;
      if (KEY_tmp & 1<<23) KEY_ROWL1_tmp = 5;
      if (KEY_tmp & 1<<4)  KEY_ROWL1_tmp = 6;
      if (KEY_tmp & 1<<10) KEY_ROWL1_tmp = 7;
      if (KEY_tmp & 1<<16) KEY_ROWL1_tmp = 8;
      if (KEY_tmp & 1<<19) KEY_ROWL1_tmp = 9;
      
      if (KEY_tmp & 1<<9)  KEY_ROWL2_tmp = 1;
      if (KEY_tmp & 1<<3)  KEY_ROWL2_tmp = 2;
      if (KEY_tmp & 1<<12) KEY_ROWL2_tmp = 3;
      if (KEY_tmp & 1<<6)  KEY_ROWL2_tmp = 4;
    
       KEY_ROWL_tmp = KEY_ROWL2_tmp? KEY_ROWL2_tmp + 9 : KEY_ROWL1_tmp;

      if (KEY_tmp & 1<<18) KEY_ROW3_tmp = 1;
      if (KEY_tmp & 1<<0)  KEY_ROW3_tmp += 2;

      if ((KEY_ROW3_tmp == 2) && (KEY_ROWH_tmp > 0)) { // load conf
        led1_time = 300; // blink
        load_conf(KEY_ROWH_tmp);
      }
      else if ((KEY_ROW3_tmp == 1) && (KEY_ROWH_tmp > 0)) { // save conf
       led1_time = 300; // blink
       save_conf(KEY_ROWH_tmp);
      }        
      else if ((KEY_ROW3_tmp > 0) && (KEY_ROWL_tmp > 0)) { // option selection
        switch (KEY_ROWL_tmp) {
          case 1:
            LFO3_mode = 0;
          break;
          case 2:
            LFO3_mode = 1;
          break;
          case 3:
            LFO3_mode = 2;
          break;
          case 4:
            LFO3_mode = 3;
          break;
          case 5:
            LFO3_mode = 4;
          break;
          case 6:
            GATE_mode = 0;
          break;
          case 7:
            GATE_mode = 3;
          break;
          case 8:
            GATE_mode = 1;
          break;          
          case 9:
            ADSR_mode = 0;
          break;
          case 10:
            ADSR_mode = 1;
          break;
          case 11:
            audio_out_mode = 0;
          break;
          case 12:
            audio_out_mode = 1;
          break;
          case 13:
            audio_out_mode = 2;
          break;
        }
      } 

      noInterrupts(); // no audio interuption when updating the modulation sources 

      if ((KEY_ROWL_tmp >  0) && (KEY_ROWH_tmp > 0)) { // 1 inter sur la rangé du haut + 1 inter sur la rangé du bas
        //modulation asignation
        modulation_index[KEY_ROWL_tmp-1]=KEY_ROWH_tmp-1;
      }
      else if ((KEY_ROWL1_tmp > 0) && (KEY_ROWL2_tmp > 0)) { // modulation type
        index1 = (KEY_ROWL1_tmp-1) * 2;
        index2 = index1 + 1;
        switch (KEY_ROWL2_tmp) {
          case 1:
            modulation_type_all = BitClear(BitClear(modulation_type_all, index1), index2);
          break;
          case 2:
            modulation_type_all = BitClear(BitSet(  modulation_type_all, index1), index2);
          break;
          case 3:
            modulation_type_all = BitSet(  BitClear(modulation_type_all, index1), index2);
          break;
          case 4:
            modulation_type_all = BitSet(  BitSet(  modulation_type_all, index1), index2);
          break;
        }
      }
      // Cas particulié
      // automodulation WS
      if ((KEY_ROWL1_tmp >  0) && (modulation_index[KEY_ROWL1_tmp-1] == mod_1) ) {
        modulation_index[KEY_ROWL1_tmp-1] = ((KEY_ROWL1_tmp-1) / 3); // si on a une modulation direct : on la vire. si besion, elle sera remise la ligne suivante
      }
      if( (KEY_ROWL1_tmp >  0) && (modulation_index[KEY_ROWL1_tmp-1] == ((KEY_ROWL1_tmp-1) / 3)) && (((modulation_type_all >> (2*(KEY_ROWL1_tmp-1)) ) & 3) == 3) ) {// mod key // automod // mod type == WS
        modulation_index[KEY_ROWL1_tmp-1] = mod_1; // auto modulation WS : on remplace l'automodulation par une modulation direct      
      }
      // automodulation XY
      if ((KEY_ROWL2_tmp >  0) && (KEY_ROWH_tmp >  0) && (KEY_ROWH_tmp - 1 == mod_XY) ) // automo joystick
        modulation_index[KEY_ROWL_tmp-1] = mod_1; 
      if ((KEY_ROWL_tmp >  0) && (modulation_index[KEY_ROWL_tmp-1] == mod_LR)) { // modulation sur l'audio, on choisis L, R ou LR selon la destination de la mod
        switch(KEY_ROWL_tmp) {
          case 1: // osc1
            modulation_index[0] = mod_L;       // modulation LR sur OSC 1, on la passe en mod_L
            break;
          case 2:
            modulation_index[1] = mod_L;       // modulation LR sur OSC 1, on la passe en mod_L
            break;
          case 3:
            modulation_index[2] = mod_L;       // modulation LR sur OSC 1, on la passe en mod_L
            break;
          case 7: // osc3
            modulation_index[6] = mod_R;       // modulation LR sur OSC 3, on la passe en mod_L
            break;
          case 8:
            modulation_index[7] = mod_R;       // modulation LR sur OSC 3, on la passe en mod_L
            break;
          case 9:
            modulation_index[8] = mod_R;      // modulation LR sur OSC 3, on la passe en mod_R
            break;
         case 10:
            modulation_index[9] = mod_L;      // modulation LR sur X-, on la passe en mod_L
            break;
         case 11:
            modulation_index[10] = mod_R;      // modulation LR sur X+ 1, on la passe en mod_R
            break;
         case 12:
            modulation_index[11] = mod_L;      // modulation LR sur Y-, on la passe en mod_L
            break;
         case 13:
            modulation_index[12] = mod_R;      // modulation LR sur Y+, on la passe en mod_R
            break;
        }
      }
      interrupts();
      
      if ((KEY_ROWL_tmp == 0) && (KEY_ROWH_tmp == 0) && (KEY_ROW3_tmp == 0)) { // pas de key : on efface toutes les leds
        led_VCO1_off();
        led_VCO2_off();
        led_VCO3_off();
        masque_led = 0xFFFFFFFF; 
        blink_led = 0;
        led1_time = 0;
      }
      else if ((KEY_ROWL_tmp >  0) && (KEY_ROW3_tmp ==  0)) { // modulation visualisation 
        masque_led = 0;
        tmp = KEY_ROWL1_tmp? KEY_ROWL1_tmp : KEY_ROWL_tmp; // L1 prioritaire sur L2 : en cas de changement de type de modulation, on reste sur L1
        test_module = modulation_index[tmp - 1];
        
        blink_led = 1+((modulation_type_all >> (2*(tmp-1))) & 0b11); // clignottement de la led MIDI en fct du type de modulation
        
        if ((KEY_old ==  0) || (KEY_ROWL2_tmp > 0) ) {
          blink_time = 1; // reset le timer si on appuie sur une touche de modulation_type
          led1_time = 0;
        }
        if (test_module == mod_1) { // automodulation
           if (KEY_ROWL1_tmp > 0) { // automod sur les OSC
             test_module = (KEY_ROWL1_tmp - 1) / 3;
           }
           else { // automod sur le joystick 
             test_module = mod_XY;
           }     
        }
        else if ((test_module == mod_L)||(test_module == mod_R)) { // automodulation
          test_module = mod_LR;
        }

        switch (test_module) {
          case mod_VCO1:
            led_VCO1_on();
            led_VCO2_off();
            led_VCO3_off();
            and_led2 = 512;
            and_led3 = 512;
            and_led4 = 512;
          break;
          case mod_VCO2:
            led_VCO1_off();
            led_VCO2_on();
            led_VCO3_off();
            and_led2 = 512;
            and_led3 = 512;
            and_led4 = 512;
          break;          
          case mod_VCO3:
            led_VCO1_off();
            led_VCO2_off();
            led_VCO3_on();
            and_led2 = 512;
            and_led3 = 512;
            and_led4 = 512;
          break;          
          case mod_LFO1:
            led_VCO1_off();
            led_VCO2_off();
            led_VCO3_off();
            and_led2 = 511;
            and_led3 = 512;
            and_led4 = 512;
          break;          
          case mod_LFO2:
            led_VCO1_off();
            led_VCO2_off();
            led_VCO3_off();
            and_led2 = 512;
            and_led3 = 511;
            and_led4 = 512;
          break;          
          case mod_LFO3:
            led_VCO1_off();
            led_VCO2_off();
            led_VCO3_off();
            and_led2 = 512;
            and_led3 = 512;
            and_led4 = 511;
          break;                 
          case mod_ADSR:
            led_VCO1_off();
            led_VCO2_on();
            led_VCO3_on();
            and_led2 = 511;
            and_led3 = 511;
            and_led4 = 511;
          break;          
          case mod_EXT1:
            led_VCO1_on();
            led_VCO2_off();
            led_VCO3_on();
            and_led2 = 511;
            and_led3 = 511;
            and_led4 = 511;
          break;          
          case mod_LR:
            led_VCO1_on();
            led_VCO2_on();
            led_VCO3_off();
            and_led2 = 511;
            and_led3 = 511;
            and_led4 = 511;
          break;          
          case mod_XY:
            led_VCO1_on();
            led_VCO2_on();
            led_VCO3_on();
            and_led2 = 512;
            and_led3 = 511;
            and_led4 = 511;
          break;  
            case mod_L:
            led_VCO1_on();
            led_VCO2_on();
            led_VCO3_off();
            and_led2 = 511;
            and_led3 = 511;
            and_led4 = 511;
          break;          
          case mod_R:
            led_VCO1_on();
            led_VCO2_on();
            led_VCO3_off();
            and_led2 = 511;
            and_led3 = 511;
            and_led4 = 511;
          break;                  
        }
      }
      else if (KEY_ROW3_tmp >  0) {
        masque_led = 0;
        switch (LFO3_mode) {
          case 0: 
            led_VCO1_off();
            led_VCO2_off();
            led_VCO3_off();
            break;
          case 1: 
            led_VCO1_off();
            led_VCO2_off();
            led_VCO3_on();
            break;
          case 2: 
            led_VCO1_off();
            led_VCO2_on();
            led_VCO3_off();
            break;
          case 3: 
            led_VCO1_on();
            led_VCO2_off();
            led_VCO3_off();
            break;
          case 4: 
            led_VCO1_on();
            led_VCO2_on();
            led_VCO3_on();
            break;
        }
        switch (GATE_mode) {
          case 0: 
            and_led2 = 512;
          break;
          case 1: 
            and_led2 = 511;
          break;
          case 2: 
          case 3: 
            and_led2 = 150;
          break;
        }
        if (ADSR_mode) and_led3 = 511; else and_led3 = 512;
        switch (audio_out_mode) {
          case 0: 
            and_led4 = 512;
          break;
          case 1: 
            and_led4 = 150;
          break;
          case 2: 
            and_led4 = 511;
          break;
        }
      }
      #ifndef nosave
        flash_lock_bit = 1;
      #endif
    }
    else { // note keyboard Mode
      KEY_LOCAL_tmp = 0;
      KEY_tmp = KEY;
      if (KEY_tmp & 1<<5)  KEY_LOCAL_tmp = 52;
      if (KEY_tmp & 1<<2)  KEY_LOCAL_tmp = 53;
      if (KEY_tmp & 1<<11) KEY_LOCAL_tmp = 54;
      if (KEY_tmp & 1<<8)  KEY_LOCAL_tmp = 55;
      if (KEY_tmp & 1<<17) KEY_LOCAL_tmp = 56;
      if (KEY_tmp & 1<<20) KEY_LOCAL_tmp = 57;
      if (KEY_tmp & 1<<14) KEY_LOCAL_tmp = 58;
      if (KEY_tmp & 1<<23) KEY_LOCAL_tmp = 59;
      if (KEY_tmp & 1<<1)  KEY_LOCAL_tmp = 60;
      if (KEY_tmp & 1<<4)  KEY_LOCAL_tmp = 61;
      if (KEY_tmp & 1<<7)  KEY_LOCAL_tmp = 62;
      if (KEY_tmp & 1<<10) KEY_LOCAL_tmp = 63;
      if (KEY_tmp & 1<<16) KEY_LOCAL_tmp = 64;
      if (KEY_tmp & 1<<13) KEY_LOCAL_tmp = 65;
      if (KEY_tmp & 1<<19) KEY_LOCAL_tmp = 66;
      if (KEY_tmp & 1<<22) KEY_LOCAL_tmp = 67;
      if (KEY_tmp & 1<<9)  KEY_LOCAL_tmp = 68;
      if (KEY_tmp & 1<<3)  KEY_LOCAL_tmp = 69;
      if (KEY_tmp & 1<<21) KEY_LOCAL_tmp = 70;
      if (KEY_tmp & 1<<12) KEY_LOCAL_tmp = 71;
      if (KEY_tmp & 1<<24) KEY_LOCAL_tmp = 72;
      if (KEY_tmp & 1<<6)  KEY_LOCAL_tmp = 73;
      if (KEY_tmp & 1<<15) KEY_LOCAL_tmp = 74;
      if (KEY_tmp & 1<<18) KEY_LOCAL_tmp = 75;
      if (KEY_tmp & 1<<0)  KEY_LOCAL_tmp = 76;    
      if (KEY_LOCAL_tmp) {
        KEY_LOCAL_goal = KEY_LOCAL_tmp  << (2+18);
      }
      NOTE_ON = KEY_LOCAL_tmp;
    }
    
    KEY_old = KEY;
    break;
  }
}

inline void PORTAMENTO_update() {
  uint32_t tmp32;
  tmp32 = adc_value16[PORTAMENTO_VALUE];
  portamento = (table_CV2freq[0x350-(tmp32>>7)]);
}

inline void PORTAMENTO() {
  uint32_t tmp;
  tmp = KEY_LOCAL;
  KEY_LOCAL = tmp + m_s32xs32_s32H(KEY_LOCAL_goal - tmp, portamento);
}

