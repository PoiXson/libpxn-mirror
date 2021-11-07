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
#include "MemUtils.h"



void mem_set(void *dest, const int val, const size_t len) {
	size_t pos = len;
	char  *ptr = dest;
	while (pos-- != 0)
		*ptr++ = val;
}



void* realloc_zero(void *buf, const size_t old_size, const size_t new_size) {
	void *ptr = realloc(buf, new_size);
	if (new_size > old_size) {
		if (ptr != NULL) {
			char *start = ((char*)ptr) + old_size;
			size_t len = new_size - old_size;
			mem_set(start, 0, len);
		}
	}
	return ptr;
}
