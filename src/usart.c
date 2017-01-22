#include "usart.h"

#include "gpio.h"

#include <stdbool.h>


#ifndef USART
#define USART USART2
#endif

// Ring buffers
#define RING_BUF_NAME tx_buf
#define RING_BUF_SIZE USART_TX_RING_BUF_SIZE
#include "ringbuf.def"

#define RING_BUF_NAME rx_buf
#define RING_BUF_SIZE USART_RX_RING_BUF_SIZE
#include "ringbuf.def"


static void _set_txe_interrupt(bool enable) {
  if (enable) USART->CR1 |= USART_CR1_TXEIE;
  else USART->CR1 &= ~USART_CR1_TXEIE;
}


static void _set_rxne_interrupt(bool enable) {
  if (enable) USART->CR1 |= USART_CR1_RXNEIE;
  else USART->CR1 &= ~USART_CR1_RXNEIE;
}


void USART2_IRQHandler() {
  if (USART->ISR & USART_ISR_TXE) {
    if (tx_buf_empty()) _set_txe_interrupt(false); // Disable interrupt

    else {
      USART->TDR = tx_buf_peek();
      tx_buf_pop();
    }

    USART->ICR = USART_ISR_TXE;
  }

  if (USART->ISR & USART_ISR_RXNE) {
    if (rx_buf_full()) _set_rxne_interrupt(false); // Disable interrupt

    else {
      uint8_t data = USART->RDR;
      rx_buf_push(data);
    }

    USART->ICR = USART_ISR_RXNE;
  }
}


void usart_init() {
  // Clocks
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Enable USART2 peripheral clock
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;    // Enable GPIOA peripheral clock

  // GPIO
  GPIO_SET_AFRL(A, 2, 1);    // AF1
  GPIO_SET_AFRL(A, 3, 1);    // AF1

  GPIO_SET_MODER(A, 2, 2);   // Alt mode
  GPIO_SET_MODER(A, 3, 2);   // Alt mode

  // USART
  USART->BRR = SystemCoreClock / USART_BAUD;               // Set baud rate
  USART->CR1 = USART_CR1_TE | USART_CR1_UE | USART_CR1_RE; // Enable Rx & Tx

  // Interrupt
  NVIC_EnableIRQ(USART2_IRQn);
  NVIC_SetPriority(USART2_IRQn, 2); // Low
}


int8_t usart_getc() {
  while (rx_buf_empty()) continue;

  uint8_t data = rx_buf_peek();
  rx_buf_pop();

  _set_rxne_interrupt(true); // Enable interrupt

  return data;
}


int usart_read(char *buffer, int length) {
  int bytes;

  for (bytes = 0; bytes < length; bytes++)
    *buffer++ = usart_getc();

  return bytes;
}


void usart_putc(char c) {
  while (tx_buf_full()) continue;
  tx_buf_push(c);
  _set_txe_interrupt(true); // Enable interrupt
}


int usart_write(const char *buffer, int length) {
  for (int i = 0; i < length; i++) usart_putc(*buffer++);
  return length;
}


void usart_flush() {
}
