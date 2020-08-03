#ifndef GLOBAL_HELPER_FUNCTIONS_H_
#define GLOBAL_HELPER_FUNCTIONS_H_

#include <stdint.h>

extern uint32_t sysclk_in_mhz;

void wait(const uint32_t us);
void update_sysclk_to_128mhz();

#endif /* GLOBAL_HELPER_FUNCTIONS_H_ */