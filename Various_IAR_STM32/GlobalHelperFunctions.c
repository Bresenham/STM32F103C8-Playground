#include "GlobalHelperFunctions.h"
#include "CMSIS/stm32f1xx.h"

void wait(const uint32_t us) {
	
  /* Rough Estimation */
  volatile uint32_t amount_of_loops = ( sysclk_in_mhz * us ) / 12;

  while(amount_of_loops > 0) {
    amount_of_loops--;
  }
}

void update_sysclk_to_128mhz() {
  /* Disable PLL To Configure */
  RCC->CR &= ~RCC_CR_PLLON;
  
  /* Enable External Oscillator */
  RCC->CR |= RCC_CR_HSEON;
  
  /* Wait Until External Oscillator Becomes Ready */
  while( !(RCC->CR & RCC_CR_HSERDY) ) { ; }
  
  /* 16x PLL */
  RCC->CFGR |= RCC_CFGR_PLLMULL9;
  
  /* External Oscillator is not divided */
  RCC->CFGR &= ~RCC_CFGR_PLLXTPRE;
  
  /* Use External Oscillator as PLL Input */
  RCC->CFGR |= RCC_CFGR_PLLSRC;
  
  /* No Clock Division for APB2-Bus */
  RCC->CFGR &= ~RCC_CFGR_PPRE2;
  
  /* No Clock Division for APB1-Bus */
  RCC->CFGR &= ~RCC_CFGR_PPRE1;
  
  /* Re-Enable PLL */
  RCC->CR |= RCC_CR_PLLON;
  
  /* Wait Until PLL Becomes Ready */
  while( !(RCC->CR & RCC_CR_PLLRDY) ) { ; }
  
  /* Clear Flash Waitstates */
  FLASH->ACR &= ~FLASH_ACR_LATENCY;
  
  /* Enable Prefetch Buffer */
  // FLASH->ACR |= FLASH_ACR_PRFTBE;
  
  /* Wait Until Prefetch Buffer Is Enabled */
  // while( !(FLASH->ACR & FLASH_ACR_PRFTBS) ) { ; }
  
  /* Set Flash To Two Waitstates */
  FLASH->ACR |= FLASH_ACR_LATENCY_1;
  
  /* Clear System Clock Selection */
  RCC->CFGR &= ~RCC_CFGR_SW;
  
  /* Select PLL As System Clock */
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  
  /* Wait Until PLL As System Clock Is Active */
  while( (RCC->CFGR & RCC_CFGR_SWS != RCC_CFGR_SWS_PLL) ) { ; }
  
  SystemCoreClockUpdate();
}