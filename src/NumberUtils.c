//===============================================================================
// Copyright (c) 2020-2021 PoiXson, Mattsoft
// <https://poixson.com> <https://mattsoft.net>
// Released under the GPL 3.0
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//===============================================================================
#include "NumberUtils.h"

#include <stdbool.h>



int pow_int(const int base, const int power) {
	if (power == 0) return 0;
	int result = base;
	for (int i=1; i<power; i++) {
		result *= base;
	}
	return result;
}

size_t pow_sizet(const size_t base, const int power) {
	if (power == 0) return 0;
	size_t result = base;
	for (int i=1; i<power; i++) {
		result *= base;
	}
	return result;
}



size_t len_int(const int value) {
	if (value < 0)
		return len_int(0-value)+1;
	size_t count = 0;
	int count_value = 1;
	while (true) {
		if (value < count_value)
			return (count == 0 ? 1 : count);
		count++;
		count_value *= 10;
	}
}

size_t len_sizet(const size_t value) {
	size_t count = 0;
	size_t count_value = 1;
	while (true) {
		if (value < count_value)
			return (count == 0 ? 1 : count);
		count++;
		// max length of size_t
		if (count >= 19) return 19;
		count_value *= 10;
	}
}



char* i_to_str(const int value) {
	int len = len_int(value);
	char *result = calloc(len+1, sizeof(char));
	int val = value;
	if (val < 0) {
		val = 0 - val;
		result[0] = '-';
	}
	for (int i=len-1; i>=0; --i, val /= 10) {
		if (i == 0 && result[0] == '-')
			break;
		result[i] = (val % 10) + '0';
		if (val <= 0)
			break;
	}
	return result;
}
