#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  backtrace();
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  uint64 va;
  int num;
  unsigned int bitmask = 0;
  uint64 addr;
  argaddr(0, &va);
  argint(1, &num);
  argaddr(2, &addr);
  struct proc *p = myproc();
  pte_t *pte;
  for (int i = 0; i < num; i++) {
    if (va > MAXVA) {
      return -1;
    }
    pte = walk(p->pagetable, va, 0);
    if (!pte) {
      return -1;
    }
    if (*pte & PTE_A) {
      bitmask |= (1 << i);
      *pte &= (~PTE_A);
    }
    va = va + PGSIZE;
  }
  if (copyout(p->pagetable, addr, (char*)&bitmask, sizeof(bitmask)) < 0) {
    return -1;
  }
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  int mask;
  argint(0, &mask);
  myproc()->mask = mask;
  return 0;
}

uint64
sys_sysinfo(void)
{
  struct proc *p = myproc();
  uint64 addr;
  struct sysinfo info;
  argaddr(0, &addr);
  info.freemem = getfreemem();
  info.nproc = getprocnum();
  if (copyout(p->pagetable, addr, (char*)&info, sizeof(info)) < 0) {
    return -1;
  }
  return 0;
}

uint64
sys_sigalarm(void)
{
  int interval;
  argint(0, &interval);
  if (interval < 0) {
    return -1;
  }
  uint64 addr;
  argaddr(1, &addr);
  struct proc *p = myproc();
  p->interval = interval;
  p->handler = (void*)addr;
  return 0;
}

uint64
sys_sigreturn(void)
{
  struct proc *p = myproc();
  if (p->handling) {
    memmove(p->trapframe, p->handler_trapframe, sizeof(struct trapframe));
  }
  p->handling = 0;
  return p->trapframe->a0;
}