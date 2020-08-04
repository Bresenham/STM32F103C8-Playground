#include "CMSIS/stm32f1xx.h"
#include "DS18B20.h"

#include "GlobalHelperFunctions.h"

#define MATCH_ROM_CMD	0x55

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

  /* Set Open-Drain Output */
  GPIOC->CRH |= GPIO_CRH_CNF14_0;
  GPIOC->CRH &= ~GPIO_CRH_CNF14_1;

  /* Set 50MHz Output-Mode */
  GPIOC->CRH |= ( GPIO_CRH_MODE14_0 | GPIO_CRH_MODE14_1 );
}

void writeOne() {
	
  setWriteMode();

  /* Begin Write 0 Slot By Pulling 1-Wire Line Low */
  GPIOC->ODR &= ~GPIO_ODR_ODR14;

  /* Release 1-Wire Line Within 15탎 But As Fast As Possible */
  GPIOC->ODR |= GPIO_ODR_ODR14;

  /* Wait Long Time */
  wait(60);	
}

void writeZero() {
	
  setWriteMode();

  /* Begin Write 1 Slot By Pulling 1-Wire Line Low */
  GPIOC->ODR &= ~GPIO_ODR_ODR14;

  /* Wait Long Time */
  wait(60);

  /* Release 1-Wire Line */
  GPIOC->ODR |= GPIO_ODR_ODR14;

  /* Wait Short Time */
  wait(5);

}

uint8_t readBit() {
	
  setWriteMode();

  /* Begin Read Slot By Pulling 1-Wire Line Low */
  GPIOC->ODR &= ~GPIO_ODR_ODR14;

  /* Wait Short Time */
  wait(1);

  /* Release 1-Wire Line */
  GPIOC->ODR |= GPIO_ODR_ODR14;

  setReadMode();

  /* Sample At The End Of 15탎 Read Slot */
  wait(7);
  
  const uint8_t bit_val = (GPIOC->IDR & GPIO_IDR_IDR14) >> 14;
  
  /* Read Time Slot Is At Least 60탎 Long */
  wait(52);
  
  return bit_val;
}

bool startCommunication() {
	
  setWriteMode();

  /* Begin Master Tx Reset Pulse By Pulling 1-Wire Line Low */
  GPIOC->ODR &= ~GPIO_ODR_ODR14;

  /* Wait For At Least 480탎 */
  wait(480);

  /* End Master Tx Reset Pulse By Releasing 1-Wire Line */
  GPIOC->ODR |= GPIO_ODR_ODR14;

  setReadMode();

  /* Let DS18B20 Delay Time Elapse */
  wait(60);

  /* 1-Wire Should Definitely Be Low Now */
  const bool is_low = !( ( GPIOC->IDR & GPIO_IDR_IDR14 ) >> 14 );

  /* Rx Pulse Of DS18B20 Need To Be At Least 480탎 long */
  wait(420);

  return is_low;
}

void sendByte(const uint8_t byte) {
	
  for(uint8_t bit_pos = 0; bit_pos <= 7; bit_pos++) {
          
    if( byte & (1 << bit_pos) ) {
      writeOne();
    } else {
      writeZero();
    }

    /* Recovery Time Between Write Slots */
    wait(2);
  }
	
}

uint64_t readData(const uint8_t amountOfBits) {
	
  uint64_t retData;

  for(uint8_t bit_pos = 0; bit_pos <= amountOfBits; bit_pos++) {
          
    const uint8_t read_bit = readBit();
    retData |= (read_bit << bit_pos);

    /* Recovery Time Between Read Slots */
    wait(2);
  }

  return retData;
}

uint64_t readDeviceID() {
	
  uint64_t ret_val = 0;

  const bool successfull = startCommunication();
  if(successfull) {
    sendByte(MATCH_ROM_CMD);
    wait(2);
    ret_val = readData(64);
  }

  return ret_val;
}

void initDS18B20(struct DS18B20 *self) {
  self->readDeviceID = &readDeviceID;
}
