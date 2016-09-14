#include "lcd.h"
#include "gpio.h"


typedef struct {
  bool backlight;
  uint8_t addr;
  uint8_t width;
  uint8_t height;
} lcd_t;


static lcd_t lcd = {
  .backlight = true,
};


void lcd_init(uint8_t addr, uint8_t width, uint8_t height) {
  lcd.addr = addr;
  lcd.width = width;
  lcd.height = height;

  // Clocks
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;   // Enable I2C1 peripheral clock
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;    // Enable GPIOB peripheral clock

  // GPIO
  GPIO_SET_AFRL(B, 6, 1);    // AF1
  GPIO_SET_AFRL(B, 7, 1);    // AF1

  GPIO_SET_MODER(B, 6, 2);   // Alt mode
  GPIO_SET_MODER(B, 7, 2);   // Alt mode

  GPIO_SET_OTYPER(B, 6, 1);  // Open-drain
  GPIO_SET_OTYPER(B, 7, 1);  // Open-drain

  GPIO_SET_OSPEEDR(B, 6, 0); // Low speed
  GPIO_SET_OSPEEDR(B, 7, 0); // Low speed

  GPIO_SET_PUPDR(B, 6, 1);   // Pull-up
  GPIO_SET_PUPDR(B, 7, 1);   // Pull-up

  // I2C
  I2C1->TIMINGR = 0x1045061d;
  I2C1->CR1 |= I2C_CR1_PE;   // Enable I2C port
}


static void _i2c_wait(uint16_t flag, bool active) {
  for (int i = 0; i < 0x1000; i++)
    if ((!(I2C1->ISR & flag) ^ active)) break;
}


static void _write_i2c(uint8_t data) {
  if (lcd.backlight) data |= BACKLIGHT_BIT;

  I2C1->ICR = I2C_ICR_NACKCF | I2C_ICR_STOPCF;     // Clear NACKF and STOPF
  _i2c_wait(I2C_ISR_BUSY, false);                  // Wait !BUSY
  // Set slave address, byte count, START, AUTOEND
  I2C1->CR2 = (lcd.addr << 1) | (1 << 16) | I2C_CR2_START | I2C_CR2_AUTOEND;
  _i2c_wait(I2C_ISR_TXIS, true);                   // Wait for transmit
  I2C1->TXDR = data;                               // Send data
  _i2c_wait(I2C_ISR_STOPF, true);                  // Wait for STOP
  I2C1->ICR = 0x3f30;                              // Clear flags
}


static void _write_nibble(uint8_t data) {
  _write_i2c(data);

  // Strobe
  _write_i2c(data | ENABLE_BIT);
  _write_i2c(data & ~ENABLE_BIT);
}


static void _i2c_delay() {
  for (int i = 0; i < 0xffff; i++) continue;
}

void lcd_reset() {
  _i2c_delay();
  _write_nibble(3 << 4); // Home
  _i2c_delay();
  _write_nibble(3 << 4); // Home
  _i2c_delay();
  _write_nibble(3 << 4); // Home
  _write_nibble(2 << 4); // 4-bit

  lcd_clear();
  lcd_write(LCD_FUNCTION_SET | LCD_2_LINE | LCD_5x8_DOTS | LCD_4_BIT_MODE);
  lcd_write(LCD_DISPLAY_CONTROL | LCD_DISPLAY_ON);
  lcd_write(LCD_ENTRY_MODE_SET | LCD_ENTRY_SHIFT_INC);
}


void lcd_writef(uint8_t cmd, uint8_t flags) {
  _write_nibble(flags | (cmd & 0xf0));
  _write_nibble(flags | ((cmd << 4) & 0xf0));
}


void lcd_write(uint8_t cmd) {lcd_writef(cmd, 0);}


void lcd_set_cursor(bool on, bool blink) {
  lcd_write(LCD_DISPLAY_CONTROL |
            (on ? LCD_CURSOR_ON : 0) | (blink ? LCD_BLINK_ON : 0));
}


void lcd_set_backlight(bool enable) {
  lcd.backlight = enable;
  _write_i2c(0);
}


void lcd_program_char(uint8_t addr, uint8_t data[8]) {
  if (addr < 0 || 8 <= addr) return;

  lcd_write(LCD_SET_CGRAM_ADDR | (addr << 3));

  for (int i = 0; i < 8; i++) // TODO is this the right byte count?
    lcd_writef(data[i], REG_SELECT_BIT);
}


void lcd_goto(int x, int y) {
  static const uint8_t rows[] = {0, 64, 20, 84};

  if (x < 0 || lcd.width <= x || y < 0 || lcd.height <= y) return;
  lcd_write(LCD_SET_DDRAM_ADDR | (rows[y] + x));
}


void lcd_text(const char *msg, int8_t x, int8_t y) {
  lcd_goto(x, y);
  while (*msg) lcd_writef(*msg++, REG_SELECT_BIT);
}


void lcd_clear() {
  lcd_write(LCD_CLEAR_DISPLAY);
  lcd_write(LCD_RETURN_HOME);
}
