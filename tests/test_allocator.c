#include "test_allocator.h"

#include <assert.h>
#include <stdlib.h>
static void* allocations[8192];
static size_t allocation_count;
static int remaining = -1;
void fail_allocation_after(int count) { remaining = count; }
size_t outstanding_allocations(void) { return allocation_count; }
static int should_fail(void) {
  if (remaining < 0) return 0;
  if (remaining == 0) return 1;
  --remaining;
  return 0;
}
static void* remember(void* pointer) {
  if (pointer) {
    assert(allocation_count < 8192);
    allocations[allocation_count++] = pointer;
  }
  return pointer;
}
void* test_malloc(size_t size) {
  return should_fail() ? NULL : remember(malloc(size));
}
void* test_calloc(size_t count, size_t size) {
  return should_fail() ? NULL : remember(calloc(count, size));
}
void test_free(void* pointer) {
  if (!pointer) return;
  for (size_t i = 0; i < allocation_count; ++i) {
    if (allocations[i] == pointer) {
      allocations[i] = allocations[--allocation_count];
      free(pointer);
      return;
    }
  }
  assert(!"free of untracked allocation");
}
