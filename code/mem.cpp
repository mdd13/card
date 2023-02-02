#pragma once

#include "common.h"

// NOTE: Not implemented yet! So use normal malloc() and free()


void *Alloc(size_t size) {
	void *result = malloc(size);

	Assert(result);

	// NOTE(): May be need to be zero buffer.
	uint8_t *tmp_buf = (uint8_t *)result;
	for (int i = 0; i < size; ++i) {
		tmp_buf[i] = 0;
	}

	return result;
}

void Free(void *ptr) {
	free(ptr);
}
