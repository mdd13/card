#pragma once

#include "common.cpp"

// NOTE: Not implemented yet! So just use malloc() and free()

void *GameMemAlloc(usize size) {
	void *result = malloc(size);

	Assert(result);

	memset(result, 0, size);

	return result;
}

void GameMemFree(void *ptr) {
	if (ptr != 0) {
		free(ptr);
	}
}

void GameMemCopy(void *dst, void *src, usize size) {
	memcpy(dst, src, size);	
}

void *GameMemRealloc(void *ptr, usize oldsize, usize newsize) {
	void *result = GameMemAlloc(newsize);
	GameMemCopy(result, ptr, oldsize);

	GameMemFree(ptr);
	return result;
}

void GameMemMove(void *dst, void *src, usize len) {
	for (i32 i = 0; i < len; ++i) {
		((char *)dst)[i] = ((char *)src)[i];
	}
}

#define GameMemRemoveIndex(ptr, len, idx) if ((len) > 0 && (idx) < (len) && (idx) >= 0) { \
		GameMemMove((ptr) + (idx), (ptr) + (idx) + 1, ((len) - (idx) - 1) * sizeof((ptr)[0])); \
	}
