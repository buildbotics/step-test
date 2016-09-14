#include "stm32f0xx.h"

#include <stdbool.h>


void SysTick_Handler() {
  GPIOC->ODR ^= GPIO_ODR_8;
  GPIOC->ODR ^= GPIO_ODR_9;
}


void init() {
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;   // Enable GPIOC peripheral clock

  GPIOC->MODER |= GPIO_MODER_MODER8_0; // Output
  GPIOC->MODER |= GPIO_MODER_MODER9_0; // Output

  GPIOC->BSRR = GPIO_BSRR_BR_8; // Reset
  GPIOC->BSRR = GPIO_BSRR_BS_9; // Set
}


int main() {
  init();

  SysTick_Config(SystemCoreClock / 4);

  while (true) continue;
}
