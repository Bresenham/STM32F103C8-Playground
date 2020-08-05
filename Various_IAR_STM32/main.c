#include "CMSIS/stm32f1xx.h"
#include <stdbool.h>
#include <stdint.h>
#include "DS18B20.h"
#include "GlobalHelperFunctions.h"

void init_led();
void init_timer();
void led_on();
void led_off();

struct DS18B20 ds18b20;

static bool is_led_on = true;
uint32_t sysclk_in_mhz;

void init_led() {
  /* Enable Clock for GPIO Port C */
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

  /* GPIO Port C Pin 13 Push-Pull */
  GPIOC->CRH &= ~( GPIO_CRH_CNF13_0 | GPIO_CRH_CNF13_1 );

  /* GPIO Port C Pin 13 50MHz Output Mode */
  GPIOC->CRH |= ( GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1 );
}

void led_on() {
  GPIOC->ODR &= (uint16_t)( ~GPIO_ODR_ODR13 );
}

void led_off() {
  GPIOC->ODR |= GPIO_ODR_ODR13;
}

int main(void) {
  
  update_sysclk_to_128mhz();
  
  sysclk_in_mhz = SystemCoreClock / 1000000;
  
  init_led();
  
  initDS18B20(&ds18b20);

  ds18b20.readDeviceID(&ds18b20);
        
  while(true) {
    // wait(5);
    // GPIOC->ODR ^= GPIO_ODR_ODR13;
    // wait(5);
    // GPIOC->ODR ^= GPIO_ODR_ODR13;
  }
}
