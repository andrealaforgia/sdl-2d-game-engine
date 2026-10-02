#ifndef TESTS_TEST_ALLOCATOR_H_
#define TESTS_TEST_ALLOCATOR_H_
#include <stddef.h>
void* test_malloc(size_t size);
void* test_calloc(size_t count, size_t size);
void test_free(void* pointer);
void fail_allocation_after(int count);
size_t outstanding_allocations(void);
#endif  // TESTS_TEST_ALLOCATOR_H_
