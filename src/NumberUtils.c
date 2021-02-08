#include "NumberUtils.h"

#include <stdlib.h>
#include <stdbool.h>



char* itostr(int value) {
	int len = len_int(value);
	char *result = calloc(len+1, sizeof(char));
	if (value < 0) {
		value = 0 - value;
		result[0] = '-';
	}
	for (int i=len-1; i>=0; --i, value /= 10) {
		if (i == 0 && result[0] == '-')
			break;
		result[i] = (value % 10) + '0';
		if (value <= 0)
			break;
	}
	return result;
}

size_t len_int(const int value) {
	if (value < 0) {
		return len_int(0-value)+1;
	}
	size_t count = 0;
	int count_value = 1;
	while (true) {
		if (value < count_value) {
			if (count == 0)
				return 1;
			return count;
		}
		count++;
		count_value *= 10;
	}
}
