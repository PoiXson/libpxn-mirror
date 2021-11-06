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
