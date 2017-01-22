#include "step.h"

#include "gpio.h"
#include "lcd.h"

#include <stdio.h>
#include <string.h>


typedef struct {
  int32_t count;
  int32_t last;
  float velocity;
  float mm_per_step;

  const struct {
    uint8_t step;
    uint8_t enable;
    uint8_t dir;
  } pins;
} stepper_t;


#define MM_PER_STEP (6.35 * 1.8 / 360 / 32)

static stepper_t steps[3] = {
  {
    .mm_per_step = MM_PER_STEP,
    .pins = {4, 9, 12},
  }, {
    .mm_per_step = MM_PER_STEP,
    .pins = {5, 10, 13},
  }, {
    .mm_per_step = MM_PER_STEP,
    .pins = {8, 11, 14},
  },
};

static const uint16_t mask = 0x130;

#define PIN_MASK(N, TYPE) (1 << steps[N].pins.TYPE)
#define READ_PIN(N, TYPE) (GPIOB->IDR & PIN_MASK(N, TYPE))


void EXTI4_15_IRQHandler() {
  const uint16_t pr = EXTI->PR;
  EXTI->PR = (pr & mask); // Only clear interrupts which were read

  // Count steps
  for (int i = 0; i < 3; i++)
    if (pr & PIN_MASK(i, step) && READ_PIN(i, enable))
      steps[i].count += READ_PIN(i, dir) ? -1 : 1;
}


void step_init() {
  // Clocks
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG peripheral clock
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;    // Enable GPIOB peripheral clock

  for (int i = 0; i < 3; i++) {
    GPIO_SET_MODER(B, steps[i].pins.step,   0); // Input
    GPIO_SET_MODER(B, steps[i].pins.enable, 0); // Input
    GPIO_SET_MODER(B, steps[i].pins.dir,    0); // Input

    GPIO_SET_PUPDR(B, steps[i].pins.step,   1); // Pull-up
    GPIO_SET_PUPDR(B, steps[i].pins.enable, 1); // Pull-up
    GPIO_SET_PUPDR(B, steps[i].pins.dir,    1); // Pull-up

    // Interrupt EXTI source mapped to port B pins
    uint8_t pin = steps[i].pins.step;
    SYSCFG->EXTICR[pin >> 2] =
      (SYSCFG->EXTICR[pin >> 2] & ~(0xf << (4 * (pin & 3)))) |
      (1 << (4 * (pin & 3)));
  }

  EXTI->IMR |= mask;  // Enable mask
  EXTI->RTSR |= mask; // Rising edge
  //EXTI->FTSR |= mask; // Falling edge

  NVIC_EnableIRQ(EXTI4_15_IRQn);
  NVIC_SetPriority(EXTI4_15_IRQn, 0); // Hi
}


int32_t step_get_count(unsigned i) {return steps[i].count;}
float step_get_velocity(unsigned i) {return steps[i].velocity;}


void step_reset() {
  for (int i = 0; i < 3; i++)
    steps[i].count = steps[i].last = steps[i].velocity = 0;
}


void step_callback() {
  for (int i = 0; i < 3; i++) {
    __disable_irq();
    int32_t count = steps[i].count;
    __enable_irq();

    steps[i].velocity = (count - steps[i].last) * MM_PER_STEP / 0.01 * 60;
    steps[i].last = count;
  }
}
