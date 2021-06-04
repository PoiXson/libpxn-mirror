#pragma once

#include <stdlib.h>
#include <stdbool.h>



#define PROGFLAGS_LONG_MAX  3
#define PROGFLAGS_LONG_SIZE 10
#define PROGFLAGS_DESC_SIZE 200
#define PROGFLAGS_ACTION_SIZE 20
#define PROGFLAGS_FLAGS_SPACE 25



extern char *progflags_exec_name;



typedef enum ProgFlag_Type {
	FLAGTYPE_BOOL = 0x0,
	FLAGTYPE_INT  = 0x1,
	FLAGTYPE_TEXT = 0x2
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



extern char*  prog_actions[PROGFLAGS_ACTION_SIZE];
extern size_t prog_actions_size;



size_t progflag_allocate();
void   progflag_init(const size_t index);

size_t progflags_add(const char flag_short, const size_t flags_long_count, ...);
size_t progflags_add_spacer();

void   progflags_process(const int argc, char *argv[], char *exec_name);
void   progflags_process_value(const size_t flag_index, char *str);
size_t progflags_find_flag(const char *flag);

bool  progflags_get_bool(size_t index);
char* progflags_get_text(size_t index);

size_t prog_flags_count();
size_t prog_actions_count();

void display_help();
