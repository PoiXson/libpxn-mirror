#include "ProgFlags.h"

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#include "StringUtils.h"
#include "FileUtils.h"
#include "Logs.h"



char *progflags_exec_name;

ProgFlag *progflags;
size_t progflags_size = 0;

char *prog_actions[PROGFLAGS_ACTION_SIZE];
size_t prog_actions_size = 0;



size_t progflag_allocate() {
	// allocate first block
	if (progflags_size == 0) {
		progflags_size = 2;
		progflags = malloc(progflags_size * sizeof(ProgFlag));
		if (progflags == NULL) {
			log_fatal("Failed to allocate progflags array");
			exit(1);
		}
		for (size_t index=0; index<progflags_size; index++) {
			progflag_init(index);
		}
		progflags[0].used = true;
		return 0;
	}
	// find space in existing array
	for (size_t index=0; index<progflags_size; index++) {
		if (progflags[index].used == false) {
			progflag_init(index);
			progflags[index].used = true;
			return index;
		}
	}
	// expand array
	progflags = realloc(progflags, sizeof(ProgFlag) * progflags_size * 2);
	if (progflags == NULL) {
		log_fatal("Failed to reallocate progflags array");
		exit(1);
	}
	for (size_t index=progflags_size; index<progflags_size*2; index++) {
		progflag_init(index);
	}
	progflags_size *= 2;
	return progflag_allocate();
}

void progflag_init(const size_t index) {
	progflags[index].used  = false;
	progflags[index].index = index;
	progflags[index].flag_short = '\0';
	for (size_t i=0; i<PROGFLAGS_LONG_MAX; i++)
		memset(progflags[index].flags_long[i], '\0', PROGFLAGS_LONG_SIZE);
	memset(progflags[index].desc, '\0', PROGFLAGS_DESC_SIZE);
	progflags[index].type = FLAGTYPE_BOOL;
	progflags[index].value_text = NULL;
}



ProgFlag* progflags_add(const char flag_short, const size_t flags_long_count, ...) {
	size_t index = progflag_allocate();
	progflags[index].flag_short = flag_short;
	va_list flags_long;
	va_start(flags_long, flags_long_count);
	for (size_t i=0; i<PROGFLAGS_LONG_MAX; i++) {
		if (i < flags_long_count) {
			strlcpy(
				progflags[index].flags_long[i],
				va_arg(flags_long, char*),
				PROGFLAGS_LONG_SIZE
			);
		} else {
			progflags[index].flags_long[i][0] = '\0';
		}
	}
	char *desc = va_arg(flags_long, char*);
	strlcpy(progflags[index].desc, desc, PROGFLAGS_DESC_SIZE);
	va_end(flags_long);
	return &(progflags[index]);
}

ProgFlag* progflags_add_spacer() {
	size_t index = progflag_allocate();
	progflags[index].flag_short = '-';
	return &(progflags[index]);
}



void progflags_process(const int argc, char *argv[], char *exec_name) {
	// program file name
	if (exec_name != NULL && strlen(exec_name) > 0) {
		size_t len = strlen(exec_name) + 1;
		progflags_exec_name = malloc(len * sizeof(char));
		strlcpy(progflags_exec_name, exec_name, len);
	} else {
		size_t len = strlen(argv[0]) + 1;
		progflags_exec_name = malloc(len * sizeof(char));
		strlcpy(progflags_exec_name, basename(argv[0]), len);
	}
	size_t flag_index = -1;
	// parse args
	for (int arg_index=1; arg_index<argc; arg_index++) {
		// is a flag
		if (argv[arg_index][0] == '-') {
			// --flag
			if (argv[arg_index][1] == '-') {
				// --flag=value
				size_t pos = chrpos(argv[arg_index], '=');
				if (pos != -1) {
					argv[arg_index][pos] = '\0';
					flag_index = progflags_find_flag(argv[arg_index]);
					if (flag_index == -1) {
						log_severe("Unknown argument: %s", argv[arg_index]);
						exit(1);
					}
					pos++;
					progflags_process_value(flag_index, argv[arg_index]+pos);
					continue;
				}
				// --flag
				flag_index = progflags_find_flag(argv[arg_index]);
				if (flag_index == -1) {
					log_severe("Unknown argument: %s", argv[arg_index]);
					exit(1);
				}
				if (progflags[flag_index].type == FLAGTYPE_BOOL
				|| arg_index == argc - 1
				|| argv[arg_index+1][0] == '-') {
					progflags[flag_index].type = FLAGTYPE_BOOL;
					progflags[flag_index].value_bool = true;
				} else {
					arg_index++;
					progflags_process_value(flag_index, argv[arg_index]);
				}
				continue;
			// -x flag
			} else {
				size_t len = strlen(argv[arg_index]);
				if (len < 2) {
					log_severe("Unknown argument: %s", argv[arg_index]);
					exit(1);
				}
				// single -x flag
				if (len == 2) {
					flag_index = progflags_find_flag(argv[arg_index]);
					if (flag_index == -1) {
						log_severe("Unknown argument: %s", argv[arg_index]);
						exit(1);
					}
					if (progflags[flag_index].type == FLAGTYPE_BOOL
					|| arg_index == argc - 1
					|| argv[arg_index+1][0] == '-') {
						progflags[flag_index].type = FLAGTYPE_BOOL;
						progflags[flag_index].value_bool = true;
					} else {
						arg_index++;
						progflags_process_value(flag_index, argv[arg_index]);
					}
					continue;
				} else {
					// -flag=value
					size_t pos = chrpos(argv[arg_index], '=');
					if (pos != -1) {
						if (pos != 2) {
							log_severe("Unknown argument: %s", argv[arg_index]);
							exit(1);
						}
						argv[arg_index][pos] = '\0';
						flag_index = progflags_find_flag(argv[arg_index]);
						if (flag_index == -1) {
							log_severe("Unknown argument: %s", argv[arg_index]);
							exit(1);
						}
						pos++;
						progflags_process_value(flag_index, argv[arg_index]+pos);
						continue;
					}
					// multiple -flags
					char tmp[3];
					tmp[0] = '-';
					tmp[2] = '\0';
					for (size_t i=1; i<len; i++) {
						tmp[1] = argv[arg_index][i];
						flag_index = progflags_find_flag(tmp);
						if (flag_index == -1) {
							log_severe("Unknown argument: -%c", argv[arg_index][i]);
							exit(1);
						}
						if (progflags[flag_index].type == FLAGTYPE_BOOL
						|| arg_index == argc - 1
						|| argv[arg_index+1][0] == '-') {
							progflags[flag_index].value_bool = true;
							progflags[flag_index].type = FLAGTYPE_BOOL;
						} else {
							progflags_process_value(flag_index, argv[arg_index+1]);
						}
					}
					continue;
				}
			}
		} // end is flag
		log_severe("Unknown argument: %s", argv[arg_index]);
		exit(1);
	}
}

void progflags_process_value(const size_t flag_index, char *str) {
	switch (progflags[flag_index].type) {
	case FLAGTYPE_BOOL:
		if (strlcmp(str, "yes", 4) == 0 || strlcmp(str, "true", 5) == 0) {
			progflags[flag_index].value_bool = true;
			return;
		}
		if (strlcmp(str, "no", 3) == 0 || strlcmp(str, "false", 6) == 0) {
			progflags[flag_index].value_bool = false;
			return;
		}
		log_warning("Unknown argument value: %s", str);
		exit(1);
	case FLAGTYPE_INT:
		progflags[flag_index].value_int = atoi(str);
		return;
	case FLAGTYPE_TEXT: {
		size_t len = strlen(str) + 1;
		progflags[flag_index].value_text = calloc(len, sizeof(char));
		strlcpy(progflags[flag_index].value_text, str, len);
		return;
	}
	default:
		log_fatal("Unknown flag value type: %lu", progflags[flag_index].type);
		exit(1);
	}
}

size_t progflags_find_flag(const char *flag) {
	if (strlen(flag) < 2) return -1;
	if (flag[0] != '-')   return -1;
	flag++;
	// --flag
	if (flag[0] == '-') {
		flag++;
		for (size_t index=0; index<progflags_size; index++) {
			if (progflags[index].used != true)
				continue;
			for (size_t i=0; i<PROGFLAGS_LONG_MAX; i++) {
				if (progflags[index].flags_long[i][0] == '\0')
					continue;
				if (strcmp(flag, progflags[index].flags_long[i]) == 0)
					return index;
			}
		}
	// -x flag
	} else {
		for (size_t index=0; index<progflags_size; index++) {
			if (progflags[index].used != true)
				continue;
			if (progflags[index].flag_short == '-')
				continue;
			if (progflags[index].flag_short == flag[0])
				return index;
		}
	}
	return -1;
}



size_t prog_flags_count() {
	size_t count = 0;
	for (size_t index=0; index<progflags_size; index++) {
		if (progflags[index].used == true)
			count++;
	}
	return count;
}

size_t prog_actions_count() {
//TODO
return 0;
}



void display_help() {
	size_t options_count = prog_flags_count();
	size_t actions_count = prog_actions_count();
	log_line("");
	log_line("Usage: ");
	{
		log_line(
			"  %s%s%s",
			progflags_exec_name,
			(options_count > 0 ? " [options]" : ""),
			(actions_count > 0 ? " [actions]" : "")
		);
	}
	log_line("");
	if (actions_count > 0) {
//		log_line("Actions:");
//TODO: display available actions
	}
	if (options_count > 0) {
		log_line("Options:");
		size_t len = PROGFLAGS_DESC_SIZE + 30;
		char line[len];
		size_t f_index;
		for (size_t index=0; index<progflags_size; index++) {
			// spacer
			if (progflags[index].flag_short == '-') {
				log_line("");
				continue;
			}
			// -x short flag
			f_index = 0;
			strlcpy(line, "  ", len);
			if (progflags[index].flag_short != '\0') {
				line[2] = '-';
				line[3] = progflags[index].flag_short;
				line[4] = '\0';
				f_index++;
			}
			// --long flags
			for (size_t i=0; i<PROGFLAGS_LONG_MAX; i++) {
				if (progflags[index].flags_long[i][0] == '\0')
					continue;
				if (f_index >= 3)
					break;
				if (f_index == 0) {
					strlcat(line, "--", len);
				} else {
					strlcat(line, ", --", len);
				}
				strlcat(line, progflags[index].flags_long[i], len);
				f_index++;
			}
			// no flags
			if (line[3] == '\0')
				continue;
			str_pad_end(line, 24);
			line[24] = ' ';
			line[25] = '\0';
			// description
			{
				char *desc = progflags[index].desc;
				size_t pos;
				while (true) {
					pos = chrpos(desc, '\n');
					if (pos == -1) {
						strlcat(line, desc, len);
						break;
					}
					// multi-line description
					desc[pos] = '\0';
					strlcat(line, desc, len);
					desc += pos+1;
					if (strlen(desc) == 0)
						break;
					log_line(line);
					for (int i=0; i<27; i++)
						line[i] = ' ';
					line[27] = '\0';
				}
			}
			log_line(line);
		}
	}
	log_line("");
	exit(1);
}
