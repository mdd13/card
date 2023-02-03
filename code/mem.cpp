#pragma once

#include "common.cpp"

// NOTE: Not implemented yet! So use normal malloc() and free()


void *Alloc(size_t size) {
	void *result = malloc(size);

	Assert(result);
	memset(result, 0, size);

	return result;
}

void Free(void *ptr) {
	free(ptr);
}
