#include "stm32f0xx.h"

#include "gpio.h"
#include "lcd.h"
#include "reset.h"
#include "step.h"

#include <stdbool.h>
#include <stdio.h>


void SysTick_Handler() {
  GPIO_TGL_PIN(C, 8);
  GPIO_TGL_PIN(C, 9);

  reset_callback();

  char buf[21];
  for (int i = 0; i < 3; i++) {
    sprintf(buf, "% 19ld%c", steps_get(i), "XYZ"[i]);
    lcd_text(buf, 0, i + 1);
  }
}


void init() {
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;   // Enable GPIOC peripheral clock

  GPIO_SET_MODER(C, 8, 1); // Output
  GPIO_SET_MODER(C, 9, 1); // Output

  GPIO_CLR_PIN(C, 8);
  GPIO_SET_PIN(C, 9);

  lcd_init(0x27, 20, 4);
  reset_init();
  step_init();
}


int main() {
  init();

  SysTick_Config(SystemCoreClock / 4);

  while (true) continue;
}
