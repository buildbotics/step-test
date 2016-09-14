#pragma once

#include <stdint.h>
#include <stdbool.h>


// Control flags
enum {
  REG_SELECT_BIT          = 1 << 0,
  READ_BIT                = 1 << 1,
  ENABLE_BIT              = 1 << 2,
  BACKLIGHT_BIT           = 1 << 3,
};


// Commands
enum {
  LCD_CLEAR_DISPLAY       = 1 << 0,
  LCD_RETURN_HOME         = 1 << 1,
  LCD_ENTRY_MODE_SET      = 1 << 2,
  LCD_DISPLAY_CONTROL     = 1 << 3,
  LCD_CURSOR_SHIFT        = 1 << 4,
  LCD_FUNCTION_SET        = 1 << 5,
  LCD_SET_CGRAM_ADDR      = 1 << 6,
  LCD_SET_DDRAM_ADDR      = 1 << 7,
};


// Entry Mode Set flags
enum {
  LCD_ENTRY_SHIFT_DISPLAY = 1 << 0,
  LCD_ENTRY_SHIFT_INC     = 1 << 1,
  LCD_ENTRY_SHIFT_DEC     = 0 << 1,
};


// Display Control flags
enum {
  LCD_BLINK_ON            = 1 << 0,
  LCD_BLINK_OFF           = 0 << 0,
  LCD_CURSOR_ON           = 1 << 1,
  LCD_CURSOR_OFF          = 0 << 1,
  LCD_DISPLAY_ON          = 1 << 2,
  LCD_DISPLAY_OFF         = 0 << 2,
};


// Cursor Shift flags
enum {
  LCD_SHIFT_RIGHT         = 1 << 2,
  LCD_SHIFT_LEFT          = 0 << 2,
  LCD_SHIFT_DISPLAY       = 1 << 3,
  LCD_SHIFT_CURSOR        = 0 << 3,
};


// Function Set flags
enum {
  LCD_5x11_DOTS           = 1 << 2,
  LCD_5x8_DOTS            = 0 << 2,
  LCD_2_LINE              = 1 << 3,
  LCD_1_LINE              = 0 << 3,
  LCD_8_BIT_MODE          = 1 << 4,
  LCD_4_BIT_MODE          = 0 << 4,
};


void lcd_init(uint8_t addr, uint8_t width, uint8_t height);
void lcd_reset();
void lcd_writef(uint8_t cmd, uint8_t flags);
void lcd_write(uint8_t cmd);
void lcd_set_cursor(bool on, bool blink);
void lcd_set_backlight(bool enable);
void lcd_program_char(uint8_t addr, uint8_t data[8]);
void lcd_goto(int x, int y);
void lcd_text(const char *msg, int8_t x, int8_t y);
void lcd_clear();
