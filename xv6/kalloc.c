// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
  // free pages number
  uint fpgn;
  // page count share, indicate the times that one page shared by child process
  ushort pc_sh[PHYSTOP >> PGSHIFT];
} kmem;

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  kmem.fpgn = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE){
    kfree(p);
    kmem.pc_sh[V2P(p) >> PGSHIFT] = 0;
  }
}
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  

  if(kmem.use_lock)
    acquire(&kmem.lock);
  if(kmem.pc_sh[V2P(v) >> PGSHIFT] > 0) {
      kmem.pc_sh[V2P(v) >> PGSHIFT] -= 1;
  }

  if (kmem.pc_sh[V2P(v) >> PGSHIFT] == 0) {
  	  memset(v, 1, PGSIZE);
  	  kmem.fpgn += 1;
  	  r = (struct run*)v;
	  r->next = kmem.freelist;
	  kmem.freelist = r;
  }

  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r) {
    kmem.fpgn -= 1;
    kmem.freelist = r->next;
    kmem.pc_sh[V2P((void *)r) >> PGSHIFT] = 1;
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

uint get_free_pages_num(void) {
  uint t = 0;
  acquire(&kmem.lock);
  t = kmem.fpgn;
  release(&kmem.lock);
  return t;
}

void add_page_share(int pg_addr) {
   if (pg_addr > PHYSTOP || pg_addr < (uint)V2P(end))
      panic("add_page_share");

   acquire(&kmem.lock);
   kmem.pc_sh[pg_addr >> PGSHIFT] += 1;
   release(&kmem.lock);
}

void red_page_share(int pg_addr) {
   if (pg_addr > PHYSTOP || pg_addr < (uint)V2P(end))
      panic("red_page_share");

   acquire(&kmem.lock);
   kmem.pc_sh[pg_addr >> PGSHIFT] -= 1;
   release(&kmem.lock);
}

ushort get_page_share(int pg_addr) {
   if (pg_addr > PHYSTOP || pg_addr < (uint)V2P(end))
     panic("get page share!");
 
   ushort pcs = 0;
   acquire(&kmem.lock);
   pcs = kmem.pc_sh[pg_addr >> PGSHIFT];
   release(&kmem.lock);
   return pcs;
}