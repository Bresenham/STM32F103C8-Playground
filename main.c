#include "stdbool.h"
#include "Rendering/renderer.h"

void TIM2_IRQHandler (void) {
    TIM2->SR &=~TIM_SR_UIF;
    GPIOC->ODR ^= GPIO_ODR_ODR13;
}

void timer2_setup() {
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  
  TIM2->PSC = 0;
  TIM2->ARR = 0xFFFF;
  TIM2->DIER |= TIM_DIER_UIE;
  
  NVIC_EnableIRQ(TIM2_IRQn);

  TIM2->CR1 = TIM_CR1_CEN;
}

void init_clock() {
  RCC->CR |= RCC_CR_HSION;
  while(!(RCC->CR & RCC_CR_HSIRDY))
    asm volatile ("nop");
  RCC->CFGR &=~ (RCC_CFGR_SW_0 | RCC_CFGR_SW_1);
  RCC->CR &=~RCC_CR_PLLON;
  while(RCC->CR & RCC_CR_PLLRDY)
    asm volatile ("nop");
  
  RCC->CFGR &=~ RCC_CFGR_PLLSRC;
  RCC->CFGR &=~(RCC_CFGR_PLLMULL_3 | RCC_CFGR_PLLMULL_2 | RCC_CFGR_PLLMULL_1 | RCC_CFGR_PLLMULL_0);
  RCC->CFGR |= RCC_CFGR_PLLMULL_3 | RCC_CFGR_PLLMULL_2 | RCC_CFGR_PLLMULL_1 | RCC_CFGR_PLLMULL_0;
  RCC->CR |= RCC_CR_PLLON;
  
  while(!(RCC->CR & RCC_CR_PLLRDY))
    asm volatile ("nop");
  
  RCC->CFGR |= RCC_CFGR_SW_1;
  
  while(!(RCC->CFGR & RCC_CFGR_SW_1))
    asm volatile ("nop");
}

int main()
{
  init_clock();
  init_SPI();
  
  /* Set GPIO C13 as High-Speed PushPull output */
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  GPIOC->CRH = 0;
  GPIOC->CRH |= GPIO_CRH_MODE13_1 | GPIO_CRH_MODE13_0;
  GPIOC->ODR &=~GPIO_ODR_ODR13;
  
  /* Set PIN A8 as MCO Output */ 
  /*
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  GPIOA->CRH = GPIO_CRH_CNF8_1;
  GPIOA->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_MODE8_0;
  RCC->CFGR |= RCC_CFGR_MCO_2;
  */

  //timer2_setup();
  
  ILI9163C_init();
  clear(0x0000);
  while(1){
    render();
    display();
  }
}
