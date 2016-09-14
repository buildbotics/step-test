#pragma once

#include "stm32f0xx.h"


#define GPIO_MASK(WIDTH) ((1 << (WIDTH)) - 1)
#define GPIO_OFFSET(PIN, WIDTH, VALUE) ((VALUE) << ((PIN) * (WIDTH)))

#define GPIO_SET(PORT, REG, WIDTH, PIN, VALUE)                          \
  GPIO##PORT->REG =                                                     \
    (GPIO##PORT->REG & ~GPIO_OFFSET(PIN, WIDTH, GPIO_MASK(WIDTH))) |    \
    GPIO_OFFSET(PIN, WIDTH, VALUE & GPIO_MASK(WIDTH))

#define GPIO_SET_MODER(PORT, PIN, VALUE)        \
  GPIO_SET(PORT, MODER, 2, PIN, VALUE)

#define GPIO_SET_AFRL(PORT, PIN, VALUE)         \
  GPIO_SET(PORT, AFR[0], 4, PIN, VALUE)

#define GPIO_SET_AFRH(PORT, PIN, VALUE)         \
  GPIO_SET(PORT, AFR[1], 4, PIN, VALUE)

#define GPIO_SET_OTYPER(PORT, PIN, VALUE)       \
  GPIO_SET(PORT, OTYPER, 1, PIN, VALUE)

#define GPIO_SET_OSPEEDR(PORT, PIN, VALUE)      \
  GPIO_SET(PORT, OSPEEDR, 2, PIN, VALUE)

#define GPIO_SET_PUPDR(PORT, PIN, VALUE)        \
  GPIO_SET(PORT, PUPDR, 2, PIN, VALUE)

#define GPIO_SET_PIN(PORT, PIN) GPIO##PORT->ODR |= 1 << (PIN)
#define GPIO_CLR_PIN(PORT, PIN) GPIO##PORT->ODR &= ~(1 << (PIN))
#define GPIO_TGL_PIN(PORT, PIN) GPIO##PORT->ODR ^= 1 << (PIN)
