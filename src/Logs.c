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
#include "Logs.h"

#include "StringUtils.h"
#include "pxnDefines.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>



void (*log_printer)(LOG_PRINTER_ARGS) = NULL;

#ifdef DEBUG
LogLevel current_level = LVL_ALL;
#else
LogLevel current_level = LVL_INFO;
#endif

char log_module[] = {0};

size_t count_warnings = 0;
size_t count_severe   = 0;
size_t count_fatal    = 0;

bool console_color_enabled = DEFAULT_CONSOLE_COLOR;



void set_log_printer( void (*printer)(LOG_PRINTER_ARGS) ) {
	log_printer = printer;
}

void set_log_module(char *name) {
	strlcpy(log_module, name, LOG_NAME_MAX);
}



void log_lines(void (*callback)(char *msg, ...), char *text) {
	if (callback == NULL)
		return;
	if (text == NULL) {
		(*callback) ("<null>");
		return;
	}
	size_t len = strlen(text) + 1;
	char *txt = malloc(len * sizeof(char));
	strlcpy(txt, text, len);
	char *ptr = txt;
	size_t pos;
	while (true) {
		pos = chrpos(ptr, '\n');
		if (pos == -1) {
			(*callback) (ptr);
			break;
		}
		ptr[pos] = '\0';
		(*callback) (ptr);
		ptr += pos;
		ptr++;
	}
	free(txt);
}

void log_dash() {
	if (log_printer == NULL) \
		set_log_printer(log_print); \
	log_printer(LVL_ALL, log_module, ""); \
}



void log_nl() {
	log_line("");
}

#define LOG_PRINT(LVL) \
	va_list args; \
	va_start(args, msg); \
	size_t buf_size = strlen(msg) + LOG_ARGS_LEN_MAX + 1; \
	char buf[buf_size]; \
	vsnprintf(buf, buf_size, msg, args); \
	if (log_printer == NULL) \
		set_log_printer(log_print); \
	log_printer(LVL, log_module, buf); \
	va_end(args);

void log_line(char *msg, ...) {
	LOG_PRINT(LVL_OFF);
}

void log_title(char *msg, ...) {
	LOG_PRINT(LVL_TITLE);
}

void log_detail(char *msg, ...) {
	LOG_PRINT(LVL_DETAIL);
}

void log_info(char *msg, ...) {
	LOG_PRINT(LVL_INFO);
}

void log_notice(char *msg, ...) {
	LOG_PRINT(LVL_NOTICE);
}

void log_warning(char *msg, ...) {
	count_warnings++;
	LOG_PRINT(LVL_WARNING)
}

void log_severe(char *msg, ...) {
	count_severe++;
	LOG_PRINT(LVL_SEVERE);
}

void log_fatal(char *msg, ...) {
	count_fatal++;
	LOG_PRINT(LVL_FATAL);
}



void log_print(LogLevel level, char *mod_name, char *msg) {
	// title
	if (level == LVL_TITLE) {
		size_t len = strlen(msg) + 30;
		char buf[len];
		buf[0] = '\0';
		bool color = has_color_enabled();
		snprintf(buf, len, "  %s[[ %s ]]%s ",
			(color ? COLOR_CYAN : ""),
			msg,
			(color ? COLOR_RESET : "")
		);
		log_write(buf);
		return;
	}
	if (level == LVL_ALL && strlen(msg) == 0) {
		log_write(" [ - - - ] ");
		return;
	}
	if (is_level_loggable(level)) {
		// level name
		char lvlName[LOG_NAME_MAX+1];
		log_level_to_name(level, lvlName);
		// plain line
		if (strlen(lvlName) == 0) {
			log_write(msg);
			return;
		}
		// module name
		char modName[LOG_NAME_MAX+4];
		if (mod_name[0] == '\0') {
			modName[0] = '\0';
		} else {
			snprintf(modName, LOG_NAME_MAX+4, "][%s", mod_name);
		}
		// message with level
		for (char *c=lvlName; (*c=toupper(*c)); ++c);
		str_pad_center(lvlName, 7);
		size_t msg_len = strlen(msg);
		size_t buf_size = msg_len + LOG_NAME_MAX+5;
		char buf[buf_size+1];
		snprintf(buf, buf_size, " [%s%s] %s", lvlName, modName, msg);
		if (msg_len-1 == LOG_ARGS_LEN_MAX) {
			buf[buf_size-4] = '.';
			buf[buf_size-3] = '.';
			buf[buf_size-2] = '.';
		}
		buf[buf_size-1] = '\0';
		log_write(buf);
	}
}

void log_write(char *line) {
	printf(line);
	if (line[strlen(line)-1] != '\n') {
		printf("\n");
	}
//TODO: log to file
}



bool is_level_loggable(const LogLevel level) {
	if (current_level == LVL_ALL)
		return true;
	if (level >= current_level)
		return true;
	return false;
}

void log_level_set(const LogLevel level) {
	current_level = level;
}

void log_level_to_name(const LogLevel level, char *name) {
	switch (level) {
	case LVL_OFF:     break;
	case LVL_ALL:     break;
	case LVL_TITLE:   break;
	case LVL_DETAIL:  strcpy(name, "Detail");  return;
	case LVL_INFO:    strcpy(name, "Info");    return;
	case LVL_NOTICE:  strcpy(name, "Notice");  return;
	case LVL_WARNING: strcpy(name, "Warning"); return;
	case LVL_SEVERE:  strcpy(name, "Severe");  return;
	case LVL_FATAL:   strcpy(name, "Fatal");   return;
	default: break;
	}
	name[0] = '\0';
}



size_t get_warning_count() {
	size_t count = count_warnings;
	count_warnings = 0;
	return count;
}

size_t get_severe_count() {
	size_t count = count_severe;
	count_severe = 0;
	return count;
}

size_t get_fatal_count() {
	size_t count = count_fatal;
	count_fatal = 0;
	return count;
}



bool has_warnings() {
	return (count_warnings > 0);
}

bool has_severe() {
	return (count_severe > 0);
}

bool has_fatal() {
	return (count_fatal > 0);
}



bool set_color_enabled(bool enabled) {
	console_color_enabled = enabled;
}

bool has_color_enabled() {
	return console_color_enabled;
}
