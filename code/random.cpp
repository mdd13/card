#pragma once

#include <time.h>
#include "common.h"

// TODO: Update random function
int RandomInt(int min_num, int max_num) {
	int result = 0, low_num = 0, hi_num = 0;

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

void ShuffleIntArray(int *a, size_t len) {
	if (len <= 1) {
		return;
	}

	for (int i = 0; i < len - 1; i++) {
		int j = i + RandomInt(0, (len - (i + 1)));
		Swap(a[i], a[j]);
	}
}
