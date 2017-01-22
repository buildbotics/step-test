#include "stm32f0xx.h"

#include "gpio.h"
#include "lcd.h"
#include "reset.h"
#include "step.h"
#include "usart.h"

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>


static volatile bool _update = false;


void SysTick_Handler() {
  step_callback();

  bool active = false;
  for (int i = 0; i < 3; i++) if (step_get_velocity(i)) active = true;

  static char buffer[128];
  if (active) {
    int len =
      sprintf(buffer, "%0.4g,%0.4g,%0.4g\n",
              step_get_velocity(0), step_get_velocity(1), step_get_velocity(2));
    if (0 < len) write(1, buffer, len);
  }

  static int quarter = 0;
  if (++quarter == 25) {
    _update = true;
    quarter = 0;
  }
}


void init() {
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;   // Enable GPIOC peripheral clock

  GPIO_SET_MODER(C, 8, 1); // Output
  GPIO_SET_MODER(C, 9, 1); // Output

  GPIO_CLR_PIN(C, 8);
  GPIO_SET_PIN(C, 9);

  usart_init();
  lcd_init(0x27, 20, 4);
  reset_init();
  step_init();
}


int main() {
  init();

  SysTick_Config(SystemCoreClock / 100);

  while (true)
    if (_update) {
      _update = false;

      GPIO_TGL_PIN(C, 8);
      GPIO_TGL_PIN(C, 9);

      reset_callback();

      char buf[21];
      for (int i = 0; i < 3; i++) {
        sprintf(buf, "%9.2f|% 8ld|%c", step_get_velocity(i), step_get_count(i),
                "XYZ"[i]);
        lcd_text(buf, 0, i + 1);
      }
    }
}
