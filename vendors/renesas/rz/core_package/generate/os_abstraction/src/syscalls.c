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
/* Support files for GNU libc.  Files in the system namespace go here.
   Files in the C namespace (ie those that do not start with an
   underscore) go in .c.  */

#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include "swi.h"

#include "r_typedefs.h"
#include "r_devlink_wrapper.h"
#include "r_os_abstraction_config.h"

/* Forward prototypes.  */
int     _system     _PARAMS ((const char *));
int     _rename     _PARAMS ((const char *, const char *));
int     _isatty		_PARAMS ((int));
clock_t _times		_PARAMS ((struct tms *));
int     _gettimeofday	_PARAMS ((struct timeval *, void *));
void    _raise 		_PARAMS ((void));
int     _unlink		_PARAMS ((const char *));
int     _link 		_PARAMS ((void));
int     _stat 		_PARAMS ((const char *, struct stat *));
int     _fstat 		_PARAMS ((int, struct stat *));
caddr_t _sbrk		_PARAMS ((int));
int     _getpid		_PARAMS ((int));
int     _kill		_PARAMS ((int, int));
void    _exit		_PARAMS ((int));
int     _close		_PARAMS ((int));
int     _swiclose	_PARAMS ((int));
int     _open		_PARAMS ((const char *, int, ...));
int     _swiopen	_PARAMS ((const char *, int));
int     _write 		_PARAMS ((int, char *, int));
int     _swiwrite	_PARAMS ((int, char *, int));
int     _lseek		_PARAMS ((int, int, int));
int     _swilseek	_PARAMS ((int, int, int));
int     _read		_PARAMS ((int, char *, int));
int     _swiread	_PARAMS ((int, char *, int));
void    initialise_monitor_handles _PARAMS ((void));

static int	wrap		_PARAMS ((int));
static int	error		_PARAMS ((int));
static int	get_errno	_PARAMS ((void));
static int	remap_handle	_PARAMS ((int));
static int 	findslot	_PARAMS ((int));

/* Register name faking - works in collusion with the linker.  */
register char * stack_ptr asm ("sp");


/* following is copied from libc/stdio/local.h to check std streams */
extern void   _EXFUN(__sinit,(struct _reent *));
#define CHECK_INIT(ptr) \
  do						\
    {						\
      if ((ptr) && !(ptr)->__sdidinit)		\
	__sinit (ptr);				\
    }						\
  while (0)

/* Adjust our internal handles to stay away from std* handles.  */
#define FILE_HANDLE_OFFSET (0x00)


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

#define MAX_OPEN_FILES 20
static poslog openfiles [MAX_OPEN_FILES];

static int
findslot (int fh)
{
  int i;
  for (i = 0; i < MAX_OPEN_FILES; i ++)
    if (openfiles[i].handle == fh)
      break;
  return i;
}

/* Function to convert std(in|out|err) handles to internal versions.  */
static int
remap_handle (int fh)
{
  CHECK_INIT(_REENT);

  if (fh == STDIN_FILENO)
    return monitor_stdin;
  if (fh == STDOUT_FILENO)
    return monitor_stdout;
  if (fh == STDERR_FILENO)
    return monitor_stderr;

  return fh - FILE_HANDLE_OFFSET;
}

void
initialise_monitor_handles (void)
{
  int i;
  int fh;
  char * name;

#if R_OS_ENABLE_VIRTUAL_CONSOLE
  int volatile block[3];

  name = "\\\\.\\stdin";
  fh = low_open(name, O_RDWR);    /* Reserve file handle for devlink */  
  block[0] = (int) ":tt";
  block[2] = 3;     /* length of filename */
  block[1] = 0;     /* mode "r" */
  monitor_stdin = do_AngelSWI (AngelSWI_Reason_Open, (void *) block);

  block[0] = (int) ":tt";
  block[2] = 3;     /* length of filename */
  block[1] = 4;     /* mode "w" */
  monitor_stdout = monitor_stderr = do_AngelSWI (AngelSWI_Reason_Open, (void *) block);

  name = "\\\\.\\stdin";
  fh = low_open(name, O_RDWR);    /* Reserve file handle for devlink */
  name = "\\\\.\\stdout";
  fh = low_open(name, O_WRONLY);  /* Reserve file handle for devlink */
  name = "\\\\.\\stderr";
  fh = low_open(name, O_WRONLY);  /* Reserve file handle for devlink */
#else
  name = "\\\\.\\stdin";
  fh = low_open(name, O_RDWR);
  monitor_stdin = fh;
  name = "\\\\.\\stdout";
  fh = low_open(name, O_WRONLY);
  monitor_stdout = fh;
  name = "\\\\.\\stderr";
  fh = low_open(name, O_WRONLY);
#endif

  for (i = 0; i < MAX_OPEN_FILES; i ++)
    openfiles[i].handle = -1;

  openfiles[0].handle = monitor_stdin;
  openfiles[0].pos = 0;
  openfiles[1].handle = monitor_stdout;
  openfiles[1].pos = 0;
  openfiles[2].handle = monitor_stdout;
  openfiles[2].pos = 0;
}

static int
get_errno (void)
{
#if R_OS_ENABLE_VIRTUAL_CONSOLE
  return do_AngelSWI (AngelSWI_Reason_Errno, NULL);
#endif
}

static int
error (int result)
{
  errno = get_errno ();
  return result;
}

static int
wrap (int result)
{
  if (result == -1)
    return error (-1);
  return result;
}

/* Returns # chars not! written.  */
int
_swiread (int file,
	  char * ptr,
	  int len)
{
#if R_OS_ENABLE_VIRTUAL_CONSOLE
  int fh = remap_handle (file);
  int block[3];
  
  block[0] = fh;
  block[1] = (int) ptr;
  block[2] = len;
  
  return do_AngelSWI (AngelSWI_Reason_Read, block);
#endif
}

int __attribute__((weak))
_read (int file,
       char * ptr,
       int len)
{
#if R_OS_ENABLE_VIRTUAL_CONSOLE
  if ((file == STDIN_FILENO) || (file == STDOUT_FILENO) || (file == STDERR_FILENO))
  {
    int slot = findslot (remap_handle (file));
    int x = _swiread (file, ptr, len);

    if (x < 0)
      return error (-1);

    if (slot != MAX_OPEN_FILES)
      openfiles [slot].pos += len - x;

    /* x == len is not an error, at least if we want feof() to work.  */
    return len - x;
  }
  else
  {
    int_t   i = low_read(file, (uint8_t*)ptr, (uint32_t)len);
    return wrap(i);
  }
#else
  int_t   i = low_read(file, (uint8_t*)ptr, (uint32_t)len);
  return wrap(i);
#endif
}

int
_swilseek (int file,
	   int ptr,
	   int dir)
{
  int res = 0;
  int fh = remap_handle (file);
  int slot = findslot (fh);
#if R_OS_ENABLE_VIRTUAL_CONSOLE
  int block[2];
#endif

  if (dir == SEEK_CUR)
    {
      if (slot == MAX_OPEN_FILES)
	return -1;
      ptr = openfiles[slot].pos + ptr;
      dir = SEEK_SET;
    }
  
#if R_OS_ENABLE_VIRTUAL_CONSOLE
  if (dir == SEEK_END)
    {
      block[0] = fh;
      ptr += do_AngelSWI (AngelSWI_Reason_FLen, block);
    }
  
  /* This code only does absolute seeks.  */
  block[0] = remap_handle (file);
  block[1] = ptr;
  res = do_AngelSWI (AngelSWI_Reason_Seek, block);
#endif

  if (slot != MAX_OPEN_FILES && res == 0)
    openfiles[slot].pos = ptr;

  /* This is expected to return the position in the file.  */
  return res == 0 ? ptr : -1;
}

int
_lseek (int file,
	int ptr,
	int dir)
{
  return wrap (_swilseek (file, ptr, dir));
}

/* Returns #chars not! written.  */
int
_swiwrite (
	   int    file,
	   char * ptr,
	   int    len)
{
#if R_OS_ENABLE_VIRTUAL_CONSOLE
  int fh = remap_handle (file);

  int block[3];
  
  block[0] = fh;
  block[1] = (int) ptr;
  block[2] = len;
  
  return do_AngelSWI (AngelSWI_Reason_Write, block);



#endif
}

int __attribute__((weak))
_write (int    file,
	char * ptr,
	int    len)
{
#if R_OS_ENABLE_VIRTUAL_CONSOLE
  if ((file == STDIN_FILENO) || (file == STDOUT_FILENO) || (file == STDERR_FILENO))
  {
    int slot = findslot (remap_handle (file));
    int x = _swiwrite (file, ptr,len);

    if (x == -1 || x == len)
      return error (-1);
    
    if (slot != MAX_OPEN_FILES)
      openfiles[slot].pos += len - x;
    
    return len - x;
  }
  else
  {
    int_t i;

    i = low_write(file, (uint8_t*)ptr, (uint32_t)len);

    return wrap(i);
  }
#else
  int_t i;

  i = low_write(file, (uint8_t*)ptr, (uint32_t)len);

  return wrap(i);
#endif
}

extern int strlen (const char *);

int
_swiopen (const char * path,
	  int          flags)
{
  int fh;

  int i = findslot (-1);
  
  if (i == MAX_OPEN_FILES)
    return -1;


  fh = low_open((char *)path, flags);

  if (fh >= 0)
    {
      openfiles[i].handle = fh;
      openfiles[i].pos = 0;
    }

  return fh >= 0 ? fh + FILE_HANDLE_OFFSET : error (fh);
}

int
_open (const char * path,
       int          flags,
       ...)
{
  return wrap (_swiopen (path, flags));
}

int
_swiclose (int file)
{
  int res;
#if R_OS_ENABLE_VIRTUAL_CONSOLE
  int myhan = remap_handle (file);
  int slot = findslot (myhan);

  
  if (slot != MAX_OPEN_FILES)
    openfiles[slot].handle = -1;

  res = low_close(myhan);
#else
  int slot = findslot (file);

  if (slot != MAX_OPEN_FILES)
    openfiles[slot].handle = -1;

  res = low_close(file);
#endif
  return res;
}

int
_close (int file)
{
  return wrap (_swiclose (file));
}

int
_kill (int pid, int sig)
{
  (void)pid; (void)sig;
#if R_OS_ENABLE_VIRTUAL_CONSOLE
  /* Note: The pid argument is thrown away.  */
  switch (sig) {
	  case SIGABRT:
		  return do_AngelSWI (AngelSWI_Reason_ReportException,
				  (void *) ADP_Stopped_RunTimeError);
	  default:
		  return do_AngelSWI (AngelSWI_Reason_ReportException,
				  (void *) ADP_Stopped_ApplicationExit);
  }
#endif
}

void
_exit (int status)
{
  /* There is only one SWI for both _exit and _kill. For _exit, call
     the SWI with the second argument set to -1, an invalid value for
     signum, so that the SWI handler can distinguish the two calls.
     Note: The RDI implementation of _kill throws away both its
     arguments.  */
  _kill(status, -1);
}

int
_getpid (int n)
{
  return 1;
  n = n;
}

caddr_t __attribute__((weak))
_sbrk (int incr)
{
  extern char   end asm ("__heap_start");           /* Defined by the linker.  */
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
#if 0
      extern void abort (void);

      _write (1, "_sbrk: Heap and stack collision\n", 32);
      
      abort ();
#endif
    }
  
  heap_end += incr;

  return (caddr_t) prev_heap_end;
}

extern void memset (struct stat *, int, unsigned int);

int
_fstat (int file, struct stat * st)
{
  memset (st, 0, sizeof (* st));
  st->st_mode = S_IFCHR;
  st->st_blksize = 1024;
  return 0;
  file = file;
}

int _stat (const char *fname, struct stat *st)
{
  int file;

  /* The best we can do is try to open the file readonly.  If it exists,
     then we can guess a few things about it.  */
  if ((file = _open (fname, O_RDONLY)) < 0)
    return -1;

  memset (st, 0, sizeof (* st));
  st->st_mode = S_IFREG | S_IREAD;
  st->st_blksize = 1024;
  _swiclose (file); /* Not interested in the error.  */
  return 0;
}

int
_link (void)
{
  return -1;
}

int
_unlink (const char *path)
{
#if R_OS_ENABLE_VIRTUAL_CONSOLE
  int block[2];
  block[0] = path;
  block[1] = strlen(path);
  return wrap (do_AngelSWI (AngelSWI_Reason_Remove, block)) ? -1 : 0;
#endif
}

void
_raise (void)
{
  return;
}

int
_gettimeofday (struct timeval * tp, void * tzvp)
{
  struct timezone *tzp = tzvp;
  if (tp)
    {
    /* Ask the host for the seconds since the Unix epoch.  */
#if R_OS_ENABLE_VIRTUAL_CONSOLE
      tp->tv_sec = do_AngelSWI (AngelSWI_Reason_Time,NULL);
#endif
      tp->tv_usec = 0;
    }

  /* Return fixed data for the timezone.  */
  if (tzp)
    {
      tzp->tz_minuteswest = 0;
      tzp->tz_dsttime = 0;
    }

  return 0;
}

/* Return a clock that ticks at 100Hz.  */
clock_t 
_times (struct tms * tp)
{
  clock_t timeval = 0;

#if R_OS_ENABLE_VIRTUAL_CONSOLE
  timeval = do_AngelSWI (AngelSWI_Reason_Clock,NULL);
#endif

  if (tp)
    {
      tp->tms_utime  = timeval;	/* user time */
      tp->tms_stime  = 0;	/* system time */
      tp->tms_cutime = 0;	/* user time, children */
      tp->tms_cstime = 0;	/* system time, children */
    }
  
  return timeval;
};


int
_isatty (int fd)
{
#if R_OS_ENABLE_VIRTUAL_CONSOLE
  int fh = remap_handle (fd);
  return wrap (do_AngelSWI (AngelSWI_Reason_IsTTY, &fh));
#else
  if ((fd == STDIN_FILENO) || (fd == STDOUT_FILENO))
  {
    return 1;
  }
  else
  {
    return 0;
  }
#endif
}

int
_system (const char *s)
{
#if R_OS_ENABLE_VIRTUAL_CONSOLE
  int block[2];
  int e;

  /* Hmmm.  The ARM debug interface specification doesn't say whether
     SYS_SYSTEM does the right thing with a null argument, or assign any
     meaning to its return value.  Try to do something reasonable....  */
  if (!s)
    return 1;  /* maybe there is a shell available? we can hope. :-P */
  block[0] = s;
  block[1] = strlen (s);
  e = wrap (do_AngelSWI (AngelSWI_Reason_System, block));
  if ((e >= 0) && (e < 256))
    {
      /* We have to convert e, an exit status to the encoded status of
         the command.  To avoid hard coding the exit status, we simply
	 loop until we find the right position.  */
      int exit_code;

      for (exit_code = e; e && WEXITSTATUS (e) != exit_code; e <<= 1)
	continue;
    }
  return e;
#endif
}

int
_rename (const char * oldpath, const char * newpath)
{
#if R_OS_ENABLE_VIRTUAL_CONSOLE
  int block[4];
  block[0] = oldpath;
  block[1] = strlen(oldpath);
  block[2] = newpath;
  block[3] = strlen(newpath);
  return wrap (do_AngelSWI (AngelSWI_Reason_Rename, block)) ? -1 : 0;
#endif
}
