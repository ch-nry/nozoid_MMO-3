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

inline void init_save() {  
  
  DueFlashStorage();
  flash_lock_bit = 0;
  
  if (!(PORTKEY25->PIO_PDSR & (1<<KEY25)) || (read(0)==0xFF)) { // errase assignations and conf
    load_random_config();
    save_conf(1);
    load_random_config();
    save_conf(2);
    load_random_config();
    save_conf(3);
    load_random_config();
    save_conf(4);
    load_random_config();
    save_conf(5);
    load_random_config();
    save_conf(6);
    load_random_config();
    save_conf(7);
    load_random_config();
    save_conf(8);
    load_random_config();
    save_conf(9);
    load_random_config();
    save_conf(10);
    load_random_config();
    
    flash_lock_bit = 1; // on sauve la conf
  }
  else
  #ifdef nosave
    load_random_config();
  #endif
  #ifndef nosave
    load_conf0(0);
  #endif
}
  
inline void load_random_config() {
  // default config
  
  modulation_index[index_VCO1_MOD1] = 1+(random32()%5);
  modulation_index[index_VCO1_MOD2] = 1+(random32()%5);
  modulation_index[index_VCO1_MOD3] = 1+(random32()%5);
  modulation_index[index_VCO2_MOD1] = 2+(random32()%4);
  modulation_index[index_VCO2_MOD2] = 2+(random32()%4);
  modulation_index[index_VCO2_MOD3] = 2+(random32()%4);
  modulation_index[index_VCO3_MOD1] = 3+(random32()%3);
  modulation_index[index_VCO3_MOD2] = 3+(random32()%3);
  modulation_index[index_VCO3_MOD3] = 3+(random32()%3);

  modulation_index[index_Xm] = mod_LFO1;
  modulation_index[index_Xp] = mod_LFO2;
  modulation_index[index_Ym] = mod_LFO3;
  modulation_index[index_Yp] = random32()%3;

  modulation_type_all = random32() & 0x3FFFF; //18 bits

  GATE_mode = 0; // 2 bit 
  ADSR_mode = 0; // 1 bit
  audio_out_mode = 1; // 2 bit 
  
  LFO3_mode = (random32()%5);
}

inline void save_conf0() {
  uint32_t data;
  
  switch (flash_lock_bit) {
    case 1: // il faut sauver qqq chose
      data = modulation_index[index_VCO1_MOD1] << 4;
      data += modulation_index[index_VCO1_MOD2];
      if ((uint8_t)data != read( 0))
        write_noblocking(0,(uint8_t)data);
      flash_lock_bit = 2;
    break;
    case 2:
      write_end(0);
      data =  modulation_index[index_VCO1_MOD3] << 4;
      data += modulation_index[index_VCO2_MOD1];
      if ((uint8_t)data != read( 0x100 ))
        write_noblocking(0x100,(uint8_t)data);
      flash_lock_bit = 3;
    break;
    case 3:
      write_end(0x100);
      data =  modulation_index[index_VCO2_MOD2] << 4;
      data += modulation_index[index_VCO2_MOD3];
      if ((uint8_t)data != read( 0x200 ))
        write_noblocking(0x200,(uint8_t)data);
      flash_lock_bit = 4;
    break;
    case 4:
      write_end(0x200);
      data =  modulation_index[index_VCO3_MOD1] << 4;
      data += modulation_index[index_VCO3_MOD2];
      if ((uint8_t)data != read( 0x300 ))
        write_noblocking(0x300,(uint8_t)data);
      flash_lock_bit = 5;
    break;    
    case 5:
      write_end(0x300);
      data =  modulation_index[index_VCO3_MOD3] << 4;
      data += modulation_index[index_Xm];
      if ((uint8_t)data != read( 0x400 ))
        write_noblocking(0x400,(uint8_t)data);
      flash_lock_bit = 6;
    break;    
    case 6:
      write_end(0x400);
      data =  modulation_index[index_Xp] << 4;    
      data += modulation_index[index_Ym];
      if ((uint8_t)data != read( 0x500 ))
        write_noblocking(0x500,(uint8_t)data);
      flash_lock_bit = 7;
    break;    
    case 7:
      write_end(0x500);
      data =  modulation_index[index_Yp];
      data += LFO3_mode << 4;   
      if ((uint8_t)data != read( 0x600 ))
        write_noblocking(0x600,(uint8_t)data);
      flash_lock_bit = 8;
    break;    
    case 8:
      write_end(0x600);
      data = modulation_type_all & 0xFF;
      if ((uint8_t)data != read( 0x700 ))
        write_noblocking(0x700,(uint8_t)data);
      flash_lock_bit = 9;
    break;    
    case 9:
      write_end(0x700);
      data = (modulation_type_all >> 8) & 0xFF;
      if ((uint8_t)data != read( 0x800 ))
        write_noblocking(0x800,(uint8_t)data);
      flash_lock_bit = 10;
    break;    
    case 10:
      write_end(0x800);
      data = (modulation_type_all >> 11) & 0b01100000;
      data += GATE_mode  << 3;
      data += ADSR_mode << 2;
      data += audio_out_mode << 0;
      if ((uint8_t)data != read( 0x900 ))
        write_noblocking(0x900,(uint8_t)data);
      flash_lock_bit = 11;
    break;    
    case 11:
      write_end(0x900);
      flash_lock_bit = 0; // on a fini de sauver
    break;
  }
}


inline void load_conf0(uint32_t nb) { // mandatory to keep the arg, otherwise optimisation occure and flash is not read (since the compiler consider it as a constant)
  uint8_t byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9, byte10, byte11, byte12, byte13, byte14;
  uint32_t adresse;
  adresse = 256*(nb);
  
  byte1  = read( adresse   );
  byte2  = read( adresse+0x100 );
  byte3  = read( adresse+0x200 );
  byte4  = read( adresse+0x300 );
  byte5  = read( adresse+0x400 );
  byte6  = read( adresse+0x500 );
  byte7  = read( adresse+0x600 );
  byte8  = read( adresse+0x700 );
  byte9  = read( adresse+0x800 );
  byte10 = read( adresse+0x900 );

  modulation_index[index_VCO1_MOD1] = (byte1 >> 4) & 0x0F;
  modulation_index[index_VCO1_MOD2] = (byte1 >> 0) & 0x0F;
  modulation_index[index_VCO1_MOD3] = (byte2 >> 4) & 0x0F;
  modulation_index[index_VCO2_MOD1] = (byte2 >> 0) & 0x0F;
  modulation_index[index_VCO2_MOD2] = (byte3 >> 4) & 0x0F;
  modulation_index[index_VCO2_MOD3] = (byte3 >> 0) & 0x0F;
  modulation_index[index_VCO3_MOD1] = (byte4 >> 4) & 0x0F;
  modulation_index[index_VCO3_MOD2] = (byte4 >> 0) & 0x0F;
  modulation_index[index_VCO3_MOD3] = (byte5 >> 4) & 0x0F;
  modulation_index[index_Xm] = (byte5 >> 0) & 0x0F;
  modulation_index[index_Xp] = (byte6 >> 4) & 0x0F;
  modulation_index[index_Ym] = (byte6 >> 0) & 0x0F;
  modulation_index[index_Yp] = (byte7 >> 0) & 0x0F;
  LFO3_mode = (byte7 >> 4) & 0x0F;

  modulation_type_all = byte8 + (byte9 << 8) + ((byte10 & 0b01100000) << 11) ;
    
  GATE_mode = (byte10 >> 3) & 0x03;
  ADSR_mode = (byte10 >> 2) & 0x01;
  audio_out_mode = (byte10 >> 0) & 0x03;
  
  flash_lock_bit = 1; // To save this conf as conf 0
} 


/*
inline void load_default_config() {
  // default config
  
  modulation_index[index_VCO1_MOD1] = mod_VCO2; // 4 bit
  modulation_index[index_VCO1_MOD2] = mod_VCO3;
  modulation_index[index_VCO1_MOD3] = mod_LFO1;
  modulation_index[index_VCO2_MOD1] = mod_VCO1;
  modulation_index[index_VCO2_MOD2] = mod_VCO3;
  modulation_index[index_VCO2_MOD3] = mod_LFO2;
  modulation_index[index_VCO3_MOD1] = mod_VCO1;
  modulation_index[index_VCO3_MOD2] = mod_VCO2;
  modulation_index[index_VCO3_MOD3] = mod_XY;
  modulation_index[index_Xm] = mod_LFO1;
  modulation_index[index_Xp] = mod_LFO2;
  modulation_index[index_Ym] = mod_LFO3;
  modulation_index[index_Yp] = mod_1;

  modulation_type_all = 0b111001 + (0b111001<<6) + (0b111001 <<12); // 18 bit 

  GATE_mode = 0; // 2 bit 
  ADSR_mode = 0; // 1 bit
  audio_out_mode = 1; // 2 bit 
  LFO3_mode = 0;
}
*/


inline void save_conf(uint32_t nb) { 
  uint8_t all_data[10];
  
  all_data[0] =  modulation_index[index_VCO1_MOD1] << 4;
  all_data[0] += modulation_index[index_VCO1_MOD2];
  all_data[1] =  modulation_index[index_VCO1_MOD3] << 4;
  all_data[1] += modulation_index[index_VCO2_MOD1];
  all_data[2] =  modulation_index[index_VCO2_MOD2] << 4;
  all_data[2] += modulation_index[index_VCO2_MOD3];
  all_data[3] =  modulation_index[index_VCO3_MOD1] << 4;
  all_data[3] += modulation_index[index_VCO3_MOD2];
  all_data[4] =  modulation_index[index_VCO3_MOD3] << 4;
  all_data[4] += modulation_index[index_Xm];
  all_data[5] =  modulation_index[index_Xp] << 4;    
  all_data[5] += modulation_index[index_Ym];
  all_data[6] =  modulation_index[index_Yp];
  all_data[6] += LFO3_mode << 4;
  
  all_data[7] = modulation_type_all & 0xFF;
  all_data[8] = (modulation_type_all >> 8) & 0xFF;

  all_data[9] = (modulation_type_all >> 11) & 0b01100000;
  all_data[9] += GATE_mode  << 3;
  all_data[9] += ADSR_mode << 2;
  all_data[9] += audio_out_mode << 0;
  
  write( 256*(nb+10), all_data, 10);
}

inline void load_conf(uint32_t nb) {
  uint8_t byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9, byte10;
  uint32_t adresse;
  adresse = 256*(nb+10);

  byte1  = read( adresse   );
  byte2  = read( adresse+1 );
  byte3  = read( adresse+2 );
  byte4  = read( adresse+3 );
  byte5  = read( adresse+4 );
  byte6  = read( adresse+5 );
  byte7  = read( adresse+6 );
  byte8  = read( adresse+7 );
  byte9  = read( adresse+8 );
  byte10 = read( adresse+9 );

  modulation_index[index_VCO1_MOD1] = (byte1 >> 4) & 0x0F;
  modulation_index[index_VCO1_MOD2] = (byte1 >> 0) & 0x0F;
  modulation_index[index_VCO1_MOD3] = (byte2 >> 4) & 0x0F;
  modulation_index[index_VCO2_MOD1] = (byte2 >> 0) & 0x0F;
  modulation_index[index_VCO2_MOD2] = (byte3 >> 4) & 0x0F;
  modulation_index[index_VCO2_MOD3] = (byte3 >> 0) & 0x0F;
  modulation_index[index_VCO3_MOD1] = (byte4 >> 4) & 0x0F;
  modulation_index[index_VCO3_MOD2] = (byte4 >> 0) & 0x0F;
  modulation_index[index_VCO3_MOD3] = (byte5 >> 4) & 0x0F;
  modulation_index[index_Xm] = (byte5 >> 0) & 0x0F;
  modulation_index[index_Xp] = (byte6 >> 4) & 0x0F;
  modulation_index[index_Ym] = (byte6 >> 0) & 0x0F;
  modulation_index[index_Yp] = (byte7 >> 0) & 0x0F;
  LFO3_mode = (byte7 >> 4) & 0x0b111;
  modulation_type_all = byte8 + (byte9 << 8) + ((byte10 & 0b01100000) << 11) ;
  GATE_mode = (byte10 >> 3) & 0x03;
  ADSR_mode = (byte10 >> 2) & 0x01;
  audio_out_mode = (byte10 >> 0) & 0x03;
 
  flash_lock_bit = 1; // To save this conf as conf 0
}

