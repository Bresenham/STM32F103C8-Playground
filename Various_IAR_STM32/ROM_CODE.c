#include "ROM_CODE.h"

#define SERIAL_NUM_IDX          1
#define CRC_IDX                 7

#define CRC_SHIFT_REG_XOR_1_BIT 0
#define CRC_SHIFT_REG_XOR_2_BIT 3
#define CRC_SHIFT_REG_XOR_3_BIT 4

#define CRC_SHIFT_MASK          0x73

#define DS18B20_FAMILY_CODE     0x28        

uint8_t recalculate_crc(const uint64_t value) {
  
  uint8_t shift_register = 0;
  
  /* CRC Is Calculated From the Least Significant 56 Bits Of The ROM Code */
  for(uint8_t i = 0; i < 56; i++) {
    
    const uint8_t input_bit = (value >> i) & 1;
    
    const uint8_t first_xor = input_bit ^ ( (shift_register >> CRC_SHIFT_REG_XOR_1_BIT) & 1 );
    
    const uint8_t second_xor = first_xor ^ ( (shift_register >> CRC_SHIFT_REG_XOR_2_BIT) & 1 );
    
    const uint8_t third_xor = first_xor ^ ( (shift_register >> CRC_SHIFT_REG_XOR_3_BIT) & 1 );
    
    shift_register = shift_register >> 1;
    shift_register = shift_register & CRC_SHIFT_MASK;
    shift_register |= ( (first_xor << 7) | (third_xor << 3) | (second_xor << 2) );
  }
  
  return shift_register;
}

void update(struct ROM_CODE *self, const uint64_t value) {
  self->value = value;
  
  const uint8_t recalculated_crc = recalculate_crc(value);
  
  self->is_valid = ( recalculated_crc == *self->crc ) && ( *self->family_code == DS18B20_FAMILY_CODE );
}

void init_rom_code(struct ROM_CODE *self) {
  self->is_valid = false;
  self->family_code = (uint8_t*)(&self->value);
  self->serial_number = &( (uint8_t*)(&self->value) )[SERIAL_NUM_IDX];
  self->crc = &( (uint8_t*)(&self->value) )[CRC_IDX];
  
  self->update = &update;
}