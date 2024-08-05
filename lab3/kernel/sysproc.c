#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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
  int n;
  uint ticks0;


  argint(0, &n);
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
  int number;
  uint64 bitmask;
  argaddr(0, &va) ;
  argaddr(2, &bitmask);
  argint(1, &number) ;

  struct proc* p=myproc();
  pagetable_t pagetable=p->pagetable;
  uint64 res=0;
  //printf("%d",number);
  for (int i=0;i<number;i++){
    if(va>MAXVA){
    	printf("NO @_@\n");
    	panic("Over range");
    }
    //printf("%d: %p\n",i,va);
    pte_t *pte=walk(pagetable,va,0);
    if(pte == 0 || (*pte & PTE_V) == 0 ||(*pte & PTE_U) == 0)
      continue;
    if(*pte & PTE_A) {  
      res = res | (1L << i);    //set
      *pte = *pte & (~PTE_A);   //clear
    }
    //printf("Done\n");
    va+=PGSIZE;
  }
	//printf("Done\n");
  return copyout(p->pagetable,bitmask, (char*) &res, sizeof(int)); 
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

