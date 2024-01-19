#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
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

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
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


/**sys_trace 函数的目的是允许用户通过系统调用设置进程的系统调用追踪掩码。
 * 用户传递一个整数作为掩码，函数将这个整数与当前进程的追踪掩码进行按位或操作，
 * 以添加需要追踪的系统调用。这样，内核就能够在相应系统调用返回时记录追踪信息。**/
uint64
sys_trace(void){
    int trace_sys_mask;
    /**argint用于从用户程序传递给系统调用的参数中提取整数值。
     * 如果提取成功返回值为0
     * 提取失败返回值为   -1**/
    if(argint(0,&trace_sys_mask) < 0){
        return -1;
    }
    /**输出一下用户程序传递的参数**/
//    printf("hhhhhhhhhhhhhhh===%d\n",trace_sys_mask);
    /**使用按位或运算 |= 将获取到的系统调用掩码合并到进程的 tracemask 中**/
    myproc()->tracemask |= trace_sys_mask;
    return 0;
}

uint64
sys_sysinfo(void){

    struct proc * my_proc = myproc();
    uint64 p;
    if(argaddr(0,&p)<0){
        return -1;
    }

    struct sysinfo s;
    s.freemem = kfreemem();
    s.nproc  = count_free_proc();

    if(copyout(my_proc->pagetable,p,(char *)&s, sizeof(s)) < 0){
        return -1;
    }

    return 0;
}