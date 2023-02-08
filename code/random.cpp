#pragma once

#include <time.h>
#include "common.cpp"

// TODO: Update random function
i32 RandomI32(i32 min_num, i32 max_num) {
	i32 result = 0, low_num = 0, hi_num = 0;

	if (min_num < max_num) {
		low_num = min_num;
		hi_num = max_num + 1;
	} else {
		low_num = max_num + 1;
		hi_num = min_num;
	}

	srand(time(0));
	result = (rand() % (hi_num - low_num)) + low_num;
	return result;
}

void ShuffleI32Array(i32 *a, usize len) {
	if (len <= 1) {
		return;
	}

	for (i32 i = 0; i < len - 1; i++) {
		i32 j = i + RandomI32(0, (len - (i + 1)));
		Swap(a[i], a[j]);
	}
}
