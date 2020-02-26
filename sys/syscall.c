/*
 *
 * 		Atollic TrueSTUDIO Minimal System calls file
 *
 * 		For more information about which c-functions
 * 		need which of these lowlevel functions
 * 		please consult the Newlib libc-manual
 *
 * */
#include <sys/stat.h> //caddr_t
//#include <stdlib.h>
#include <errno.h>    //EINVAL, ...
//#include <stdio.h>
//#include <signal.h>
//#include <time.h>
//#include <sys/time.h>
//#include <sys/times.h>

#undef errno
extern int errno;

//connected to the serial lib.
extern void __io_putchar(char c);

int _getpid(void)
{
    return 1;
}

int _kill(int __attribute__((unused)) pid,
          int __attribute__((unused)) sig)
{
    errno = EINVAL;
    return -1;
}

void _exit (int status)
{
    _kill(status, -1);
    while (1) {}		/* Make sure we hang here */
}

int _write(int __attribute__((unused)) file,
           char * ptr,
           int len)
{
    for (int todo = 0; todo < len; todo++)
    {
        __io_putchar( *ptr++ );
    }
    return len;
}

extern void * _end; //@ end of RAM
caddr_t _sbrk(int incr)
{
    static unsigned char *heap = NULL;
    unsigned char *prev_heap;

    if (heap == NULL) {
        heap = (unsigned char *)&_end;
    }
    prev_heap = heap;

    heap += incr;

    return (caddr_t) prev_heap;
}

int _close(int __attribute__((unused)) file)
{
    return -1;
}
//
//
//int _fstat(int __attribute__((unused)) file, struct stat *st)
//{
//    st->st_mode = S_IFCHR;
//    return 0;
//}
//
//int _isatty(int __attribute__((unused)) file)
//{
//    return 1;
//}
//
//int _lseek(int __attribute__((unused)) file,
//           int __attribute__((unused)) ptr,
//           int __attribute__((unused)) dir)
//{
//    return 0;
//}
//
//int _read(int  __attribute__((unused)) file,
//          char __attribute__((unused)) *ptr,
//          int  __attribute__((unused)) len)
//{
//    return 0;
//}
//
//int _open(char __attribute__((unused)) *path,
//          int  __attribute__((unused)) flags,
//          ...)
//{
//    /* Pretend like we always fail */
//    return -1;
//}
//
//int _wait(int __attribute__((unused)) *status)
//{
//    errno = ECHILD;
//    return -1;
//}
//
//int _unlink(char __attribute__((unused)) *name)
//{
//    errno = ENOENT;
//    return -1;
//}
//
//int _times(struct tms __attribute__((unused))* buf)
//{
//    return -1;
//}
//
//int _stat(char __attribute__((unused)) *file, struct stat *st)
//{
//    st->st_mode = S_IFCHR;
//    return 0;
//}
//
//int _link(char __attribute__((unused)) *old,
//          char __attribute__((unused)) *new)
//{
//    errno = EMLINK;
//    return -1;
//}
//
//int _fork(void)
//{
//    errno = EAGAIN;
//    return -1;
//}
//
//int _execve(char __attribute__((unused))  *name,
//            char __attribute__((unused)) **argv,
//            char __attribute__((unused)) **env)
//{
//    errno = ENOMEM;
//    return -1;
//}
