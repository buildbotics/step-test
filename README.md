Step test
=========
Firmware for STM32 discovery board used to track and verify stepper motor
outputs.

# Setup

  | Pin  | Wire   | Description  |
  | ---- | ------ | ------------ |
  | 3v   | White  | Reset switch |
  | PA0  | Yellow | Reset switch |
  | PA2  | Brown  | USART TX     |
  | Gnd  | Black  | Ground       |
  | PB4  | Green  | X Step       |
  | PB9  | Blue   | X Enable     |
  | PB12 | White  | X Dir        |
  | PB5  | Green  | Y Step       |
  | PB10 | Blue   | Y Enable     |
  | PB13 | White  | Y Dir        |
  | PB8  | Green  | Z Step       |
  | PB11 | Blue   | Z Enable     |
  | PB14 | White  | Z Dir        |

Wire colors are particular to my setup.

Device is powered and programmed over USB port.

Logic level USB to RS232 is connected to USART TX and Ground.  USART config is
N81 115200.

View the real-time velocity plot by running:

    ./plot_velocity.py --port /dev/ttyUSB0 --baud 115200

# Notes

* Distance (mm) per step is hard coded in ``step.c``.
* Baud rate is set in ``usart.h``.
* Program STM32F051 over USB with ``make program``
* On board green & blue LEDs alternate to indicate device is running.
* Pin out of Acroname Brainstem is:

```
       ||                   v-- Host LED
       ||-+
     +-||  |--------------------+
    |  ||  | o <- RTS      []   |-+  <- Gnd
    |  ||  |                    | |  <- Rx <-
    |  ||  |                    | |  <- Vcc
    |  ||  | o <- CTX      []   |-+  <- Tx ->
     +-||  |--------------------+
       ||-+
       ||                   ^-- Power LED
```
