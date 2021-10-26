#include "Params.h"

#include "StringUtils.h"
#include "NumberUtils.h"
#include "FileUtils.h"
#include "Logs.h"

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>



char *params_exec_name;

Param *params;
size_t params_size = 0;

char *param_actions[PARAMS_ACTION_SIZE];
size_t param_actions_size = 0;



size_t param_allocate() {
	// allocate first block
	if (params_size == 0) {
		params_size = 2;
		params = malloc(params_size * sizeof(Param));
		if (params == NULL) {
			log_fatal("Failed to allocate params array");
			exit(1);
		}
		for (size_t index=0; index<params_size; index++) {
			param_init(index);
		}
		params[0].used = true;
		return 0;
	}
	// find space in existing array
	for (size_t index=0; index<params_size; index++) {
		if (params[index].used == false) {
			param_init(index);
			params[index].used = true;
			return index;
		}
	}
	// expand array
	params = realloc(params, sizeof(Param) * params_size * 2);
	if (params == NULL) {
		log_fatal("Failed to reallocate params array");
		exit(1);
	}
	for (size_t index=params_size; index<params_size*2; index++) {
		param_init(index);
	}
	params_size *= 2;
	return param_allocate();
}

void param_init(const size_t index) {
	params[index].used  = false;
	params[index].index = index;
	params[index].flag_short = '\0';
	for (size_t i=0; i<PARAMS_LONG_MAX; i++)
		memset(params[index].flags_long[i], '\0', PARAMS_LONG_SIZE);
	memset(params[index].desc, '\0', PARAMS_DESC_SIZE);
	params[index].type = FLAGTYPE_BOOL;
	params[index].value_text = NULL;
}



size_t param_add(const char flag_short, const size_t flags_long_count, ...) {
	size_t index = param_allocate();
	params[index].flag_short = flag_short;
	va_list flags_long;
	va_start(flags_long, flags_long_count);
	for (size_t i=0; i<PARAMS_LONG_MAX; i++) {
		if (i < flags_long_count) {
			strlcpy(
				params[index].flags_long[i],
				va_arg(flags_long, char*),
				PARAMS_LONG_SIZE
			);
		} else {
			params[index].flags_long[i][0] = '\0';
		}
	}
	char *desc = va_arg(flags_long, char*);
	strlcpy(params[index].desc, desc, PARAMS_DESC_SIZE);
	va_end(flags_long);
	return index;
}

size_t param_add_spacer() {
	size_t index = param_allocate();
	params[index].flag_short = '-';
	return index;
}



void params_process(const int argc, char *argv[], char *exec_name) {
	// program file name
	if (exec_name != NULL && strlen(exec_name) > 0) {
		size_t len = strlen(exec_name) + 1;
		params_exec_name = malloc(len * sizeof(char));
		strlcpy(params_exec_name, exec_name, len);
	} else {
		size_t len = strlen(argv[0]) + 1;
		params_exec_name = malloc(len * sizeof(char));
		strlcpy(params_exec_name, basename(argv[0]), len);
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
					flag_index = params_find_flag(argv[arg_index]);
					if (flag_index == -1) {
						log_severe("Unknown argument: %s", argv[arg_index]);
						exit(1);
					}
					pos++;
					params_process_value(flag_index, argv[arg_index]+pos);
					continue;
				}
				// --flag
				flag_index = params_find_flag(argv[arg_index]);
				if (flag_index == -1) {
					log_severe("Unknown argument: %s", argv[arg_index]);
					exit(1);
				}
				if (params[flag_index].type == FLAGTYPE_BOOL
				|| arg_index == argc - 1
				|| argv[arg_index+1][0] == '-') {
					params[flag_index].type = FLAGTYPE_BOOL;
					params[flag_index].value_bool = true;
				} else {
					arg_index++;
					params_process_value(flag_index, argv[arg_index]);
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
					flag_index = params_find_flag(argv[arg_index]);
					if (flag_index == -1) {
						log_severe("Unknown argument: %s", argv[arg_index]);
						exit(1);
					}
					if (params[flag_index].type == FLAGTYPE_BOOL
					|| arg_index == argc - 1
					|| argv[arg_index+1][0] == '-') {
						params[flag_index].type = FLAGTYPE_BOOL;
						params[flag_index].value_bool = true;
					} else {
						arg_index++;
						params_process_value(flag_index, argv[arg_index]);
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
						flag_index = params_find_flag(argv[arg_index]);
						if (flag_index == -1) {
							log_severe("Unknown argument: %s", argv[arg_index]);
							exit(1);
						}
						pos++;
						params_process_value(flag_index, argv[arg_index]+pos);
						continue;
					}
					// multiple -flags
					char tmp[3];
					tmp[0] = '-';
					tmp[2] = '\0';
					for (size_t i=1; i<len; i++) {
						tmp[1] = argv[arg_index][i];
						flag_index = params_find_flag(tmp);
						if (flag_index == -1) {
							log_severe("Unknown argument: -%c", argv[arg_index][i]);
							exit(1);
						}
						if (params[flag_index].type == FLAGTYPE_BOOL
						|| arg_index == argc - 1
						|| argv[arg_index+1][0] == '-') {
							params[flag_index].value_bool = true;
							params[flag_index].type = FLAGTYPE_BOOL;
						} else {
							params_process_value(flag_index, argv[arg_index+1]);
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

void params_process_value(const size_t flag_index, char *str) {
	switch (params[flag_index].type) {
	case FLAGTYPE_BOOL:
		if (strlcmp(str, "yes", 4) == 0 || strlcmp(str, "true", 5) == 0) {
			params[flag_index].value_bool = true;
			return;
		}
		if (strlcmp(str, "no", 3) == 0 || strlcmp(str, "false", 6) == 0) {
			params[flag_index].value_bool = false;
			return;
		}
		log_warning("Unknown argument value: %s", str);
		exit(1);
	case FLAGTYPE_INT:
		params[flag_index].value_int = atoi(str);
		return;
	case FLAGTYPE_TEXT: {
		size_t len = strlen(str) + 1;
		params[flag_index].value_text = calloc(len, sizeof(char));
		strlcpy(params[flag_index].value_text, str, len);
		return;
	}
	default:
		log_fatal("Unknown flag value type: %lu", params[flag_index].type);
		exit(1);
	}
}

size_t params_find_flag(const char *flag) {
	if (strlen(flag) < 2) return -1;
	if (flag[0] != '-')   return -1;
	flag++;
	// --flag
	if (flag[0] == '-') {
		flag++;
		for (size_t index=0; index<params_size; index++) {
			if (params[index].used != true) continue;
			for (size_t i=0; i<PARAMS_LONG_MAX; i++) {
				if (params[index].flags_long[i][0] == '\0')         continue;
				if (strcmp(flag, params[index].flags_long[i]) == 0) return index;
			}
		}
	// -x flag
	} else {
		for (size_t index=0; index<params_size; index++) {
			if (params[index].used != true)       continue;
			if (params[index].flag_short == '\0') continue;
			if (params[index].flag_short == '-')  continue;
			if (params[index].flag_short == flag[0])
				return index;
		}
	}
	return -1;
}



bool params_get_bool(size_t index) {
	switch (params[index].type) {
	case FLAGTYPE_BOOL: return   params[index].value_bool;
	case FLAGTYPE_INT:  return  (params[index].value_int != 0);
	case FLAGTYPE_TEXT: return ((params[index].value_text)[0]) != '\0';
	default: break;
	}
	return false;
}

char* params_get_text(size_t index) {
	switch (params[index].type) {
	case FLAGTYPE_BOOL:
		if (params[index].value_bool)
			return strdup("true");
		return strdup("false");
	case FLAGTYPE_INT:  return itostr(params[index].value_int);
	case FLAGTYPE_TEXT: return strdup(params[index].value_text);
	default: break;
	}
	return NULL;
}



size_t prog_flags_count() {
	size_t count = 0;
	for (size_t index=0; index<params_size; index++) {
		if (params[index].used == true)
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
	bool color = has_color_enabled();
	log_nl();
	log_line("%sUsage:%s",
		(color ? COLOR_YELLOW : ""),
		(color ? COLOR_RESET  : "")
	);
	{
		log_line(
			"  %s%s%s",
			params_exec_name,
			(options_count > 0 ? " [options]" : ""),
			(actions_count > 0 ? " [actions]" : "")
		);
	}
	log_nl();
	if (actions_count > 0) {
//		log_line("Actions:");
//TODO: display available actions
	}
	if (options_count > 0) {
		log_line("%sOptions:%s",
			(color ? COLOR_YELLOW : ""),
			(color ? COLOR_RESET  : "")
		);
		size_t line_size = PARAMS_DESC_SIZE + PARAMS_FLAGS_SPACE + 16;
		char line[line_size];
		size_t f_index;
		for (size_t index=0; index<params_size; index++) {
			if (params[index].used != true)
				continue;
			// spacer
			if (params[index].flag_short == '-') {
				log_nl();
				continue;
			}
			// -x short flag
			f_index = 0;
			strlcpy(line, "  ", line_size);
			if (params[index].flag_short != '\0') {
				line[2] = '-';
				line[3] = params[index].flag_short;
				line[4] = '\0';
				f_index++;
			}
			// --long flags
			for (size_t i=0; i<PARAMS_LONG_MAX; i++) {
				if (params[index].flags_long[i][0] == '\0')
					continue;
				if (f_index >= 3)
					break;
				// flag doesn't fit on line
				if (strlen(line) + strlen(params[index].flags_long[i]) > PARAMS_FLAGS_SPACE-5)
					continue;
				strlcat(
					line,
					(f_index == 0 ? "--" : ", --"),
					line_size
				);
				strlcat(line, params[index].flags_long[i], line_size);
				f_index++;
			}
			// no flags
			if (f_index == 0)
				continue;
			str_pad_end(line, PARAMS_FLAGS_SPACE - 1);
			line[PARAMS_FLAGS_SPACE-1] = ' ';
			line[PARAMS_FLAGS_SPACE  ] = '\0';
			if (color) {
				strlcatfront(line, COLOR_GREEN, line_size);
				strlcat     (line, COLOR_RESET, line_size);
			}
			// description
			{
				char *desc = params[index].desc;
				size_t pos;
				while (true) {
					pos = chrpos(desc, '\n');
					if (pos == -1) {
						strlcat(line, desc, line_size);
						break;
					}
					// multi-line description
					desc[pos] = '\0';
					strlcat(line, desc, line_size);
					desc += pos+1;
					if (strlen(desc) == 0)
						break;
					log_line(line);
					for (int i=0; i<PARAMS_FLAGS_SPACE+2; i++)
						line[i] = ' ';
					line[PARAMS_FLAGS_SPACE+2] = '\0';
				}
			}
			log_line(line);
		}
	}
	log_nl();
	exit(1);
}
