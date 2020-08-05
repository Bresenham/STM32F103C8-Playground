#ifndef ROM_CODE_H
#define ROM_CODE_H

#include <stdint.h>
#include <stdbool.h>

struct ROM_CODE {
  
  bool is_valid;
  
  uint64_t value;
  
  uint8_t *family_code;
  uint8_t *serial_number;
  uint8_t *crc;
  
  void (*update)(struct ROM_CODE*, const uint64_t value);
};

void init_rom_code(struct ROM_CODE *self);

#endif /* ROM_CODE_H */