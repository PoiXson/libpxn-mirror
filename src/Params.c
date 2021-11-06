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
#include "Params.h"

#include "MemUtils.h"
#include "StringUtils.h"
#include "NumberUtils.h"
#include "Logs.h"

#include <stdarg.h>
#include <libgen.h>



char *params_exec_name;

Param *param_flags = NULL;
size_t param_flags_size = 0;



size_t param_allocate() {
	if (param_flags_size == 0) {
		param_flags_size = 1;
		param_flags = calloc(param_flags_size, sizeof(Param));
		if (param_flags == NULL) {
			log_fatal("Failed to allocate params array");
			exit(1);
		}
		return 0;
	}
	for (size_t index=0; index<param_flags_size; index++) {
		if (param_flags[index].used == false)
			return index;
	}
	size_t old_size = param_flags_size;
	size_t new_size = param_flags_size * 2;
	param_flags = realloc_zero(
		param_flags,
		sizeof(Param) * old_size,
		sizeof(Param) * new_size
	);
	if (param_flags == NULL) {
		log_fatal("Failed to reallocate params array");
		exit(1);
	}
	param_flags_size = new_size;
	return old_size;
}



size_t param_add(const char flag_short, const size_t flags_long_count, ...) {
	size_t index = param_allocate();
	Param *param = &(param_flags[index]);
	param->used = true;
	param->flag_short = flag_short;
	va_list flags_long;
	va_start(flags_long, flags_long_count);
	for (size_t flag_index=0; flag_index<PARAMS_LONG_MAX; flag_index++) {
		if (flag_index < flags_long_count) {
			str_l_cpy(
				param->flags_long[flag_index],
				va_arg(flags_long, char*),
				PARAMS_LONG_SIZE
			);
		} else {
			param->flags_long[flag_index][0] = '\0';
		}
	}
	char *desc = va_arg(flags_long, char*);
	str_l_cpy(param->desc, desc, PARAMS_DESC_SIZE);
	va_end(flags_long);
	return index;
}

size_t param_add_spacer() {
	size_t index = param_allocate();
	Param *param = &(param_flags[index]);
	param->used = true;
	param->flag_short = '-';
	return index;
}



void params_process(const int argc, char *argv[], const char *exec_name) {
	// program file name
	if (str_not_empty(exec_name)) {
		size_t len = str_len(exec_name) + 1;
		params_exec_name = calloc(len, sizeof(char));
		str_l_cpy(params_exec_name, exec_name, len);
	} else {
		size_t len = str_len(argv[0]) + 1;
		params_exec_name = calloc(len, sizeof(char));
		str_l_cpy(params_exec_name, basename(argv[0]), len);
	}
	size_t flag_index = -1;
	// parse args
	// ARGS_LOOP:
	for (int arg_index=1; arg_index<argc; arg_index++) {
		// is a flag
		if (argv[arg_index][0] == '-') {
			// --flag
			if (argv[arg_index][1] == '-') {
				// --flag=value
				size_t pos = chr_pos(argv[arg_index], '=');
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
				if (param_flags[flag_index].type == PARAM_TYPE_BOOL
				|| arg_index == argc - 1
				|| argv[arg_index+1][0] == '-') {
					param_flags[flag_index].type = PARAM_TYPE_BOOL;
					param_flags[flag_index].value_bool = true;
				} else {
					arg_index++;
					params_process_value(flag_index, argv[arg_index]);
				}
				continue;
			// -x flag
			} else {
				size_t len = str_len(argv[arg_index]);
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
					if (param_flags[flag_index].type == PARAM_TYPE_BOOL
					|| arg_index == argc - 1
					|| argv[arg_index+1][0] == '-') {
						param_flags[flag_index].type = PARAM_TYPE_BOOL;
						param_flags[flag_index].value_bool = true;
					} else {
						arg_index++;
						params_process_value(flag_index, argv[arg_index]);
					}
					continue;
				} else {
					// -flag=value
					size_t pos = chr_pos(argv[arg_index], '=');
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
						if (param_flags[flag_index].type == PARAM_TYPE_BOOL
						|| arg_index == argc - 1
						|| argv[arg_index+1][0] == '-') {
							param_flags[flag_index].value_bool = true;
							param_flags[flag_index].type = PARAM_TYPE_BOOL;
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
	} // end ARGS_LOOP
}

void params_process_value(const size_t flag_index, const char *str) {
	switch (param_flags[flag_index].type) {
	case PARAM_TYPE_BOOL:
		if (str_l_cmp(str, "yes", 4) == 0 || str_l_cmp(str, "true", 5) == 0) {
			param_flags[flag_index].value_bool = true;
			return;
		}
		if (str_l_cmp(str, "no", 3) == 0 || str_l_cmp(str, "false", 6) == 0) {
			param_flags[flag_index].value_bool = false;
			return;
		}
		log_warning("Unknown argument value: %s", str);
		exit(1);
	case PARAM_TYPE_INT:
		param_flags[flag_index].value_int = atoi(str);
		return;
	case PARAM_TYPE_TEXT: {
		size_t len = str_len(str) + 1;
		param_flags[flag_index].value_text = calloc(len, sizeof(char));
		str_l_cpy(param_flags[flag_index].value_text, str, len);
		return;
	}
	default:
		log_fatal("Unknown flag value type: %lu", param_flags[flag_index].type);
		exit(1);
	}
}



size_t params_find_flag(const char *flag) {
	if (str_len(flag) < 2) return -1;
	if (flag[0] != '-')   return -1;
	flag++;
	// --flag
	if (flag[0] == '-') {
		flag++;
		for (size_t index=0; index<param_flags_size; index++) {
			if (param_flags[index].used != true) continue;
			for (size_t i=0; i<PARAMS_LONG_MAX; i++) {
				if (param_flags[index].flags_long[i][0] == '\0')         continue;
				if (str_cmp(flag, param_flags[index].flags_long[i]) == 0) return index;
			}
		}
	// -x flag
	} else {
		for (size_t index=0; index<param_flags_size; index++) {
			if (param_flags[index].used != true)       continue;
			if (param_flags[index].flag_short == '\0') continue;
			if (param_flags[index].flag_short == '-')  continue;
			if (param_flags[index].flag_short == flag[0])
				return index;
		}
	}
	return -1;
}



bool params_get_bool(size_t index) {
	switch (param_flags[index].type) {
	case PARAM_TYPE_BOOL: return   param_flags[index].value_bool;
	case PARAM_TYPE_INT:  return  (param_flags[index].value_int != 0);
	case PARAM_TYPE_TEXT: return ((param_flags[index].value_text)[0]) != '\0';
	default: break;
	}
	return false;
}

char* params_get_text(size_t index) {
	switch (param_flags[index].type) {
	case PARAM_TYPE_BOOL:
		if (param_flags[index].value_bool)
			return str_dup("true");
		return str_dup("false");
	case PARAM_TYPE_INT:  return i_to_str(param_flags[index].value_int);
	case PARAM_TYPE_TEXT: return str_dup (param_flags[index].value_text);
	default: break;
	}
	return NULL;
}



size_t get_params_count() {
	size_t count = 0;
	for (size_t index=0; index<param_flags_size; index++) {
		if (param_flags[index].used == true)
			count++;
	}
	return count;
}



void display_help() {
	size_t options_count = get_params_count();
//	size_t actions_count = prog_actions_count();
	bool color = has_log_color_enabled();
	log_nl();
	log_line(
		"%sUsage:%s",
		(color ? COLOR_YELLOW : ""),
		(color ? COLOR_RESET  : "")
	);
	{
		log_line(
			"  %s%s",
			params_exec_name,
			(options_count > 0 ? " [options]" : "")
//			(actions_count > 0 ? " [actions]" : "")
		);
	}
	log_nl();
//	if (actions_count > 0) {
//		log_line("Actions:");
//TODO: display available actions
//	}
	if (options_count > 0) {
		log_line(
			"%sOptions:%s",
			(color ? COLOR_YELLOW : ""),
			(color ? COLOR_RESET  : "")
		);
		size_t line_size = PARAMS_DESC_SIZE + PARAMS_FLAGS_SPACE + 16;
		char line[line_size];
		size_t f_index;
		for (size_t index=0; index<param_flags_size; index++) {
			if (param_flags[index].used != true)
				continue;
			// spacer
			if (param_flags[index].flag_short == '-') {
				log_nl();
				continue;
			}
			// -x short flag
			f_index = 0;
			str_l_cpy(line, "  ", line_size);
			if (param_flags[index].flag_short != '\0') {
				line[2] = '-';
				line[3] = param_flags[index].flag_short;
				line[4] = '\0';
				f_index++;
			}
			// --long flags
			for (size_t i=0; i<PARAMS_LONG_MAX; i++) {
				if (param_flags[index].flags_long[i][0] == '\0')
					continue;
				if (f_index >= 3)
					break;
				// flag doesn't fit on line
				if (str_len(line) + str_len(param_flags[index].flags_long[i]) > PARAMS_FLAGS_SPACE-5)
					continue;
				str_l_cat(
					line,
					(f_index == 0 ? "--" : ", --"),
					line_size
				);
				str_l_cat(line, param_flags[index].flags_long[i], line_size);
				f_index++;
			}
			// no flags
			if (f_index == 0)
				continue;
//			str_pad_end(line, PARAMS_FLAGS_SPACE - 1);
			line[PARAMS_FLAGS_SPACE-1] = ' ';
			line[PARAMS_FLAGS_SPACE  ] = '\0';
//			if (color) {
//				str_f_cat(line, COLOR_GREEN, line_size);
//				str_l_cat(line, COLOR_RESET, line_size);
//			}
			// description
			{
				char *desc = param_flags[index].desc;
				size_t pos;
				while (true) {
					pos = chr_pos(desc, '\n');
					if (pos == -1) {
						str_l_cat(line, desc, line_size);
						break;
					}
					// multi-line description
					desc[pos] = '\0';
					str_l_cat(line, desc, line_size);
					desc += pos+1;
					if (str_len(desc) == 0)
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
