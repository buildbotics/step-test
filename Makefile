TARGET=step-test
CPU=cortex-m0
DEVICE=STM32F051

COMMON = -mcpu=$(CPU) -mthumb -ffunction-sections -fdata-sections -fno-exceptions -fno-unwind-tables

CFLAGS += $(COMMON) -Wall -Werror -O3 -std=gnu99 -D$(DEVICE)
CFLAGS += -mlittle-endian -march=armv6-m
CFLAGS += -MD -MP -MT $@ -MF build/dep/$(@F).d
CFLAGS += -Istm32f0 -Isrc

LINKFLAGS += $(COMMON)
LINKFLAGS += -Tstm32f0.ld -Wl,--gc-sections

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))

STMLIB = build/libstm32f0.a

STM_SRC := $(wildcard stm32f0/*.c) $(wildcard stm32f0/*.s)
STM_OBJ := $(patsubst stm32f0/%.c,build/%.o,$(STM_SRC))
STM_OBJ := $(patsubst stm32f0/%.s,build/%.o,$(STM_OBJ))


PREFIX=arm-none-eabi-
CC=$(PREFIX)gcc
AS=$(PREFIX)as
LD=$(PREFIX)ld
SIZE=$(PREFIX)size
OBJCOPY=$(PREFIX)objcopy

STFLASH=st-flash


all: $(TARGET).bin size

build/%.o: */%.c
	@mkdir -p $(shell dirname $@)
	$(CC) -c -o $@ $< $(CFLAGS)

build/%.o: */%.s
	@mkdir -p $(shell dirname $@)
	$(CC) -c -o $@ $< $(CFLAGS)

$(STMLIB): $(STM_OBJ)
	$(AR) -r $@ $(STM_OBJ)

$(TARGET).elf: $(OBJ) $(STMLIB)
	$(CC) -o $@ $(OBJ) $(LINKFLAGS) $(STMLIB)

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

size: $(TARGET).elf
	@$(SIZE) $<

program: $(TARGET).bin
	$(STFLASH) write $< 0x08000000

tidy:
	rm -f $(shell find . -name \*~ -o -name \#\*)

clean: tidy
	rm -rf $(TARGET).bin $(TARGET).elf build

.PHONY: tidy clean all program size

# Dependencies
-include $(shell mkdir -p build/dep) $(wildcard build/dep/*)
