// Per-CPU state
struct cpu {
  uchar apicid;                // Local APIC ID
  struct context *scheduler;   // swtch() here to enter scheduler
  struct taskstate ts;         // Used by x86 to find stack for interrupt
  struct segdesc gdt[NSEGS];   // x86 global descriptor table
  volatile uint started;       // Has the CPU started?
  int ncli;                    // Depth of pushcli nesting.
  int intena;                  // Were interrupts enabled before pushcli?
  struct proc *proc;           // The process running on this cpu or null
};

extern struct cpu cpus[NCPU];
extern int ncpu;

//PAGEBREAK: 17
// Saved registers for kernel context switches.
// Don't need to save all the segment registers (%cs, etc),
// because they are constant across kernel contexts.
// Don't need to save %eax, %ecx, %edx, because the
// x86 convention is that the caller has saved them.
// Contexts are stored at the bottom of the stack they
// describe; the stack pointer is the address of the context.
// The layout of the context matches the layout of the stack in swtch.S
// at the "Switch stacks" comment. Switch doesn't save eip explicitly,
// but it is on the stack and allocproc() manipulates it.
struct context {
  uint edi;
  uint esi;
  uint ebx;
  uint ebp;
  uint eip;
};

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// 物理页表项数
#define NUM_MEM_PAGE_ENTRIES 256

// 虚拟页表项数
#define NUM_VIRTUAL_PAGE_ENTRIES 64

// 虚拟页表的偏移量
#define VIRTUAL_PAGE_OFFSET (NUM_VIRTUAL_PAGE_ENTRIES * PGSIZE)

// 物理内存页表页数
#define NUM_MEM_PAGES 32

// 物理内存总页表项数
#define MEM_CAPACITY (NUM_MEM_PAGE_ENTRIES * NUM_MEM_PAGES)

// 虚拟页面文件最大大小
#define VPFILE_SIZE 65536

// 虚拟页面文件数量
#define MAX_VPFILES 4

// 记录物理内存和虚拟内存的双向链表
struct mem_page_entry
{
  char *vaddr;
  struct mem_page_entry *next;
  struct mem_page_entry *prev;
};

struct virtual_page_entry
{
  char *vaddr;
};

struct mem_page
{
  struct mem_page *prev;
  struct mem_page *next;
  struct mem_page_entry entries[NUM_MEM_PAGE_ENTRIES];
};

struct virtual_page
{
  struct virtual_page *prev;
  struct virtual_page *next;
  struct virtual_page_entry entries[NUM_VIRTUAL_PAGE_ENTRIES];
};

// 每个进程最多占用128页共享内存
#define PROC_SHR_MEM_NUM 128

// Per-process state
struct proc {
  uint sz;                     // Size of process memory (bytes)
  pde_t* pgdir;                // Page table
  char *kstack;                // Bottom of kernel stack for this process
  enum procstate state;        // Process state
  int pid;                     // Process ID
  struct proc *parent;         // Parent process
  struct trapframe *tf;        // Trap frame for current syscall
  struct context *context;     // swtch() here to run process
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
  uint stacksize;              // Process stack size
  int stack_growing;           // if the stack is growing

  int num_mem_page_entries;         // 物理页面项数
  int num_virtual_page;        // 虚拟页数


	struct file *vpfile[MAX_VPFILES];     //虚拟内存文件

  //双向链表，包括物理内存页表、虚拟内存页表
	struct mem_page *mem_page_head;
  struct mem_page *mem_page_tail;
  
  struct mem_page_entry *mem_queue_head;
  struct mem_page_entry *mem_queue_tail;

  struct virtual_page *virtual_page_head;
  struct virtual_page *virtual_page_tail;

  int shrmem_sigs[PROC_SHR_MEM_NUM];

};

// 共享内存结构体
struct share_memory
{
  void *addr;
  int sig;
};

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap


// 系统共享内存大小：32768页
#define SHR_MEM_NUM 32768
