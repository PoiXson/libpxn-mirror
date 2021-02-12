#include "StringUtils.h"

#include <stdbool.h>
#include <string.h> // strlen
#include <limits.h> // int_min
#include <ctype.h>  // tolower



size_t strlcpy(char *dest, char *src, const size_t size) {
	size_t len = strlen(dest);
	for (size_t i=0; i<size; i++) {
		dest[i] = src[i];
		if (src[i] == '\0')
			break;
		len++;
	}
	dest[size-1] = '\0';
	return len;
}

size_t strlcat(char *dest, char *src, const size_t size) {
	size_t len = strlen(dest);
	if (len >= size) {
		dest[size-1] = '\0';
		return len;
	}
	dest = dest + len;
	while (*src != '\0' && len < size-1) {
		*dest++ = *src++;
		len++;
	}
	*dest = '\0';
	return len;
}



size_t strlrcpy(char **dest, char *src, size_t *size) {
	size_t len_dest = strlen(*dest);
	size_t len_src  = strlen(src);
	if (len_dest + len_src + 1 > *size) {
		*size = len_dest + len_src + 1;
		*dest = realloc(*dest, (*size) * sizeof(char));
		memset((*dest)+len_dest, '\0', len_src + 1);
	}
	strlcpy(*dest, src, *size);
}

size_t strlrcat(char **dest, char *src, size_t *size) {
	size_t len_dest = strlen(*dest);
	size_t len_src  = strlen(src);
	// reallocate more space
	if (len_dest + len_src + 1 > *size) {
		if (len_dest > 256) {
			while (len_dest + len_src + 1 > *size) {
				(*size) *= 2;
			}
		} else {
			*size = len_dest + len_src + 1;
		}
		*dest = realloc(*dest, (*size) * sizeof(char));
		memset((*dest)+len_dest, '\0', len_src + 1);
	}
	strlcat(*dest, src, *size);
}



int strlcmp(const char *strA, const char *strB, const size_t size) {
	if (size == -1 || size == 0)
		return INT_MIN;
	if (strA == NULL) {
		if (strB == NULL)
			return 0;
		return INT_MIN;
	}
	if (strB == NULL)
		return INT_MAX;
	size_t index = 0;
	int cmp;
	while (true) {
		if (++index > size)
			return 0;
		if (*strA == '\0') {
			if (*strB == '\0')
				return 0;
			return (int)(*strB);
		}
		if (*strB == '\0')
			return 0 - (int)(*strA);
		cmp = tolower( (int)(*strB) ) - tolower( (int)(*strA) );
		if (cmp != 0)
			return cmp;
		strA++;
		strB++;
	}
}



char* str_trim(char *str) {
	if (str == NULL)
		return NULL;
	// trim front
	while (true) {
		if (*str == ' ' ) { *str++; continue; }
		if (*str == '\t') { *str++; continue; }
		if (*str == '\0') { *str++; continue; }
		break;
	}
	size_t len = strlen(str) - 1;
	// trim end
	while (true) {
		if (str[len] == ' ' ) { len--; continue; }
		if (str[len] == '\t') { len--; continue; }
		if (str[len] == '\0') { len--; continue; }
		break;
	}
	str[len+1] = '\0';
	return str;
}



size_t chrpos(const char *haystack, const char needle) {
	return chrposs(haystack, needle, 0);
}

size_t chrposs(const char *haystack, const char needle, const size_t start) {
	size_t len = strlen(haystack);
	if (len < start)
		return -1;
	size_t i = start;
	while (i < len) {
		if (haystack[i] == needle)
			return i;
		if (haystack[i] == '\0')
			return (needle == '\0') ? i : -1;
		i++;
	}
	return (needle == '\0') ? len : -1;
}



void str_pad_front(char *line, const size_t size) {
	if (size < 1) return;
	size_t len = strlen(line);
	if (len >= size) return;
	size_t count = size - len;
	char pad[count+1];
	for (size_t index=0; index<count; index++) {
		pad[index] = ' ';
	}
	pad[count] = '\0';
	char result[size+1];
	strcpy(result, pad);
	strcat(result, line);
	strcpy(line, result);
}

void str_pad_end(char *line, const size_t size) {
	if (size < 1) return;
	size_t len = strlen(line);
	if (len >= size) return;
	size_t count = size - len;
	char pad[count+1];
	for (size_t index=0; index<count; index++) {
		pad[index] = ' ';
	}
	pad[count] = '\0';
	strcat(line, pad);
}

void str_pad_center(char *line, const size_t size) {
	if (size < 1) return;
	size_t len = strlen(line);
	if (len >= size) return;
	size_t count = ((size - len) / 2);
	char pad[count+2];
	for (size_t index=0; index<count; index++) {
		pad[index] = ' ';
	}
	pad[count] = '\0';
	char result[size+1];
	strcpy(result, pad);
	strcat(result, line);
	count = 1 + (((size - len) - 1) / 2);
	for (size_t index=0; index<count; index++) {
		pad[index] = ' ';
	}
	pad[count] = '\0';
	strcat(result, pad);
	strcpy(line, result);
}



bool str_starts_with(const char *haystack, const char *needle) {
	for (size_t i=0; needle[i] != '\0'; i++) {
		if (haystack[i] != needle[i])
			return false;
	}
	return true;
}

bool str_ends_with(const char *haystack, const char *needle) {
	if (strlen(haystack) < strlen(needle))
		return false;
	size_t len = strlen(haystack) - strlen(needle);
	return (strcmp(&haystack[len], needle) == 0);
}
