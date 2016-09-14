/*
 * newlib_stubs.c
 *
 *  Created on: 2 Nov 2010
 *      Author: nanoage.co.uk
 */
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>

#include "stm32f0xx_usart.h"


#ifndef USART
#define USART USART1
#endif


#undef errno
extern int errno;

char *__env[1] = {0};
char **environ = __env;


int _write(int file, char *ptr, int len);


void _exit(int status) {
  while (1) continue;
}


int _close(int file) {return -1;}


int _execve(char *name, char **argv, char **env) {
  errno = ENOMEM;
  return -1;
}


int _fork() {
  errno = EAGAIN;
  return -1;
}


int _fstat(int file, struct stat *st) {
  st->st_mode = S_IFCHR;
  return 0;
}


int _getpid() {return 1;}


int _isatty(int file) {
  switch (file){
  case STDOUT_FILENO:
  case STDERR_FILENO:
  case STDIN_FILENO:
    return 1;
  default:
    errno = EBADF;
    return 0;
  }
}


int _kill(int pid, int sig) {
  errno = EINVAL;
  return (-1);
}


int _link(char *old, char *new) {
  errno = EMLINK;
  return -1;
}


int _lseek(int file, int ptr, int dir) {return 0;}


caddr_t _sbrk(int incr) {
  extern char _ebss; // Defined by the linker
  static char *heap_end;
  char *prev_heap_end;

  if (!heap_end) heap_end = &_ebss;
  prev_heap_end = heap_end;

  char * stack = (char*) __get_MSP();
  if (heap_end + incr >  stack) {
    _write (STDERR_FILENO, "Heap and stack collision\n", 25);
    errno = ENOMEM;
    return  (caddr_t) -1;
  }

  heap_end += incr;
  return (caddr_t) prev_heap_end;
}


int _read(int file, char *ptr, int len) {
  int n;
  int num = 0;
  switch (file) {
  case STDIN_FILENO:
    for (n = 0; n < len; n++) {
      while ((USART->ISR & USART_FLAG_RXNE) == (uint16_t)RESET) {}
      char c = (char)(USART->RDR & (uint16_t)0x01FF);
      *ptr++ = c;
      num++;
    }
    break;

  default:
    errno = EBADF;
    return -1;
  }

  return num;
}


int _stat(const char *filepath, struct stat *st) {
  st->st_mode = S_IFCHR;
  return 0;
}


clock_t _times(struct tms *buf) {return -1;}


int _unlink(char *name) {
  errno = ENOENT;
  return -1;
}


int _wait(int *status) {
  errno = ECHILD;
  return -1;
}


int _write(int file, char *ptr, int len) {
  switch (file) {
  case STDOUT_FILENO:
    for (int n = 0; n < len; n++) {
      while ((USART->ISR & USART_FLAG_TC) == (uint16_t)RESET) {}
      USART->TDR = (*ptr++ & (uint16_t)0x01FF);
    }
    break;

  case STDERR_FILENO:
    for (int n = 0; n < len; n++) {
      while ((USART->ISR & USART_FLAG_TC) == (uint16_t)RESET) {}
      USART->TDR = (*ptr++ & (uint16_t)0x01FF);
    }
    break;

  default:
    errno = EBADF;
    return -1;
  }

  return len;
}
