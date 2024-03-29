#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int sys_fork(void)
{
  return fork();
}

int sys_exit(void)
{
  exit();
  return 0; // not reached
}

int sys_wait(void)
{
  return wait();
}

int sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int sys_getpid(void)
{
  return myproc()->pid;
}

int sys_sbrk(void)
{
  int addr;
  int n;
  struct proc *curproc = myproc();

  if (argint(0, &n) < 0)
    return -1;
  addr = curproc->sz;
  if (growproc(n) < 0)
    return -1;
  // In case that heap collides with stack, keep one page gap
  if (curproc->sz + n > USERSTACKTOP - curproc->stacksize - PGSIZE)
    return -1;
  curproc->sz += n;
  return addr;
}

int sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_fpgn(void)
{
  return get_free_pages_num();
}

int sys_make_shrmem(void)
{
  int sig;
  if (argint(0, &sig) < 0)
  {
    return -1;
  }
  return make_shrmem(sig);
}

int sys_remove_shrmem(void)
{
  int sig;
  if (argint(0, &sig) < 0)
  {
    return -1;
  }
  return remove_shrmem(sig);
}

int sys_read_shrmem(void)
{
  int sig;
  char *content;
  if (argint(0, &sig) < 0 || argptr(1, &content, PGSIZE) < 0)
  {
    return -1;
  }
  return read_shrmem(sig, content);
}

int sys_write_shrmem(void)
{
  int sig;
  char *content;
  if (argint(0, &sig) < 0 || argstr(1, &content) < 0)
  {
    return -1;
  }
  return write_shrmem(sig, content);
}