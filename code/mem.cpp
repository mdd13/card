#pragma once

#include "common.cpp"

// NOTE: Not implemented yet! So use normal malloc() and free()

void *Alloc(usize size) {
	void *result = malloc(size);

	Assert(result);
	memset(result, 0, size);

	return result;
}

void Free(void *ptr) {
	free(ptr);
}

#define RemoveIndex(ptr, len, idx) if ((len) > 0 && (idx) < (len) && (idx) >= 0) { \
		memmove((ptr) + (idx), (ptr) + (idx) + 1, ((len) - (idx) - 1) * sizeof((ptr)[0])); \
	}
