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

#include "StringUtils.h"

#include <stdbool.h>
#include <stdlib.h>



#define TEST_MSG_SIZE 100



typedef struct TestingPoint {
	bool used;
	int line;
	bool success;
	double timestamp;
	char msg[TEST_MSG_SIZE];
} TestingPoint;

typedef struct TestingGroup {
	bool used;
	void (*func)();
	char *func_name;
	char *group_name;
	double time_start;
	double time_end;
	// asserts
	TestingPoint *points;
	size_t points_size;
} TestingGroup;

typedef struct TestingState {
	bool abort_on_fail;
	bool display_detail;
	double time_start;
	double time_end;
	size_t count_groups;
	size_t count_success;
	size_t count_failed;
	// test groups
	TestingGroup *groups;
	size_t groups_size;
	size_t current_group;
} TestingState;

extern TestingState *testing_state;



void testing_init(int argc, char *argv[]);
void testing_start();
int  testing_done();
void testing_free();

size_t testing_alloc_group();
size_t testing_alloc_point(TestingGroup *group);

void testing_add(const void *func, const char *group_name, const char *func_name);

TestingPoint* do_assert(const int line, const bool test);
void testing_abort_on_fail();

void assert          (const int line, const bool test);
void assert_not      (const int line, const bool test);
void assert_null     (const int line, const void *value);
void assert_not_null (const int line, const void *value);
void assert_str_cmp  (const int line, const char  *expected, const char  *actual);
void assert_int_cmp  (const int line, const int    expected, const int    actual);
void assert_sizet_cmp(const int line, const size_t expected, const size_t actual);
