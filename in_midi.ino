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

// RX MIDI
// PD5 : RXD3, peripheric B, USART3
// Midi is 31.25 Kb/s
// chip is at 84 000 KHz
// CD = 168

uint32_t MIDI_status, MIDI_data1, MIDI_state;
uint32_t MIDI_PITCHWHEEL;
uint32_t use_midi_pitchwheel;
uint32_t MIDI_fader_LSB[33];

// MIDI_state :
//    0 : no status received
//    1 : 1 status received, waiting for data1
//    2 : data 1 received, waiting for data2

inline void init_midi() {
  // RX3D : PD5, peripherical B  
  REG_PMC_PCER0 = 1 << 20; //switch on uart3 clock
  USART3->US_MR = 0x000008C0; // misc configuration
  USART3->US_BRGR = 168; // baude rate
  PIOD->PIO_ABSR |= 1<<5; // peripherical B for midi RX pin 
  USART3->US_CR = 0x00000010; // enable receiver
  MIDI_state = 0;
  use_midi_pitchwheel = 0;
}

inline void MIDI_in() { // the main loop is faster than midi message, no need to get midi data in interuption
  uint32_t midi_data, MIDI_data2;
  
  if (USART3->US_CSR  & (1<<0)) { // RXRDY
    midi_data = (USART3->US_RHR) & 0xFF;
    led1_time = 150; // small blink
    
    if (midi_data & 0x80) { // test for status
      if ((midi_data & 0xF0) != 0xF0) { // remove real time messages
        MIDI_status = midi_data;
        MIDI_state = 1;
      }
    }
    else { // data
      switch (MIDI_state) {
      case 0 :
        // data without any status : discard
        break;
      case 1:
        MIDI_state = 2;
        MIDI_data1 = midi_data;
        #ifdef MIDI_omni
        if ((MIDI_status & 0xF0) == 0b11000000)  // progam change OMNI
        #endif
        #ifndef MIDI_omni        
        if ((MIDI_status) == 0b11000000 + MIDI_channel)  // progam change
        #endif
        {
          MIDI_state = 1;
          if ( (midi_data < 10) && (flash_lock_bit == 0) && (efc_perform_command_is_ready(EFC1)) ) {
          //if (midi_data < 10) {
            load_conf(1+midi_data); // load a memory
          }
        }
        break;
      case 2:
        MIDI_data2 = midi_data;
        MIDI_state = 1; // go back waiting for data 1 (running status)

        // we have a full midi message
        #ifdef MIDI_omni
        if(((MIDI_status & 0xF0) == 0x90) && (MIDI_data2 != 0) ) // note on, OMNI
        #endif
        #ifndef MIDI_omni
        if(((MIDI_status) == 0x90 + MIDI_channel) && (MIDI_data2 != 0) ) // note on, specific midi chanel
        #endif        
        {  
          MIDI_gate = 1;
          MIDI_pitch = MIDI_data1;
          
          KEY_LOCAL_goal = (16+MIDI_pitch) << (2+18);
        }
        #ifdef MIDI_omni
        else if((((MIDI_status & 0xF0) == 0x90) && (MIDI_data2 == 0)) || ((MIDI_status & 0xF0) == 0x80))  // note off
        #endif
        #ifndef MIDI_omni        
        else if((((MIDI_status) == 0x90 + MIDI_channel) && (MIDI_data2 == 0)) || ((MIDI_status) == 0x80  + MIDI_channel))  // note off
        #endif
        {
          if (MIDI_pitch == MIDI_data1){
            MIDI_gate = 0;
          }
        }
        #ifndef EXT_ANA
        #ifdef MIDI_omni
        else if ((MIDI_status & 0xF0) == 0b11100000) 
        #endif
        #ifndef MIDI_omni   
        else if ((MIDI_status) == 0b11100000 + MIDI_channel) // cannal 0
        #endif
        { // Pitch bend
          MIDI_PITCHWHEEL = (MIDI_data1 << 2) + (MIDI_data2 << 9); // 16 bits
          use_midi_pitchwheel = 1;
        }
        #endif
        
        
        #ifdef MIDI_omni
        else if ((MIDI_status & 0xF0) == 0b10110000)  // CC omni
        #endif
        #ifndef MIDI_omni       
        else if ((MIDI_status) == 0b10110000 + MIDI_channel)  // CC cannal n
        #endif        
        {
          if (MIDI_data1 < 32) { // bit poids fort
            if (MIDI_data1 < 30) {
              MIDI_fader[MIDI_data1] = (MIDI_data2<<9) + MIDI_fader_LSB[MIDI_data1];
            }
            else if (MIDI_data1  == 30) { // joystick : since modulation should be + and -, on doit faire autrement
              MIDI_joystick_X = -((MIDI_data2<<9) + MIDI_fader_LSB[MIDI_data1]) + (1<<15); // on symetrise
            }
            else if (MIDI_data1  == 31) { // joystick 
              MIDI_joystick_Y = -((MIDI_data2<<9) + MIDI_fader_LSB[MIDI_data1]) + (1<<15);
            }
          }
          else {
            MIDI_data1 -= 32;
            if (MIDI_data1 <= 32)
              MIDI_fader_LSB[MIDI_data1] = MIDI_data2 << 2;
          }
        }
        break;
      }
    }
  }
}
