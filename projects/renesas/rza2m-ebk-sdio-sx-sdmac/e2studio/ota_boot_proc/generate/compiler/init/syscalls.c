/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name : syscalls.c
* Description :
******************************************************************************/
/* Copyright (C) 2010 Embecosm Limited
 * Creative Commons License The document entitled "Howto: Porting newlib"
 * by Jeremy Bennett of Embecosm is licensed under a Creative Commons
 * Attribution 2.0 UK: England & Wales License. See the Legal Notice for details. 
 */
#include <sys/stat.h>
#include <errno.h>
#include <r_devlink_wrapper.h>
#include <unistd.h>

#include "swi.h"
#include "r_typedefs.h"
#include "r_startup_config.h"

#undef errno
extern int  errno;


static int monitor_stdin;
static int monitor_stdout;
static int monitor_stderr;

/* Struct used to keep track of the file position, just so we
   can implement fseek(fh,x,SEEK_CUR).  */

typedef struct
{
  int handle;
  int pos;
}
poslog;

#define MAX_OPEN_FILES (20)


/* Struct used to keep track of the file position, just so we
   can implement fseek(fh,x,SEEK_CUR).  */
struct fdent
{
  int handle;
  int pos;
  int ipending; /* Indicates fdent is being used but 'handle' not yet
                   allocated by the low level open  function */
};

/* Forward prototypes.  */
void    initialise_monitor_handles _PARAMS ((void));

static int get_errno (void);
static int error (int result);
static int wrap  _PARAMS ((int));

int _swiopen (const char * path, int flags);
int _open (const char * path, int flags, ...);
int _swiclose (int fh);
int _close(int fh);
int _swiwrite (int fh, char * ptr, int len);
int _write(int fh , const char *buffer , unsigned int n);
int _swiread (int fh, char * ptr, int len);
int _read(int fh , char *buffer , unsigned int n);
int _stat (const char *fname, struct stat *st);
int _fstat (int fd, struct stat * st);
int _getpid(int n);
int _isatty(int fd);
int _lseek (int fd, int ptr, int dir);
caddr_t _sbrk (int incr);

extern int starc_write(int fh , const char *buffer , unsigned int n);
extern int starc_read(int fh , char *buffer , unsigned int n);

void initialise_monitor_handles (void)
{
  /* Open the standard file descriptors by opening the special
   * teletype device, ":tt", read-only to obtain a descritpor for
   * standard input and write-only to obtain a descriptor for standard
   * output. Finally, open ":tt" in append mode to obtain a descriptor
   * for standard error. Since this is a write mode, most kernels will
   * probably return the same value as for standard output, but the
   * kernel can differentiate the two using the mode flag and return a
   * different descriptor for standard error.
   */

  int fh;
  char * name;
  name = "\\\\.\\stdin";
  fh = low_open(name, O_RDWR);
  monitor_stdin = fh;
  name = "\\\\.\\stdout";
  fh = low_open(name, O_WRONLY);
  monitor_stdout = fh;
  name = "\\\\.\\stderr";
  fh = low_open(name, O_WRONLY);

  /* If we failed to open stderr, redirect to stdout. */
  if (monitor_stderr == -1)
  {
      monitor_stderr = monitor_stdout;
  }
}

static int get_errno (void)
{
#if SWI_TRAP_ENABLED
  __asm ("swi %a0" :: "i" (SWI_GetErrno));
#endif
  return 0;
}

static int error (int result)
{
  errno = get_errno ();
  return result;
}


static int wrap (int result)
{
  if (result == -1)
  {
    return error (-1);
  }
  return result;
}

/********************************************************/
int _getpid(int n)
{
  UNUSED_PARAM(n);

  return  1;
}

/*******************************************************************************
* Function Name: _sbrk
* Description  : 
* Arguments    : incr - Requested memory size.  
* Return Value : Extended address of heap tail.
*******************************************************************************/
caddr_t _sbrk (int incr)
    {
  extern char   end __asm ("__heap_start");         /* Defined by the linker.  */
  extern char   heap_limit __asm ("__heap_end");    /* Defined by the linker.  */
  static char * heap_end = NULL;
  char *        prev_heap_end;

  if (heap_end == NULL)
    heap_end = & end;

  prev_heap_end = heap_end;

  if (heap_end + incr > &heap_limit)
    {
      /* Some of the libstdc++-v3 tests rely upon detecting
     out of memory errors, so do not abort here.  */

	    _write (1, "_sbrk: Heap and stack collision\n", 32);

#if 0
      extern void abort (void);

      _write (1, "_sbrk: Heap and stack collision\n", 32);

      abort ();
#else
      errno = ENOMEM;
      return (caddr_t) -1;
#endif
    }

  heap_end += incr;

  return (caddr_t) prev_heap_end;
}
/*******************************************************************************
End of function _sbrk
*******************************************************************************/

/********************************************************/
int _lseek (int fd, int ptr, int dir)
{
	UNUSED_PARAM(fd);
	UNUSED_PARAM(ptr);
	UNUSED_PARAM(dir);

    return  0;
}


/* Returns # chars not! written.  */
int _swiread (int fh, char * ptr, int len)
{
  int_t   i = low_read(fh, (uint8_t*)ptr, (uint32_t)len);
  return wrap(i);
}


int _swiopen (const char * path, int flags)
{
  int aflags = 0, fh;

  /* It is an error to open a file that already exists. */
  if ((flags & O_CREAT)
      && (flags & O_EXCL))
    {
      struct stat st;
      int res;
      res = _stat (path, &st);
      if (res != -1)
      {
         errno = EEXIST;
         return -1;
      }
    }

  /* The flags are Unix-style, so we need to convert them. */
#ifdef O_BINARY
  if (flags & O_BINARY)
    aflags |= 1;
#endif

  /* In O_RDONLY we expect aflags == 0. */
  if (flags & O_RDWR) {
    aflags |= O_RDWR;
  }
  if (flags & O_WRONLY) {
    aflags |= O_WRONLY;
  }
  if (flags & O_CREAT){
    aflags |= O_CREAT;
  }
  if (flags & O_TRUNC){
    aflags |= O_TRUNC;
  }
  if (flags & O_WRONLY){
    aflags |= O_WRONLY;
  }
  if (flags & O_APPEND){
    aflags |= O_APPEND;
  }

    fh = low_open((char *)path, aflags);

    /* Return a user file descriptor or an error. */
    if (fh >= 0)
    {
      return (fh);
    }
    errno = EEXIST;
    return (-1);
}


int _open (const char * path, int flags, ...)
{
  return wrap (_swiopen (path, flags));
}

/* fh, is a valid internal file handle. */
int _swiclose (int fh)
{
  int   res = low_close(fh);
  return res;
}


/********************************************************/
int _close(int fh)
{
    errno = EBADF;

    /* Attempt to close the handle. */
    errno = _swiclose (fh);
    return wrap (errno);
}

/******************************************************************************
* Function Name: _read
* Description  : GNU interface to low-level I/O read
* Arguments    : int file_no
*              : const char *buffer
*              : unsigned int n
* Return Value : none
******************************************************************************/
int _read(int fh , char *buffer , unsigned int n)
{
      int   iResult = low_read(fh, (uint8_t*) buffer, (uint32_t) n);
      return wrap(iResult);
}


/* fh, is a valid internal file handle.
   Returns the number of bytes *not* written. */
int _swiwrite (int fh, char * ptr, int len)
{
  int_t i;

  i = low_write(fh, (uint8_t*)ptr, (uint32_t)len);

  return wrap(i);
}

/******************************************************************************
* Function Name: _write
* Description  : GNU interface to low-level I/O write
* Arguments    : int file_no
*              : const char *buffer
*              : unsigned int n
* Return Value : none
******************************************************************************/
int _write(int fh , const char *buffer , unsigned int n)
{
    int_t i;

    i = _swiwrite(fh, (char_t *)buffer, (uint16_t)n);

    return (i);
}


int _stat (const char *fname, struct stat *st)
{
	UNUSED_PARAM(fname);
	UNUSED_PARAM(st);

  while(1);
}

/********************************************************/
int _fstat (int fd, struct stat * st)
{
	UNUSED_PARAM(fd);

    /* Always return as charactor device. */
    st->st_mode = S_IFCHR;
    return  0;
}

/********************************************************/
int _isatty(int fd)
{
    if ((fd == STDOUT_FILENO) || (fd == STDERR_FILENO))
    {
        return  1;
    }
    else
    {
        errno = EBADF;
        return  -1;
    }
}

/* End of File */
