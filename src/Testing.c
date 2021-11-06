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
#include "Testing.h"

#include "MemUtils.h"
#include "StringUtils.h"
#include "Logs.h"
#include "Params.h"

#include <time.h>



TestingState *testing_state = NULL;



void testing_init(int argc, char *argv[]) {
	#ifdef DEBUG
		log_line(" <Built with Debug Flags> ");
	#else
		log_line(" <Production Build> ");
	#endif
	testing_state = calloc(1, sizeof(TestingState));
	size_t flag_abort   = param_add('a', 1, "abort",   "Abort on failed asserts [default]");
	param_add_spacer();
	size_t flag_color   = param_add('C', 1, "color",   "Enable console colors");
	size_t flag_ncolor  = param_add('\0',1, "no-color","Disable console colors");
	size_t flag_verbose = param_add('v', 1, "verbose", "Detailed logging");
	size_t flag_help    = param_add('h', 1, "help",    "Display this help message and exit");
	params_process(argc, argv, argv[0]);
	testing_state->abort_on_fail  = params_get_bool(flag_abort);
	testing_state->display_detail = params_get_bool(flag_verbose);
	if (params_get_bool(flag_ncolor)) {
		set_log_color_enabled(false);
	} else
	if (params_get_bool(flag_color)) {
		set_log_color_enabled(true);
	}
	if (params_get_bool(flag_verbose)) {
		log_level_set(LVL_ALL);
	}
	// --help
	if (params_get_bool(flag_help)) {
		display_help();
		exit(1);
	}
}

void testing_start() {
	if (testing_state == NULL) {
		log_fatal("Testing framework not initialized");
		exit(1);
	}
	bool color = has_log_color_enabled();
	testing_state->time_start = clock();
	log_nl();
	// GROUPS_LOOP:
	for (size_t index=0; index<testing_state->groups_size; index++) {
		TestingGroup *group = &(testing_state->groups[index]);
		if (group->used != true) continue;
		testing_state->current_group = index;
		printf(
			"%s%20s%s %s[%s",
			(color ? COLOR_CYAN  : ""),
			group->group_name,
			(color ? COLOR_RESET : ""),
			(color ? COLOR_BLUE  : ""),
			(color ? COLOR_RESET : "")
		);
		group->time_start = clock();
		group->func();
		group->time_end = clock();
		printf(
			"%s]%s\n",
			(color ? COLOR_BLUE  : ""),
			(color ? COLOR_RESET : "")
		);
		testing_state->count_groups++;
	} // end GROUPS_LOOP
	log_nl();
	testing_state->time_end = clock();
}

int testing_done() {
	if (testing_state == NULL) {
		log_fatal("Testing framework not initialized");
		exit(1);
	}
	bool color = has_log_color_enabled();
	testing_state->time_end = clock();
	// display details
	if (testing_state->display_detail
	||  testing_state->count_failed > 0) {
		// GROUPS_LOOP:
		for (size_t group_index=0; group_index<testing_state->groups_size; group_index++) {
			TestingGroup *group = &(testing_state->groups[group_index]);
			if (group->used != true)
				continue;
			double last = group->time_start;
			// POINTS_LOOP:
			for (size_t point_index=0; point_index<group->points_size; point_index++) {
				TestingPoint *point = &(group->points[point_index]);
				if (point->used != true)
					continue;
				if (testing_state->display_detail
				||  point->success != true) {
					double duration = (point->timestamp - last) / CLOCKS_PER_SEC;
					log_line(
						" %s[%s]%s %5.3f %s %s:%i",
						(color ? (point->success ? COLOR_GREEN : COLOR_RED) : ""),
						(point->success ? (color ? "\u2713" : ".") : "X"),
						(color ? COLOR_RESET : ""),
						(duration > 0.01 ? duration : duration * 1000.0),
						(duration > 0.01 ? "s "     : "ms"             ),
						(str_empty(group->func_name) ? "" : group->func_name),
						point->line
					);
					if (str_not_empty(point->msg)) {
						log_line(point->msg);
					}
				}
				last = point->timestamp;
			} // end POINTS_LOOP
		} // end GROUPS_LOOP
	}
	log_nl();
	double elapsed = (testing_state->time_end - testing_state->time_start) / CLOCKS_PER_SEC;
	printf(
		"Ran %s%lu%s test group%s in %s%.3f%s %s\n",
		(color ? COLOR_CYAN : ""),
		testing_state->count_groups,
		(color ? COLOR_RESET : ""),
		(testing_state->count_groups == 1 ? "" : "s"),
		(color ? COLOR_CYAN : ""),
		(elapsed > 0.01 ? elapsed : elapsed * 1000.0),
		(color ? COLOR_RESET : ""),
		(elapsed > 0.01 ? "seconds" : "milliseconds")
	);
	// no asserts
	if (testing_state->count_success == 0
	&&  testing_state->count_failed  == 0) {
		printf(" %sNo test asserts ran!%s\n",
			(color ? COLOR_RED   : ""),
			(color ? COLOR_RESET : "")
		);
	} else
	// all passing
	if (testing_state->count_failed == 0) {
		printf(
			" %s<PASS> %lu passing asserts%s\n",
			(color ? COLOR_GREEN : ""),
			testing_state->count_success,
			(color ? COLOR_RESET : "")
		);
	// failed asserts
	} else {
		printf(" %lu passing asserts\n", testing_state->count_success);
		printf(" %s<FAIL> %lu failed asserts!%s\n",
			(color ? COLOR_RED : ""),
			testing_state->count_failed,
			(color ? COLOR_RESET : "")
		);
	}
	log_nl();
	// finished/exit
	if (testing_state->count_success == 0
	&&  testing_state->count_failed  == 0)
		return 1;
	if (testing_state->count_failed > 0)
		return 1;
	return 0;
}



void testing_free() {
	if (testing_state != NULL) {
		if (testing_state->groups != NULL) {
			// GROUPS_LOOP:
			for (size_t group_index=0; group_index<testing_state->groups_size; group_index++) {
				TestingGroup *group = &(testing_state->groups[group_index]);
				if (group->func_name  != NULL) free(group->func_name);
				if (group->group_name != NULL) free(group->group_name);
				if (group->points     != NULL) free(group->points);
			} // end GROUPS_LOOP:
			free(testing_state->groups);
		}
		free(testing_state);
		testing_state = NULL;
	}
}



// ========================================
// allocations



size_t testing_alloc_group() {
	if (testing_state->groups_size == 0) {
		testing_state->groups_size = 1;
		testing_state->groups = calloc(testing_state->groups_size, sizeof(TestingGroup));
		if (testing_state->groups == NULL) {
			log_fatal("Failed to allocate testing group array");
			exit(1);
		}
		return 0;
	}
	for (size_t index=0; index<testing_state->groups_size; index++) {
		if (testing_state->groups[index].used == false)
			return index;
	}
	size_t old_size = testing_state->groups_size;
	size_t new_size = testing_state->groups_size * 2;
	testing_state->groups = realloc_zero(
		testing_state->groups,
		sizeof(TestingGroup) * old_size,
		sizeof(TestingGroup) * new_size
	);
	if (testing_state->groups == NULL) {
		log_fatal("Failed to reallocate testing group array");
		exit(1);
	}
	testing_state->groups_size = new_size;
	return old_size;
}

size_t testing_alloc_point(TestingGroup *group) {
	if (testing_state == NULL) {
		log_fatal("Testing framework not initialized");
		exit(1);
	}
	if (group->points_size == 0) {
		group->points_size = 1;
		group->points = calloc(group->points_size, sizeof(TestingPoint));
		if (group->points == NULL) {
			log_fatal("Failed to allocate testing points array");
			exit(1);
		}
		return 0;
	}
	for (size_t index=0; index<group->points_size; index++) {
		if (group->points[index].used == false)
			return index;
	}
	size_t old_size = group->points_size;
	size_t new_size = group->points_size * 2;
	group->points = realloc_zero(
		group->points,
		sizeof(TestingPoint) * old_size,
		sizeof(TestingPoint) * new_size
	);
	if (group->points == NULL) {
		log_fatal("Failed to reallocate testing points array");
		exit(1);
	}
	group->points_size = new_size;
	return old_size;
}



void testing_add(const void *func, const char *group_name, const char *func_name) {
	if (testing_state == NULL) {
		log_fatal("Testing framework not initialized");
		exit(1);
	}
	size_t index = testing_alloc_group();
	TestingGroup *group = &(testing_state->groups[index]);
	group->used = true;
	group->func = func;
	size_t group_name_size = str_len(group_name) + 1;
	size_t func_name_size  = str_len(func_name ) + 1;
	group->group_name = calloc(group_name_size, sizeof(char));
	group->func_name  = calloc(func_name_size,  sizeof(char));
	str_l_cpy(group->group_name, group_name, group_name_size);
	str_l_cpy(group->func_name,  func_name,  func_name_size );
}



// ========================================
// asserts



TestingPoint* do_assert(const int line, const bool test) {
	if (testing_state == NULL) {
		log_fatal("Testing framework not initialized");
		exit(1);
	}
	TestingGroup *group = &(testing_state->groups[testing_state->current_group]);
	size_t index = testing_alloc_point(group);
	TestingPoint *point = &(group->points[index]);
	point->used      = true;
	point->timestamp = clock();
	point->line      = line;
	point->success   = test;
	if (test) {
		testing_state->count_success++;
		if (has_log_color_enabled())
			printf(COLOR_GREEN"."COLOR_RESET);
		else printf(".");
	} else {
		testing_state->count_failed++;
		if (has_log_color_enabled())
			printf(COLOR_RED"x"COLOR_RESET);
		else printf("x");
	}
	return point;
}

void testing_abort_on_fail() {
	if (testing_state->abort_on_fail) {
		testing_done();
		exit(1);
	}
}



void assert(const int line, const bool test) {
	TestingPoint *point = do_assert(line, test);
	if (!point->success) {
		testing_abort_on_fail();
	}
}
void assert_not(const int line, const bool test) {
	TestingPoint *point = do_assert(line, !test);
	if (!point->success) {
		testing_abort_on_fail();
	}
}

void assert_null(const int line, const void *value) {
	bool test = (value == NULL);
	TestingPoint *point = do_assert(line, test);
	if (!point->success) {
		str_l_cpy(point->msg, "   expected: NULL\n   actual:   <other>", TEST_MSG_SIZE);
		testing_abort_on_fail();
	}
}
void assert_not_null(const int line, const void *value) {
	bool test = (value != NULL);
	TestingPoint *point = do_assert(line, test);
	if (!point->success) {
		str_l_cpy(point->msg, "   expected: <other>\n   actual:   NULL", TEST_MSG_SIZE);
		testing_abort_on_fail();
	}
}

void assert_str_cmp(const int line, const char *expected, const char *actual) {
	int result = str_cmp(expected, actual);
	bool test = (result == 0);
	TestingPoint *point = do_assert(line, test);
	if (!point->success) {
		char *exp = str_la_unescape(expected, str_len(expected));
		char *act = str_la_unescape(actual,   str_len(actual)  );
		snprintf(
			point->msg,
			TEST_MSG_SIZE,
			"   cmp result: %i\n   expected: '%s'\n   actual:   '%s'",
		   result,
			exp,
			act
		);
		free(exp);
		free(act);
		testing_abort_on_fail();
	}
}

void assert_int_cmp(const int line, const int expected, const int actual) {
	bool test = (expected == actual);
	TestingPoint *point = do_assert(line, test);
	if (!point->success) {
		snprintf(
			point->msg,
			TEST_MSG_SIZE,
			"   expected: %i\n   actual:   %i",
			expected,
			actual
		);
		testing_abort_on_fail();
	}
}

void assert_sizet_cmp(const int line, const size_t expected, const size_t actual) {
	bool test = (expected == actual);
	TestingPoint *point = do_assert(line, test);
	if (!point->success) {
		snprintf(
			point->msg,
			TEST_MSG_SIZE,
			"   expected: %lu\n   actual:   %lu",
			expected,
			actual
		);
		testing_abort_on_fail();
	}
}
