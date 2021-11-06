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
#include <stdarg.h> // va_list



bool str_empty(const char *src);
bool str_not_empty(const char *src);

char *str_dup(const char *str);
char *str_l_dup(const char *str, const size_t size);

size_t str_len  (const char *src);
size_t str_l_len(const char *src, const size_t size);

size_t str_l_cpy(char *dest, const char *src, const size_t size);
size_t str_l_cat(char *dest, const char *src, const size_t size);
size_t str_la_cat(char **dest, size_t *size, const char *src, const size_t src_size);

char* snprintf_alloc(size_t *size, const char *msg, ...);
char* vsnprintf_alloc(size_t *size, const char *msg, va_list args);

int str_cmp  (const char *strA, const char *strB);
int str_l_cmp(const char *strA, const char *strB, const size_t size);

bool str_ends_with(const char *src, const char match);

size_t chr_pos(const char *haystack, const char needle);
size_t chr_s_pos(const char *haystack, const char needle, const size_t start);
size_t chr_r_pos(const char *haystack, const char needle);
size_t chr_rs_pos(const char *haystack, const char needle, const size_t start);

char *str_trim      (char *src);
char *str_trim_left (char *src);
void  str_trim_right(char *src);

void str_to_upper(char *src);
void str_to_lower(char *src);
void str_l_to_upper(char *src, const size_t size);
void str_l_to_lower(char *src, const size_t size);

void str_pad_front (char *src, const size_t size);
void str_pad_end   (char *src, const size_t size);
void str_pad_center(char *src, const size_t size);

void  str_l_make_safe(char *src, const size_t size);
char* str_la_unescape(const char *src, const size_t size);
char* str_a_sum(const char *src);
