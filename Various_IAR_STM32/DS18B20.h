#ifndef DS18B20_H_
#define DS18B20_H_

#include <stdint.h>
#include <stdbool.h>

#include "ROM_CODE.h"

struct DS18B20 {
  struct ROM_CODE rom_code;

  void (*readDeviceID)(struct DS18B20*);
};

void initDS18B20(struct DS18B20 *self);

#endif /* DS18B20_H_ */