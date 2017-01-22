#pragma once

#define USART_BAUD 115200
#define USART_TX_RING_BUF_SIZE 1024
#define USART_RX_RING_BUF_SIZE 256

void usart_init();
int usart_read(char *buffer, int length);
int usart_write(const char *buffer, int length);
void usart_flush();
