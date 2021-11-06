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
#pragma once

#include <stdbool.h>
#include <stdlib.h> // size_t



size_t strlcpy(     char *dest, char *src, const size_t size);
size_t strlcat(     char *dest, char *src, const size_t size);
size_t strlcatfront(char *dest, char *src, const size_t size);

size_t strrcpy(char **dest, size_t *size, char *src);
size_t strrcat(char **dest, size_t *size, char *src);

size_t strlrcpy(char **dest, size_t *size, char *src, size_t len_src);
size_t strlrcat(char **dest, size_t *size, char *src, size_t len_src);

int strlcmp(const char *strA, const char *strB, const size_t size);

char* str_trim(char *str);

char* str_sum(char *str);

char* str_unescape( char *str);
char* strl_unescape(char *str, size_t size);

size_t chrpos( const char *haystack, const char needle);
size_t chrposs(const char *haystack, const char needle, const size_t start);

size_t chrrpos( const char *haystack, const char needle);
size_t chrrposs(const char *haystack, const char needle, const size_t start);

void str_pad_front( char *line, const size_t size);
void str_pad_end(   char *line, const size_t size);
void str_pad_center(char *line, const size_t size);

bool str_starts_with(const char *haystack, const char *needle);
bool str_ends_with(  const char *haystack, const char *needle);

void str_make_safe(char *str, const size_t size);
