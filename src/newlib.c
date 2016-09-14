/* Newlib stubs
 *
 * Original by nanoage.co.uk, Nov. 2010
 * Updated Joseph Coffland, Cauldron Development LLC Sept. 2016
 */

#include <errno.h>
#include <stdio.h>

#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>

#include "stm32f0xx_usart.h"


#ifndef USART
#define USART USART1
#endif


// Environment
char *__env[1] = {0};
char **environ = __env;


// Process
void _exit(int status) {while (1) continue;}
int _execve(char *name, char **argv, char **env) {errno = ENOMEM; return -1;}
int _fork() {errno = EAGAIN; return -1;}
int _getpid() {return 1;}
int _kill(int pid, int sig) {errno = EINVAL; return (-1);}
int _wait(int *status) {errno = ECHILD; return -1;}
clock_t _times(struct tms *buf) {return -1;}


// File
int _close(int fd) {return -1;}
int _stat(const char *path, struct stat *st) {st->st_mode = S_IFCHR; return 0;}
int _fstat(int fd, struct stat *st) {st->st_mode = S_IFCHR; return 0;}
int _link(char *old, char *new) {errno = EMLINK; return -1;}
int _lseek(int fd, int ptr, int dir) {return 0;}
int _unlink(char *name) {errno = ENOENT; return -1;}


int _isatty(int fd) {
  switch (fd) {
  case STDOUT_FILENO:
  case STDERR_FILENO:
  case STDIN_FILENO:
    return 1;

  default: errno = EBADF; return 0;
  }
}


int _read(int fd, char *ptr, int len) {
  switch (fd) {
  case STDIN_FILENO: {
    int bytes;

    for (bytes = 0; bytes < len; bytes++) {
      while ((USART->ISR & USART_FLAG_RXNE) == RESET) continue;
      *ptr++ = USART->RDR;
    }

    return bytes;
  }

  default: errno = EBADF; return -1;
  }
}


int _write(int fd, char *ptr, int len) {
  switch (fd) {
  case STDOUT_FILENO:
  case STDERR_FILENO:
    for (int i = 0; i < len; i++) {
      while ((USART->ISR & USART_FLAG_TC) == RESET) continue;
      USART->TDR = *ptr++ & 0x01FF;
    }
    return len;

  default: errno = EBADF; return -1;
  }
}


caddr_t _sbrk(int len) {
  extern char _ebss; // Defined by the linker
  static char *heap_end = &_ebss;
  char *prev_heap_end = heap_end;
  char *stack = (char *)__get_MSP();

  if (stack < heap_end + len) {
    errno = ENOMEM;
    perror("Heap and stack collision\n");
    return (caddr_t)-1;
  }

  heap_end += len;

  return (caddr_t)prev_heap_end;
}
