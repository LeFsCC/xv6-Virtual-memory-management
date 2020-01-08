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

// 相关参数定义
#define NUM_MEMSTAB_PAGE_ENTRIES 256

#define NUM_VPSTAB_PAGE_ENTRIES 512

#define VPSTAB_PAGE_OFFSET (NUM_VPSTAB_PAGE_ENTRIES * PGSIZE)

#define NUM_MEMSTAB_PAGES 32

#define NUM_MEMSTAB_ENTRIES_CAPACITY (NUM_MEMSTAB_PAGE_ENTRIES * NUM_MEMSTAB_PAGES)

// 虚拟页面文件最大大小
#define VPFILE_LIMIT 65536

// 虚拟页面文件最大数量
#define MAX_VPFILES 4

// 相关链表定义
struct memstab_page_entry
{
  char *vaddr;
  struct memstab_page_entry *next;
  struct memstab_page_entry *prev;
};

struct vpstab_page_entry
{
  char *vaddr;
};

struct memstab_page
{
  struct memstab_page *prev;
  struct memstab_page *next;
  struct memstab_page_entry entries[NUM_MEMSTAB_PAGE_ENTRIES];
};

struct vpstab_page
{
  struct vpstab_page *prev;
  struct vpstab_page *next;
  struct vpstab_page_entry entries[NUM_VPSTAB_PAGE_ENTRIES];
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

  int num_mem_entries;         // 物理页数
  int num_vpstab_pages;        // 虚拟页数


	struct file *vpfile[MAX_VPFILES];     //虚拟内存文件

  //双向链表，包括物理内存页表、虚拟内存页表
	struct memstab_page *memstab_head;
  struct memstab_page *memstab_tail;
  struct memstab_page_entry *memqueue_head;
  struct memstab_page_entry *memqueue_tail;

  struct vpstab_page *vpstab_head;
  struct vpstab_page *vpstab_tail;

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
