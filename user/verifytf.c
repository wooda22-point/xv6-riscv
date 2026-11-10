#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

#define MAX_BLOCKS 1024
#define CHUNK_SIZE 32

static void *blocks [MAX_BLOCKS];

int main(void) {
  int count = 0;
  int previous_frames;
  int current_frames;
  printf("First Allocation:\n");
  // Allocate the first block
  void *ptr = block_alloc(28);

  if (ptr == 0) {
    printf("Allocation failed\n");
    exit(1);
  }

  blocks[count++] = ptr;

  // calculate the page's starting address
  uint64 page = (uint64)ptr & ~(PGSIZE - 1);

  printf("Allocated 32-byte chunk at %p (page %p)\n", ptr, (void *)page);

  previous_frames = trackframes();

  printf("Process page table has %d frames.\n", previous_frames);

  // allocate until the number of frames increases
  while (count < MAX_BLOCKS) {
    ptr = block_alloc(28);
    if (ptr == 0) {
      printf("Allocation failed\n");
      break;
    }

    blocks[count++] = ptr;
    page = (uint64)ptr & ~(PGSIZE - 1);
    printf("Allocated 32-byte chunk at %p (page %p)\n", ptr, (void *)page);
    current_frames = trackframes();
    printf("Current page count: %d\n", current_frames);
    if (current_frames > previous_frames) {
      break;
    }
    previous_frames = current_frames;
  }
  printf("Frames allocated: %d\n", count);
  printf("======================================= Beginning block_free\n");
  printf("============================\n");

  // free every allocated block
  for (int i = 0; i < count; i++) {
    page = (uint64)blocks[i] & ~(PGSIZE - 1);
    block_free(blocks[i]);
    printf("Freed 32-byte chunk at %p (page %p)\n", blocks[i], (void *) page);
  }
  printf("All blocks freed.\n");
  exit(0);
}
