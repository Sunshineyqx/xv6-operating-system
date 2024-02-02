// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

#define kmemindex(pa)  ((((uint64)pa) - ((uint64)KERNBASE)) / (PGSIZE))

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct{
  struct spinlock lock;
  int cnt;
}kmemref[(PHYSTOP - KERNBASE) / PGSIZE + 1];

void
kinit()
{
  for(int i = 0; i < (PHYSTOP - KERNBASE) / PGSIZE + 1; i++){
    initlock(&kmemref[i].lock, "kmem_ref");
  }
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
    kmemref[kmemindex(p)].cnt = 1;
    kfree(p);
  }
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

  //判断引用计数
  int index = kmemindex(pa);
  acquire(&kmemref[index].lock);
  kmemref[index].cnt--;
  if(kmemref[index].cnt > 0){
    release(&kmemref[index].lock);
    return;
  }
  release(&kmemref[index].lock);

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

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
  {
    //设置新页面的引用计数
    int index = kmemindex(r);
    acquire(&kmemref[index].lock);
    kmemref[index].cnt = 1;
    release(&kmemref[index].lock);
    kmem.freelist = r->next;
  }
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

int
getcowref(uint64 pa){
  return kmemref[kmemindex(pa)].cnt;
}

void
increcowref(uint64 pa){
  int index = kmemindex(pa);

  acquire(&kmemref[index].lock);
  ++kmemref[index].cnt;
   release(&kmemref[index].lock);
}

void decrecowref(uint64 pa){
  kfree((void*)pa);
}
