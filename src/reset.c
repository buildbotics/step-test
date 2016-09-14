#include "reset.h"

#include "gpio.h"
#include "lcd.h"
#include "step.h"

#include <stdio.h>

static bool _reset = true;


void EXTI0_1_IRQHandler() {
  _reset = true;
  EXTI->PR = 1; // Clear interrupt
}


void reset_init() {
  // Clocks
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG peripheral clock
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;    // Enable GPIOA peripheral clock

  GPIO_SET_MODER(A, 0, 0); // Input

  // Interrupt EXTI0 source mapped to pin PA0
  SYSCFG->EXTICR[0] = (SYSCFG->EXTICR[0] & ~SYSCFG_EXTICR1_EXTI0) | 0;

  EXTI->IMR |= 1;  // Enable EXTI0 mask
  EXTI->RTSR |= 1; // Rising edge

  NVIC_EnableIRQ(EXTI0_1_IRQn);
  NVIC_SetPriority(EXTI0_1_IRQn, 2); // Low
}


void reset() {
  step_reset();
  lcd_reset();
  lcd_text("-Vel-mm/s+---Steps+-", 0, 0);
}


void reset_callback() {
  if (_reset) reset();
  _reset = false;
}
