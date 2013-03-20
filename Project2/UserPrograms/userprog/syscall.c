#include "userprog/syscall.h"
#include "userprog/process.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "devices/shutdown.h"
#include "filesys/filesys.h"
#include "filesys/file.h"

static void
syscall_handler (struct intr_frame *f UNUSED);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
    int *sp = f->esp;
    switch(*sp)
    {       
        case SYS_HALT:
        {  
            /*void halt (void) 
             * Terminates Pintos by calling shutdown_power_off() (declared in "devices/shutdown.h"). 
             * This should be seldom used, because you lose some information about possible deadlock situations, etc. */
          shutdown_power_off();
        }
        
        case SYS_EXIT:
        {
            /* void exit (int status)
             *  Terminates the current user program, returning status to the kernel. 
             *  If the process's parent waits for it (see below), this is the status that will be returned. 
             *  Conventionally, a status of 0 indicates success and nonzero values indicate errors. 
             */
          thread_current()->status = *(sp+1);
          printf ("%s: exit(%d)\n", thread_name(), thread_current()->status);
          thread_exit();
        }
        
        case SYS_EXEC:
        {
            /* pid_t exec (const char *cmd_line)
             *  Runs the executable whose name is given in cmd_line, passing any given arguments, 
             *  and returns the new process's program id (pid). Must return pid -1, which otherwise 
             *  should not be a valid pid, if the program cannot load or run for any reason. 
             *  Thus, the parent process cannot return from the exec until it knows whether the child 
             *  process successfully loaded its executable. You must use appropriate synchronization to ensure this. 
             */
          f->eax = process_execute((char *) *(sp + 1));
          break;

        }
        
        case SYS_WAIT:
        {
            /* int wait (pid_t pid) */
          f->eax = process_wait (thread_current()->tid);
          break;
        }
        
        case SYS_CREATE:
        { 
            /* bool create (const char *file, unsigned initial_size)
             *  Creates a new file called file initially initial_size bytes in size. 
             *  Returns true if successful, false otherwise. 
             *  Creating a new file does not open it: opening the new file is a separate 
             *  operation which would require a open system call. 
             */
          uint32_t* dir= (uint32_t*)sp;
          char *file = (char*)dir[1];
          unsigned initial_size = (unsigned)dir[2];
          printf("File: %s\nSize: %d\n",file,initial_size);
          return filesys_create(file,initial_size);   
        }
        
        case SYS_REMOVE:
        { 
            /* bool remove (const char *file)
             *  Deletes the file called file. Returns true if successful, false otherwise. 
             *  A file may be removed regardless of whether it is open or closed, and 
             *  removing an open file does not close it. See Removing an Open File, for details. 
             */
         uint32_t* dir= (uint32_t*)sp;
         char *file = (char*)dir[1];
         printf("File: %s\n",file);
         if(filesys_remove(file))
                printf("File %s removed correctly.",file);
         else
                printf("File %s was not removed.\n",file);
        }
        
        case SYS_OPEN: 
        { 
            /*int open (const char *file) */
          struct file *file = filesys_open ((char *) *(sp + 1));
          if(file != NULL)
          {
            //do what needs to be done...
          }
          else
            f->eax = -1;
          break;
        }
        
        case SYS_FILESIZE: 
        {  
            /* int filesize (int fd)
             *  Returns the size, in bytes, of the file open as fd. 
             */
          break;
        }
        
        case SYS_READ: 
        {
            /*int read (int fd, void *buffer, unsigned size)
             * Reads size bytes from the file open as fd into buffer. 
             * Returns the number of bytes actually read (0 at end of file), or -1 if 
             * the file could not be read (due to a condition other than end of file). 
             * Fd 0 reads from the keyboard using input_getc(). */
          break;
        }
        
        case SYS_WRITE: 
        {
            /*int write (int fd, const void *buffer, unsigned size) */
          break;
        }
        
        case SYS_SEEK: 
        {
            /*void seek (int fd, unsigned position) */
          break;
        }
        
        case SYS_TELL: //unsigned
        { 
            /*unsigned tell (int fd)
             * Returns the position of the next byte to be read or written in open file fd, 
             * expressed in bytes from the beginning of the file. 
             */
          break;
        }
        
        case SYS_CLOSE: //void
        {
            /*void close (int fd)
             * Closes file descriptor fd. Exiting or terminating a process implicitly closes 
             * all its open file descriptors, as if by calling this function for each one. */
          break;
        }
    }
}