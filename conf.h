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

//you can adjust few option bellow:

// comment to speed up programation time thanks to smaller cosinus table
#define quality 

// sound caractere.  
#define DAC_clear 1 // 1 for "clear", 0 for "fat"

// more distortion after the MIX  
#define double_disto

// ADSR : Attack curve
// this variable can be in betwwen this 2 exterm value:
// 0X47000000 : for slow curve
// 0x7FFFFFFF : for hard curve
// default is 0x7FFFFFFF
#define ADSR_Goal_value 0x5FFFFFFF

// EXT : analogue // midi // auto
// ana : use only the 3 analog input (-6 / 6V)
// midi : use only midi pitch wheel / mod wheel (cc1) / breath controler (cc2)
// auto : use ana until a midi parametter is send
#define EXT_AUTO
//#define EXT_ANA
//#define EXT_MIDI

// do not save current preset, and load default config on every startup
//#define nosave

// output LFO / ADSR analog value on the "LFO OUT" pad on the PCB
// analogue value range from about 0.5V to 2.8V 
#define LFO_out

// output syncro (audio / data loop) for benchmarck purpose 
#define syncro_out

// output GATE on the dedicated pin
#define gate_out

// MIDI omni mode, or use only a specific chanel (0 to 15)
#define MIDI_omni 
//#define MIDI_channel 1

