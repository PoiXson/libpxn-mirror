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
#include "StringUtils.h"

#include "MemUtils.h"
#include "NumberUtils.h"

#include <stdio.h>  // vsnprintf
#include <limits.h> // int_min
#include <ctype.h>  // tolower



bool str_empty(const char *src) {
	if (src == NULL)    return true;
	if (src[0] == '\0') return true;
	return false;
}
bool str_not_empty(const char *src) {
	if (src == NULL)    return false;
	if (src[0] == '\0') return false;
	return true;
}



size_t str_len(const char *src) {
	if (src == NULL)
		return 0;
	size_t index = 0;
	while (true) {
		if (src[index] == '\0')
			return index;
		index++;
	}
}

size_t str_l_len(const char *src, const size_t size) {
	if (src == NULL)
		return 0;
	for (size_t index=0; index<size; index++) {
		if (src[index] == '\0')
			return index;
	}
	return size;
}



// ========================================
// copy/concat strings



char *str_dup(const char *str) {
	return str_l_dup(str, str_len(str)+1);
}
char *str_l_dup(const char *str, const size_t size) {
	char *dup = calloc(size, sizeof(char));
	str_l_cpy(dup, str, size);
	return dup;
}



size_t str_l_cpy(char *dest, const char *src, const size_t size) {
	if (dest == NULL || size == 0)
		return 0;
	if (src == NULL || size == 1) {
		dest[0] = '\0';
		return 0;
	}
	size_t index = 0;
	while (true) {
		dest[index] = src[index];
		if (src[index] == '\0')
			break;
		index++;
		if (index+1 >= size)
			break;
	}
	dest[index] = '\0';
	return index;
}

size_t str_l_cat(char *dest, const char *src, const size_t size) {
	if (dest == NULL || size == 0)
		return 0;
	if (src == NULL || size == 1) {
		dest[0] = '\0';
		return 0;
	}
	size_t dest_pos = str_len(dest);
	size_t src_pos = 0;
	while (true) {
		if (src[src_pos] == '\0') break;
		dest[dest_pos] = src[src_pos];
		dest_pos++;
		src_pos++;
		if (dest_pos >= size) break;
	}
	dest[dest_pos+1] = '\0';
	return dest_pos;
}



size_t str_la_cat(char **dest, size_t *size, const char *src, const size_t src_size) {
	size_t dest_size = str_len(*dest);
	size_t new_size = dest_size + src_size + 1;
	// allocate first
	if (*size == 0) {
		*size = new_size;
		*dest = calloc(*size, sizeof(char));
		return str_l_cpy(*dest, src, *size);
	} else
	// reallocate more space
	if (new_size > *size) {
		*dest = realloc_zero(*dest, *size, new_size);
		*size = new_size;
	}
	return str_l_cat(*dest, src, *size);
}



char* snprintf_alloc(size_t *size, const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	char *result = vsnprintf_alloc(size, msg, args);
	va_end(args);
	return result;
}

char* vsnprintf_alloc(size_t *size, const char *msg, va_list args) {
	{
		va_list va_tmp;
		va_copy(va_tmp, args);
		char *tmp = calloc(1, sizeof(char));
		*size = vsnprintf(tmp, 1, msg, va_tmp);
		free(tmp);
		va_end(va_tmp);
	}
	if (*size == -1) {
		char *result = calloc(17, sizeof(char));
		str_l_cpy(result, "Invalid encoding", 16);
		return result;
	}
	{
		char *result = calloc(*size+1, sizeof(char));
		vsnprintf(result, *size+1, msg, args);
		return result;
	}
}



// ========================================
// compare strings



int str_cmp(const char *strA, const char *strB) {
	if (strA == NULL
	||  strB == NULL) {
		if (strA != NULL) return 0 - (strA[0]);
		if (strB != NULL) return strB[0];
		return 0;
	}
	size_t sizeA = str_len(strA);
	size_t sizeB = str_len(strB);
	return str_l_cmp(strA, strB, MAX(sizeA, sizeB));
}

int str_l_cmp(const char *strA, const char *strB, const size_t size) {
	if (size == -1) return INT_MIN;
	if (size ==  0) return 0;
	if (strA == NULL
	||  strB == NULL) {
		if (strA != NULL) return 0 - (strA[0]);
		if (strB != NULL) return strB[0];
		return 0;
	}
	int cmp;
	for (size_t index=0; index<size; index++) {
		if (*strA == '\0'
		||  *strB == '\0') {
			if (*strA == '\0'
			&&  *strB == '\0')
				return 0;
			if (*strA == '\0')
				return (int)(*strB);
			return 0 - (int)(*strA);
		}
		cmp = tolower((int)(*strB)) - tolower((int)(*strA));
		if (cmp != 0)
			return cmp;
		strA++;
		strB++;
		
	}
	return 0;
}



bool str_ends_with(const char *src, const char match) {
	if (src   == NULL) return false;
	if (match == '\0') return true;
	size_t len = str_len(src);
	return (src[len-2] == match);
}



// ========================================
// find in string



size_t chr_pos(const char *haystack, const char needle) {
	return chr_s_pos(haystack, needle, 0);
}

size_t chr_s_pos(const char *haystack, const char needle, const size_t start) {
	size_t len = str_len(haystack);
	if (start > len)
		return -1;
	size_t i = start;
	while (i < len) {
		if (haystack[i] == '\0')
			return (needle == '\0') ? i : -1;
		if (haystack[i] == needle)
			return i;
		i++;
	}
	return (needle == '\0') ? len : -1;
}



size_t chr_r_pos(const char *haystack, const char needle) {
	return chr_rs_pos(haystack, needle, 0);
}

size_t chr_rs_pos(const char *haystack, const char needle, const size_t start) {
	size_t len = str_len(haystack);
	if (start > len)
		return -1;
	size_t i = start;
	size_t last = -1;
	while (i < len) {
		if (haystack[i] == '\0')
			break;
		if (haystack[i] == needle)
			last = i;
		i++;
	}
	return last;
}



// ========================================
// trim



char *str_trim(char *src) {
	if (src == NULL) return NULL;
	str_trim_right(src);
	return str_trim_left(src);
}

char *str_trim_left(char *src) {
	if (src == NULL) return NULL;
	while (*src != '\0') {
		if (*src != ' '
		&&  *src != '\t'
		&&  *src != '\n'
		&&  *src != '\r')
			break;
		*src++;
	}
	return src;
}

void str_trim_right(char *src) {
	if (src == NULL) return;
	size_t len = str_len(src);
	if (len == 0)
		return;
	len--;
	while (src[len] != '\0') {
		if (src[len] != ' '
		&&  src[len] != '\t'
		&&  src[len] != '\n'
		&&  src[len] != '\r')
			break;
		len--;
	}
	src[len+1] = '\0';
}



// ========================================
// to upper/lower



void str_to_upper(char *src) {
	if (src == NULL) return;
	for (char *c=src; (*c=toupper(*c)); c++);
}
void str_to_lower(char *src) {
	if (src == NULL) return;
	for (char *c=src; (*c=tolower(*c)); c++);
}

void str_l_to_upper(char *src, const size_t size) {
	if (src == NULL) return;
	char *c = src;
	for (size_t pos=0; pos<size; pos++) {
		*c=toupper(*c);
		c++;
	}
}
void str_l_to_lower(char *src, const size_t size) {
	if (src == NULL) return;
	char *c = src;
	for (size_t pos=0; pos<size; pos++) {
		*c=tolower(*c);
		c++;
	}
}



// ========================================
// pad string



void str_pad_front(char *src, const size_t size) {
	if (size < 1) return;
	size_t len = str_len(src);
	if (len > size) return;
	size_t siz = size - 1;
	size_t count = siz - len;
	// shift to end
	for (size_t pos=1; pos<len+1; pos++)
		src[siz-pos] = src[len-pos];
	// pad front
	for (size_t pos=0; pos<count; pos++)
		src[pos] = ' ';
	src[siz] = '\0';
}

void str_pad_end(char *src, const size_t size) {
	if (size < 1) return;
	size_t len = str_len(src);
	if (len > size) return;
	size_t siz = size - 1;
	size_t count = siz - len;
	char pad[count+1];
	for (size_t index=0; index<count; index++) {
		pad[index] = ' ';
	}
	pad[count] = '\0';
	str_l_cat(src, pad, size);
}

void str_pad_center(char *src, const size_t size) {
	if (size < 1) return;
	size_t len = str_len(src);
	if (len > size) return;
	char result[size];
	mem_set(result, ' ', size);
	result[size] = '\0';
	size_t offset = ((size - len) / 2);
	for (size_t index=0; index<len; index++) {
		if (src[index] == '\0')
			break;
		result[index+offset] = src[index];
	}
	str_l_cpy(src, result, size);
}



// ========================================
// modify string



void str_l_make_safe(char *src, const size_t size) {
	char c;
	for (size_t i=0; i<size; i++) {
		c = src[i];
		if (c == '\0') {
			for (;i<size; i++)
				src[i] = '\0';
			return;
		}
		if (c == '_' || c == '-') continue;
		if (c >= 'a' && c <= 'z') continue;
		if (c >= 'A' && c <= 'Z') continue;
		if (c >= '0' && c <= '9') continue;
		src[i] = '_';
	}
	src[size-1] = '\0';
}



char* str_la_unescape(const char *src, const size_t size) {
	// count escapes
	size_t count = 0;
	for (size_t index=0; index<size; index++) {
		if (src[index] == '\\'
		||  src[index] == '\n'
		||  src[index] == '\r'
		||  src[index] == '\t'
		||  src[index] == '\0')
			count++;
	}
	size_t result_size = size + count;
	char *result = calloc(result_size+1, sizeof(char));
	size_t pos = 0;
	for (size_t index=0; index<size; index++) {
		switch (src[index]) {
			case '\\':
				result[pos] = '\\'; pos++;
				result[pos] = '\\'; pos++;
				break;
			case '\n':
				result[pos] = '\\'; pos++;
				result[pos] = 'n';  pos++;
				break;
			case '\r':
				result[pos] = '\\'; pos++;
				result[pos] = 'r';  pos++;
				break;
			case '\t':
				result[pos] = '\\'; pos++;
				result[pos] = 't';  pos++;
				break;
			case '\0':
				result[pos] = '\\'; pos++;
				result[pos] = '0';  pos++;
				break;
			default:
				result[pos] = src[index]; pos++;
				break;
		}
	}
	result[result_size] = '\0';
	return result;
}



char* str_a_sum(const char *src) {
	size_t len = str_len(src);
	bool started = false;
	size_t result_start = 0;
	size_t pos = 0;
	for (; pos<len; pos++) {
		if (src[pos] == '\0') break;
		// find start
		if (!started) {
			if (src[pos] == ' ' ) continue;
			if (src[pos] == '\t') continue;
			if (src[pos] == '\n') continue;
			if (src[pos] == '\r') continue;
			started = true;
			result_start = pos;
		}
		if (started) {
			if (src[pos] == ' ' ) break;
			if (src[pos] == '\t') break;
			if (src[pos] == '\n') break;
			if (src[pos] == '\r') break;
		}
	}
	size_t result_len = (started ? pos - result_start : 0);
	if (result_len > 20)
		result_len = 20;
	char *result = calloc(result_len+1, sizeof(char));
	str_l_cpy(result, src+result_start, result_len+1);
	return result;
}
