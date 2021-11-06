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

#include <stdlib.h>
#include <stdbool.h>



#define PARAMS_LONG_MAX     3
#define PARAMS_LONG_SIZE   10
#define PARAMS_DESC_SIZE  200
#define PARAMS_ACTION_SIZE 20
#define PARAMS_FLAGS_SPACE 25



extern char *params_exec_name;



typedef enum Param_Flag_Type {
	FLAGTYPE_BOOL = 0x0,
	FLAGTYPE_INT  = 0x1,
	FLAGTYPE_TEXT = 0x2
} Param_Flag_Type;

typedef struct Param {
	bool used;
	size_t index;
	char flag_short;
	char flags_long[PARAMS_LONG_MAX][PARAMS_LONG_SIZE];
	char desc[PARAMS_DESC_SIZE];
	Param_Flag_Type type;
	union {
		bool value_bool;
		int  value_int;
		char *value_text;
	};
} Param;

extern Param *params;
extern size_t params_size;



extern char*  param_actions[PARAMS_ACTION_SIZE];
extern size_t param_actions_size;



size_t param_allocate();
void   param_init(const size_t index);

size_t param_add(const char flag_short, const size_t flags_long_count, ...);
size_t param_add_spacer();

void   params_process(const int argc, char *argv[], char *exec_name);
void   params_process_value(const size_t flag_index, char *str);
size_t params_find_flag(const char *flag);

bool  params_get_bool(size_t index);
char* params_get_text(size_t index);

size_t param_flags_count();
size_t param_actions_count();

void display_help();
