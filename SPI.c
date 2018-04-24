#include "SPI.h"

void init_SPI() {
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
  
  /* Set PA2 als LCD-Reset */
  GPIOA->CRL &=~GPIO_CRL_CNF2_0;
  GPIOA->CRL |= GPIO_CRL_MODE2_1 | GPIO_CRL_MODE2_0;
  
  /* Set PA3 as LCD-A0 */
  GPIOA->CRL &=~GPIO_CRL_CNF3_0;
  GPIOA->CRL |= GPIO_CRL_MODE3_1 | GPIO_CRL_MODE3_0;
  
  /* Set PA4 to SPI1_NSS Alternate Function */
  GPIOA->CRL &=~GPIO_CRL_CNF4_0;
  GPIOA->CRL |= GPIO_CRL_MODE4_1 | GPIO_CRL_MODE4_0;
  
  /* Set PA5 to SPI1_SCK Alternate Function */
  GPIOA->CRL &=~GPIO_CRL_CNF5_0;
  GPIOA->CRL |= GPIO_CRL_CNF5_1;
  GPIOA->CRL |= GPIO_CRL_MODE5_1 | GPIO_CRL_MODE5_0;
  
  /* Set PA6 to SPI1_MISO Alternate Function */
  GPIOA->CRL &=~GPIO_CRL_CNF6_0;
  GPIOA->CRL |= GPIO_CRL_CNF6_1;
  GPIOA->CRL |= GPIO_CRL_MODE6_1 | GPIO_CRL_MODE6_0;
  
  /* Set PA7 to SPI1_MOSI Alternate Function */
  GPIOA->CRL &=~GPIO_CRL_CNF7_0;
  GPIOA->CRL |= GPIO_CRL_CNF7_1;
  GPIOA->CRL |= GPIO_CRL_MODE7_1 | GPIO_CRL_MODE7_0;
  
  /* SPI Configuration */
  //SPI1->CR1 |= SPI_CR1_BR_0;
  SPI1->CR1 |= SPI_CR1_CPHA | SPI_CR1_CPOL;
  
  SPI1->CR2 |= SPI_CR2_SSOE;
  
  SPI1->CR1 |= SPI_CR1_MSTR;
  SPI1->CR1 |= SPI_CR1_SPE;
}

void clk_low() {
  SPI1->CR1 &=~SPI_CR1_SPE;
  GPIOA->CRL &=~GPIO_CRL_CNF5_1;
  GPIOA->ODR |= GPIO_ODR_ODR5;
  delay(5000);
  GPIOA->ODR &=~GPIO_ODR_ODR5;
  GPIOA->CRL |= GPIO_CRL_CNF5_1;
  SPI1->CR1 |= SPI_CR1_SPE;
}

void reset_high(){
  GPIOA->ODR |= GPIO_ODR_ODR2;
}
void reset_low(){
  GPIOA->ODR &=~GPIO_ODR_ODR2;
}

void reset() {
  reset_low();
  delay(500);
  reset_high();
  delay(500);
}

void a0_high() {
  GPIOA->ODR |= GPIO_ODR_ODR3;
}

void a0_low() {
  GPIOA->ODR &=~GPIO_ODR_ODR3;
}

void cs_low() {
  GPIOA->ODR &=~GPIO_ODR_ODR4;
}

void cs_high() {
  GPIOA->ODR |= GPIO_ODR_ODR4;
}

void spi_transmit(uint8_t data) {
  SPI1->DR = data;
  while(SPI1->SR & SPI_SR_BSY)
    asm volatile ("nop");
}