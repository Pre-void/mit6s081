// Saved registers for kernel context switches.
struct context {
  uint64 ra;
  uint64 sp;

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

// Per-CPU state.
struct cpu {
  struct proc *proc;          // The process running on this cpu, or null.
  struct context context;     // swtch() here to enter scheduler().
  int noff;                   // Depth of push_off() nesting.
  int intena;                 // Were interrupts enabled before push_off()?
};

extern struct cpu cpus[NCPU];

// per-process data for the trap handling code in trampoline.S.
// sits in a page by itself just under the trampoline page in the
// user page table. not specially mapped in the kernel page table.
// the sscratch register points here.
// uservec in trampoline.S saves user registers in the trapframe,
// then initializes registers from the trapframe's
// kernel_sp, kernel_hartid, kernel_satp, and jumps to kernel_trap.
// usertrapret() and userret in trampoline.S set up
// the trapframe's kernel_*, restore user registers from the
// trapframe, switch to the user page table, and enter user space.
// the trapframe includes callee-saved user registers like s0-s11 because the
// return-to-user path via usertrapret() doesn't return through
// the entire kernel call stack.

/**是一个用于存储中断或异常发生时的处理现场信息的数据结构。当发生中断、异常或系统调用时，
 * 操作系统会保存当前被中断程序的状态到一个 trapframe 中，以便在中断处理程序或异常处理程序中能够恢复被中断程序的执行。**/
struct trapframe {
  /*   0 */ uint64 kernel_satp;   // kernel page table 可以通过访问 kernel_satp 寄存器来获取中断前的内核页表信息。
  /*   8 */ uint64 kernel_sp;     // top of process's kernel stack   通过访问 kernel_sp 字段来找到中断前的内核栈的栈顶。
  /*  16 */ uint64 kernel_trap;   // usertrap()            用于表示中断或异常发生时应该调用的处理函数。
  /*  24 */ uint64 epc;           // saved user program counter  程序计数器（Program Counter）的值，即中断或异常发生时用户程序的下一条指令地址。
  /*  32 */ uint64 kernel_hartid; // saved kernel tp   保存内核 tp 寄存器的值，表示当前硬件线程（hart）的标识符。
  /*  40 */ uint64 ra;
  /*  48 */ uint64 sp;
  /*  56 */ uint64 gp;
  /*  64 */ uint64 tp;
  /*  72 */ uint64 t0;
  /*  80 */ uint64 t1;
  /*  88 */ uint64 t2;
  /*  96 */ uint64 s0;
  /* 104 */ uint64 s1;
  /* 112 */ uint64 a0;
  /* 120 */ uint64 a1;
  /* 128 */ uint64 a2;
  /* 136 */ uint64 a3;
  /* 144 */ uint64 a4;
  /* 152 */ uint64 a5;
  /* 160 */ uint64 a6;
  /* 168 */ uint64 a7;
  /* 176 */ uint64 s2;
  /* 184 */ uint64 s3;
  /* 192 */ uint64 s4;
  /* 200 */ uint64 s5;
  /* 208 */ uint64 s6;
  /* 216 */ uint64 s7;
  /* 224 */ uint64 s8;
  /* 232 */ uint64 s9;
  /* 240 */ uint64 s10;
  /* 248 */ uint64 s11;
  /* 256 */ uint64 t3;
  /* 264 */ uint64 t4;
  /* 272 */ uint64 t5;
  /* 280 */ uint64 t6;
};
/**UNUSED 表示进程结构未被使用，即没有关联的活跃进程。
 * SLEEPING 阻塞态 可能在等待I/O
 * RUNNABLE 就绪态
 * RUNNING  正在运行，占用cpu时间
 * ZOMBIE   进程已经终止
 * **/
enum procstate { UNUSED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  struct spinlock lock;        // 进程锁，用于在多处理器环境中对进程结构进行互斥访问

  // p->lock must be held when using these:   需要持有p->lock的情况喜爱才可以访问
  enum procstate state;        // Process state    进程状态，表示进程当前所处的状态
  struct proc *parent;         // Parent process   父进程的指针
  void *chan;                  // If non-zero, sleeping on chan  如果非零，表示进程正在休眠，等待在 chan 上
  int killed;                  // If non-zero, have been killed  如果非零，表示进程已被杀死
  int xstate;                  // Exit status to be returned to parent's wait  退出状态，将返回给父进程的 wait
  int pid;                     // Process ID  进程 ID

  // these are private to the process, so p->lock need not be held.
  uint64 kstack;               // Virtual address of kernel stack   内核栈的虚拟地址
  uint64 sz;                   // Size of process memory (bytes)    进程内存的大小（字节）
  pagetable_t pagetable;       // User page table   用户页表
  struct trapframe *trapframe; // data page for trampoline.S        trampoline.S 的数据页，用于进程切换
  struct context context;      // swtch() here to run process       进程上下文，用于保存进程的寄存器状态
  struct file *ofile[NOFILE];  // Open files                        打开的文件数组
  struct inode *cwd;           // Current directory                 当前目录
  char name[16];               // Process name (debugging)          进程名称
  uint64 tracemask;            // 用于追踪系统调用的掩码
};
