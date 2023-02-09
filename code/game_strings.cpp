#pragma once

#include "common.cpp"


i32 StringLen(const char *s) {
	const char *p = s;
	while(p != 0 && *p != '\0') {
		p++;
	}
	return p - s;
}

b32 StringEqual(const char *a, const char *b, i32 len) {
	for (int i = 0; i < len; ++i) {
		if (a[i] != b[i]) {
			return false;
		}
	}
	return true;
}

i32 StringIndexByte(const char *s, char b) {
	const char *p = s;
	while(p != 0 && *p != '\0') {
		if (b == *p) {
			return p - s;
		}
		p++;
	}
	return -1;
}

i32 StringIndex(const char *s, const char *substr) {
	i32 n = StringLen(s);
	i32 m = StringLen(substr);

	if (m == 0) {
		return 0;
	}
	if (m == 1) {
		return StringIndexByte(s, substr[0]);
	}
	if (m == n) {
		if (strcmp(s, substr) == 0) {
			return 0;
		}
		return -1;
	}
	if (n < m) {
		return -1;
	}

	for (int i = 0; i < n-m; i++) {
		if (StringEqual(s+i, substr, m)) {
			return i;
		}
	}
	return -1;
}

char *StringCopyPart(const char *src, i32 from, i32 to) {
	i32 n = StringLen(src);
	Assert((from >= 0) && (to <= n));

	i32 m = to - from + 1;

	char *result = (char *)GameMemAlloc(m + 1);
	for (int i = 0; i < m; i++) {
		result[i] = src[i+from];
	}
	result[m] = '\0';

	return result;
}
