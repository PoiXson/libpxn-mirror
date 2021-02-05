#pragma once

#include <stdlib.h>
#include <stdbool.h>



#define PROGFLAGS_LONG_MAX  3
#define PROGFLAGS_LONG_SIZE 10
#define PROGFLAGS_DESC_SIZE 200
#define PROGFLAGS_ACTION_SIZE 20



extern char *progflags_exec_name;



typedef enum ProgFlag_Type {
	type_bool = 0x0,
	type_int  = 0x1,
	type_text = 0x2
} ProgFlag_Type;

typedef struct ProgFlag {
	bool used;
	size_t index;
	char flag_short;
	char flags_long[PROGFLAGS_LONG_MAX][PROGFLAGS_LONG_SIZE];
	char desc[PROGFLAGS_DESC_SIZE];
	ProgFlag_Type type;
	union {
		bool value_bool;
		int  value_int;
		char *value_text;
	};
} ProgFlag;

extern ProgFlag *progflags;
extern size_t progflags_size;



extern char *prog_actions[PROGFLAGS_ACTION_SIZE];
extern size_t prog_actions_size;



size_t progflag_allocate();
void progflag_init(const size_t index);

ProgFlag* progflags_add(const char flag_short, const size_t flags_long_count, ...);
ProgFlag* progflags_add_spacer();

void progflags_process(const int argc, char *argv[]);
void progflags_process_value(const size_t flag_index, char *str);
size_t progflags_find_flag(const char *flag);

int prog_flags_count();
int prog_actions_count();

void display_help();
