#include "CMSIS/stm32f1xx.h"

#include "DS18B20.h"
#include "GlobalHelperFunctions.h"
#include "ROM_CODE.h"

#define MATCH_ROM_CMD	        0x33
#define ROM_CODE_LENGTH_IN_BITS 64

void setReadMode() {
  
  /* Set Input Mode */
  GPIOC->CRH &= ~( GPIO_CRH_MODE14_0 | GPIO_CRH_MODE14_1 );
  
  /* Set Pull-Up According to Table 20 */
  GPIOC->ODR |= GPIO_ODR_ODR14;
  
  /* Set Input with Pull-Up/Pull-Down */
  GPIOC->CRH |= GPIO_CRH_CNF14_1;
  GPIOC->CRH &= ~GPIO_CRH_CNF14_0;
}

void setWriteMode() {
  
  /* Set Pin High */
  GPIOC->ODR |= GPIO_ODR_ODR14;
  
  /* Set Push-Pull Output */
  GPIOC->CRH &= ~GPIO_CRH_CNF14;
  
  /* Set 50MHz Output-Mode */
  GPIOC->CRH |= ( GPIO_CRH_MODE14_0 | GPIO_CRH_MODE14_1 );
}

void writeOne() {
  
  TIM2->CNT = 0;
  
  /* Begin Write 1 Slot By Pulling 1-Wire Line Low */
  GPIOC->ODR &= ~GPIO_ODR_ODR14;
  
  /* Wait 15탎 */
  while( TIM2->CNT < 1080 ) { ; }
  
  /* Release 1-Wire Line */
  GPIOC->ODR |= GPIO_ODR_ODR14;
  
  /* Write Slot 60탎 Minimum Duration */
  while( TIM2->CNT < 4320 ) { ; }
  
  TIM2->CNT = 0;
}

void writeZero() {
  
  TIM2->CNT = 0;
  
  /* Begin Write 0 Slot By Pulling 1-Wire Line Low */
  GPIOC->ODR &= ~GPIO_ODR_ODR14;
  
  /* Wait 60탎 */
  while( TIM2->CNT < 4320 ) { ; }
  
  /* Release 1-Wire Line */
  GPIOC->ODR |= GPIO_ODR_ODR14;
  
  TIM2->CNT = 0;
}

uint8_t readBit() {
  
  setWriteMode();
  
  TIM2->CNT = 0;
  
  /* Begin Read Slot By Pulling 1-Wire Line Low */
  GPIOC->ODR &= ~GPIO_ODR_ODR14;
  
  /* Wait Short Time */
  while( TIM2->CNT < 72 ) { ; }
  
  /* Release 1-Wire Line */
  GPIOC->ODR |= GPIO_ODR_ODR14;
  
  setReadMode();
  
  /* Sample At The End Of 15탎 Read Slot */
  while( TIM2->CNT < 864 ) { ; }
  
  const uint8_t bit_val = (GPIOC->IDR & GPIO_IDR_IDR14) >> 14;
  
  if(bit_val == 1) {
    GPIOC->ODR |= GPIO_ODR_ODR15; 
  } else if(bit_val == 0) {
    GPIOC->ODR &= ~GPIO_ODR_ODR15;
  }
  
  /* Read Time Slot Must Be At Least 60탎 Long */
  while( TIM2->CNT < 4320 ) { ; }
  
  TIM2->CNT = 0;
  
  return bit_val;
}

bool startCommunication() {
  
  /* Enable Timer 2 */
  TIM2->CR1 |= TIM_CR1_CEN;
  
  setWriteMode();
  
  /* Begin Master Tx Reset Pulse By Pulling 1-Wire Line Low */
  GPIOC->ODR &= ~GPIO_ODR_ODR14;
  
  /* Wait 480탎 */
  TIM2->CNT = 0;
  while( TIM2->CNT < 34560 ) { ; }
  
  /* End Master Tx Reset Pulse By Releasing 1-Wire Line */
  TIM2->CNT = 0;
  GPIOC->ODR |= GPIO_ODR_ODR14;
  
  setReadMode();
  
  /* Let DS18B20 Delay Time Elapse */
  while( TIM2->CNT < 4320 ) { ; }
  TIM2->CNT = 0;
  
  /* 1-Wire Should Definitely Be Low Now */
  const bool is_low = !( ( GPIOC->IDR & GPIO_IDR_IDR14 ) >> 14 );
  
  /* Rx Pulse Of DS18B20 Needs To Be At Least 480탎 long */
  while( TIM2->CNT < 30240 ) { ; }
  
  /* Disable Timer 2 */
  TIM2->CR1 &= ~TIM_CR1_CEN;
  
  return is_low;
}

void sendByte(const uint8_t byte) {
  
  setWriteMode();
  
  /* Enable Timer 2 */
  TIM2->CR1 |= TIM_CR1_CEN;
  
  for(uint8_t bit_pos = 0; bit_pos <= 7; bit_pos++) {
    
    if( byte & (1 << bit_pos) ) {
      writeOne();
    } else {
      writeZero();
    }
    
    /* Recovery Time Between Write Slots */
    while( TIM2->CNT < 72 ) { ; }
  }
  
  /* Disable Timer 2 */
  TIM2->CR1 &= ~TIM_CR1_CEN;
}

uint64_t readData(const uint8_t amountOfBits) {
  
  uint64_t ret_data = 0;
  
  /* Enable Timer 2 */
  TIM2->CR1 |= TIM_CR1_CEN;
  
  for(uint8_t bit_pos = 0; bit_pos < amountOfBits; bit_pos++) {
    
    const uint64_t read_bit = readBit();
    ret_data |= (read_bit << bit_pos);
    
    /* Recovery Time Between Read Slots */
    while( TIM2->CNT < 72 ) { ; }
  }
  
  /* Disable Timer 2 */
  TIM2->CR1 &= ~TIM_CR1_CEN;
  
  return ret_data;
}

void readDeviceID(struct DS18B20 *self) {
  
  const bool successfull = startCommunication();
  if(successfull) {
    sendByte(MATCH_ROM_CMD);
    
    const uint64_t ret_val = readData(ROM_CODE_LENGTH_IN_BITS);
    
    self->rom_code.update(&self->rom_code, ret_val);
  }
}

void initDS18B20(struct DS18B20 *self) {
  
  init_rom_code(&self->rom_code);
  
  self->readDeviceID = &readDeviceID;
  
  /* Enable Clock for Timer 2 */
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  
  /* Set Timer 2 Prescaler Value */
  TIM2->PSC = 0;
  
  /* Set Timer 2 Auto Reload Value */
  TIM2->ARR = 0xFFFF;
  
  /* Disable Update */
  TIM2->CR1 |= TIM_CR1_UDIS;
}
