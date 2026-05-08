/**
 * @file    syscalls.c
 * @brief   Minimal system calls for STM32 bare-metal environment.
 */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>

/* Handle to Heap */
extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

caddr_t _sbrk(int incr)
{
    extern char end asm("end");
    static char *heap_end;
    char *prev_heap_end;
    char *stack_ptr;

    /* Get current stack pointer */
    __asm__ volatile("mov %0, sp" : "=r"(stack_ptr));

    if (heap_end == 0)
    {
        heap_end = &end;
    }

    prev_heap_end = heap_end;
    if (heap_end + incr > stack_ptr)
    {
        errno = ENOMEM;
        return (caddr_t)-1;
    }

    heap_end += incr;

    return (caddr_t)prev_heap_end;
}

int _read(int file, char *ptr, int len)
{
    (void)file;
    (void)ptr;
    (void)len;
    return 0;
}

int _lseek(int file, int ptr, int dir)
{
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

int _write(int file, char *ptr, int len)
{
    (void)file;
    (void)ptr;
    return len;
}

int _close(int file)
{
    (void)file;
    return -1;
}

int _fstat(int file, struct stat *st)
{
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    (void)file;
    return 1;
}

int _getpid(void)
{
    return 1;
}

int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}

void _exit(int status)
{
    (void)status;
    while (1)
    {
    }
}

/* End of File: syscalls.c */
