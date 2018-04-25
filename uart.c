#include "uart.h"

#define BAUD_RATE 115200

void init_UART() {
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
  
  /* Set PA9 to USART1_TX */
  GPIOA->CRH &=~GPIO_CRH_CNF9_0;
  GPIOA->CRH |= GPIO_CRH_CNF9_1 | GPIO_CRH_CNF9_0;
  GPIOA->CRH |= GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0;
  
  /* Assuming Baud Rate of 230400 by a clock of 64MHz Core Clock*/
  USART1->GTPR |= 1;
  USART1->BRR = 17 << 4;
  USART1->BRR |= 6;
  
  USART1->CR1 |= USART_CR1_TE;
  USART1->CR1 |= USART_CR1_UE;
}

void send_char(char c) {
  USART1->DR = c;
  while(!(USART1->SR & USART_SR_TXE))
    asm volatile ("nop");
}

void send_str(char *c, int size) {
  for(uint8_t i = 0; i < size; i++)
    send_char(c[i]);
}