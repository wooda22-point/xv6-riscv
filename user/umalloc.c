#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/riscv.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

// constants for num of size and max block pages
#define NUM_CLASSES 9
#define MAX_BLOCK_PAGES 256

// individual free block
struct block {
  struct block *next;
};

// Each index is a different block size
static struct block *free_list[NUM_CLASSES] = {0};

// struct for page info
struct page_info {
  char *start;
  int index;
};

// keep tracks of the pages for easier deallocation
static struct page_info pages[MAX_BLOCK_PAGES];

// stores the page count
static int page_count = 0;

// helper function to get the size of pages using an index
static int get_class(int nbytes) {
  // cant use if less than zero
  if (nbytes <= 0) {
    return -1;
  }

  // default size
  int size = 16;

  // checks the size and returns the index from 0-8
  for (int i = 0; i < NUM_CLASSES; i++) {
    if (nbytes <= size) {
      return i;
    }
    size *= 2;
  }
  // fail safe incase of no size matching
  return -1;
}

// allocates blocks of memory to specification of assignment
void * block_alloc(int nbytes) {
  // gets and checks index
  int index = get_class(nbytes);

  if (index < 0) {
    return 0;
  }
  // epic bit shift to get easy size since all are powers of two
  int size = 16 << index;

  // allocate a new page if the free list is empty
  if (free_list[index] == 0) {
    if (page_count >= MAX_BLOCK_PAGES) {
      return 0;
    }
    char *page = sbrk(PGSIZE);
    if (page == SBRK_ERROR) {
      return 0;
    }
    // update the page info array
    pages[page_count].start = page;
    pages[page_count].index = index;
    page_count++;
    // divide the page into equally sized blocks
    int count = PGSIZE / size;
    for (int i = 0; i < count; i++) {
      // i HATE pointers
      struct block *b = (struct block *)(page + i * size);
      b->next = free_list[index];
      free_list[index] = b;
    }
  }
  // remove a block from the free list
  struct block *b = free_list[index];

  free_list[index] = b->next;

  return (void *)b;
}

// frees the blocks that were allocated
void block_free (void *ptr) {
  if (ptr == 0) {
    return;
  }

  // Find which allocated page contains this pointer
  for (int i = 0; i < page_count; i++) {
    char *start = pages[i].start;
    char *end = start + PGSIZE;
    // more pointer pain
    if ((char *)ptr >= start && (char *)ptr < end) {
      int index = pages[i].index;
      // return the block to it size class's free list
      struct block *b = (struct block *)ptr;

      b->next = free_list[index];
      free_list[index] = b;

      return;
    }
  }
}

void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

static Header *
morecore(uint nu)
{
  char *p;
  Header *hp;

  if (nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
  if (p == SBRK_ERROR)
    return 0;
  hp = (Header *)p;
  hp->s.size = nu;
  free((void *)(hp + 1));
  return freep;
}

void *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
  if ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    if (p->s.size >= nunits) {
      if (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      return (void *)(p + 1);
    }
    if (p == freep)
      if ((p = morecore(nunits)) == 0)
        return 0;
  }
}
