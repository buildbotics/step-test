#include "step.h"

#include "gpio.h"
#include "lcd.h"

#include <stdio.h>

enum {
  STEP_PIN,
  ENABLE_PIN,
  DIR_PIN,
};

static int32_t steps[3] = {0};
static const uint8_t pins[] = {4, 5, 8, 9, 10, 11, 12, 13, 14};
static const uint16_t mask = 0x130;

#define READ_PIN(N, TYPE) (GPIOB->IDR & (1 << pins[TYPE * 3 + N]))


void EXTI4_15_IRQHandler() {
  // Count steps
  for (int i = 0; i < 3; i++)
    if (EXTI->PR & (1 << pins[i]) && !READ_PIN(i, ENABLE_PIN))
      steps[i] += READ_PIN(i, DIR_PIN) ? -1 : 1;

  EXTI->PR = mask; // Clear interrupts
}


void step_init() {
  // Clocks
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG peripheral clock
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;    // Enable GPIOB peripheral clock

  for (int i = 0; i < 9; i++) {
    uint8_t pin = pins[i];

    GPIO_SET_MODER(B, pin, 0); // Input
    GPIO_SET_PUPDR(B, pin, 1); // Pull-up

    // Interrupt EXTI source mapped to port B pins
    if (i < 3)
      SYSCFG->EXTICR[pin >> 2] =
        (SYSCFG->EXTICR[pin >> 2] & ~(0xf << (4 * (pin & 3)))) |
        (1 << (4 * (pin & 3)));
  }

  EXTI->IMR |= mask;  // Enable mask
  EXTI->RTSR |= mask; // Rising edge
  EXTI->FTSR |= mask; // Falling edge

  NVIC_EnableIRQ(EXTI4_15_IRQn);
  NVIC_SetPriority(EXTI4_15_IRQn, 0); // Hi
}


int32_t steps_get(unsigned i) {return steps[i];}
void steps_set(unsigned i, int32_t x) {steps[i] = x;}
