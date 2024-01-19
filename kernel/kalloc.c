// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};
/**用于内核内存管理的结构体。**/
struct {
  /**自旋锁,确保在同一时刻只有一个线程能够执行对 kmem 结构体的操作，以防止竞态条件。**/
  struct spinlock lock;
  /**指向空闲内存块的链表。这些内存块可以在需要时被分配给内核或用户程序。**/
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;
  /**获取内存分配的锁**/
  acquire(&kmem.lock);
  /**保存链表头部的**/
  r = kmem.freelist;
  /**如果不为空，将链表头后移，表示取出了一个空闲链表**/
  if(r)
    kmem.freelist = r->next;
  /**释放锁**/
  release(&kmem.lock);

  /**将这个页的内容设置为5**/
  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}


uint64
kfreemen(void){
    struct run * r;
    uint64 free = 0;   //  空闲页面的大小
    acquire(&kmem.lock);
    /**遍历空闲链表，计算总共的空闲大小**/
    r = kmem.freelist;
    while (r){
        free += PGSIZE;
        r = r->next;
    }
    release(&kmem.lock);
    /**返回总共空闲链表的大小**/
    return free;
}