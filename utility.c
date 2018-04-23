#include "utility.h"

void delay(uint16_t del) {
  uint16_t dell = del * 10;
  while(dell > 0)
    dell --;
}