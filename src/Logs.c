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
#include "NumberUtils.h"

#include <stdlib.h> // size_t



LoggerState *logger_state = NULL;



void init_logger_state() {
	if (logger_state == NULL) {
		logger_state = calloc(1, sizeof(LoggerState));
		// default log level
		#ifdef DEBUG
			logger_state->current_level = LVL_ALL;
		#else
			logger_state->current_level = LVL_INFO;
		#endif
	}
	if (logger_state->printer == NULL) logger_state->printer = log_print;
	if (logger_state->writer  == NULL) logger_state->writer  = log_write;
}



// ========================================



void log_nl() {
	log_line("");
}

void log_line(const char *msg, ...) {
	init_logger_state();
	va_list args;
	va_start(args, msg);
	(logger_state->printer)(LVL_OFF, msg, args);
	va_end(args);
}

void log_title(const char *msg, ...) {
	init_logger_state();
	va_list args;
	va_start(args, msg);
	(logger_state->printer)(LVL_TITLE, msg, args);
	va_end(args);
}

void log_detail(const char *msg, ...) {
	init_logger_state();
	va_list args;
	va_start(args, msg);
	(logger_state->printer)(LVL_DETAIL, msg, args);
	va_end(args);
}

void log_info(const char *msg, ...) {
	init_logger_state();
	va_list args;
	va_start(args, msg);
	(logger_state->printer)(LVL_INFO, msg, args);
	va_end(args);
}

void log_notice(const char *msg, ...) {
	init_logger_state();
	va_list args;
	va_start(args, msg);
	(logger_state->printer)(LVL_NOTICE, msg, args);
	va_end(args);
}

void log_warning(const char *msg, ...) {
	init_logger_state();
	logger_state->count_warning++;
	va_list args;
	va_start(args, msg);
	(logger_state->printer)(LVL_WARNING, msg, args);
	va_end(args);
}

void log_severe(const char *msg, ...) {
	init_logger_state();
	logger_state->count_severe++;
	va_list args;
	va_start(args, msg);
	(logger_state->printer)(LVL_SEVERE, msg, args);
	va_end(args);
}

void log_fatal(const char *msg, ...) {
	init_logger_state();
	logger_state->count_fatal++;
	va_list args;
	va_start(args, msg);
	(logger_state->printer)(LVL_FATAL, msg, args);
	va_end(args);
}



void log_print(const LogLevel level, const char *msg, va_list args) {
	init_logger_state();

	if (msg == NULL) {
		logger_state->printer(level, "(null)", NULL);
		return;
	}

	if (str_cmp(msg, "---") == 0) {
		logger_state->writer(" [ - - - ] ");
		return;
	}
	bool color = has_log_color_enabled();

	// render message
	size_t msg_ready_size = 0;
	char *msg_ready = vsnprintf_alloc(&msg_ready_size, msg, args);

	// title
	if (level == LVL_TITLE) {
		size_t buf_size = 0;
		char *buf = snprintf_alloc(
			&buf_size,
			"  %s[[ %s ]]%s ",
			(color ? COLOR_BROWN : ""),
			msg,
			(color ? COLOR_RESET : "")
		);
		logger_state->writer(buf);
		free(buf);
		free(msg_ready);
		return;
	}

	// level name
	char *level_name  = log_level_to_name_short(level);

	// plain line
	if (str_empty(level_name)) {
		if (level_name != NULL)
			free(level_name);
		logger_state->writer(msg_ready);
		free(msg_ready);
		return;
	}

	// loggable message
	if (is_level_loggable(level)) {
		char *level_color = NULL;
		if (color)
			level_color = log_level_to_color(level);
		size_t msg_len = str_len(msg);
		size_t buf_size = 0;
		char *buf = snprintf_alloc(
			&buf_size,
			" %s[%s]%s %s",
			(color ? level_color : ""),
			level_name,
			(color ? COLOR_RESET : ""),
			msg_ready
		);
		logger_state->writer(buf);
		free(buf);
		if (level_color != NULL)
			free(level_color);
	}
	free(level_name);
	free(msg_ready);
}

void log_write(const char *line) {
	printf(line);
	if (!str_ends_with(line, '\n'))
		printf("\n");
}



// ========================================
// log level



bool is_level_loggable(const LogLevel level) {
	init_logger_state();
	return (
		logger_state->current_level == LVL_ALL ||
		logger_state->current_level <= level
	);
}

void log_level_set(const LogLevel level) {
	init_logger_state();
	logger_state->current_level = level;
}



char* log_level_to_name_short(const LogLevel level) {
	switch (level) {
		case LVL_DETAIL:  return str_l_dup("detl", 4);
		case LVL_INFO:    return str_l_dup("info", 4);
		case LVL_NOTICE:  return str_l_dup("note", 4);
		case LVL_WARNING: return str_l_dup("Warn", 4);
		case LVL_SEVERE:  return str_l_dup("SEVR", 4);
		case LVL_FATAL:   return str_l_dup("FAIL", 4);
		default:
			return NULL;
	}
}

char* log_level_to_name_full(const LogLevel level) {
	switch (level) {
		case LVL_DETAIL:  return str_dup("detail" );
		case LVL_INFO:    return str_dup("info"   );
		case LVL_NOTICE:  return str_dup("notice" );
		case LVL_WARNING: return str_dup("Warning");
		case LVL_SEVERE:  return str_dup("SEVERE" );
		case LVL_FATAL:   return str_dup("FATAL"  );
		default:
			return NULL;
	}
}

char* log_level_to_color(const LogLevel level) {
	switch (level) {
		case LVL_DETAIL:  return str_dup(COLOR_CYAN);
		case LVL_INFO:    return str_dup(COLOR_CYAN);
		case LVL_NOTICE:  return str_dup(COLOR_BROWN);
		case LVL_WARNING: return str_dup(COLOR_BROWN);
		case LVL_SEVERE:  return str_dup(COLOR_RED);
		case LVL_FATAL:   return str_dup(COLOR_RED);
		default:
			return NULL;
	}
}



bool has_log_color_enabled() {
	init_logger_state();
	return logger_state->color_enabled;
}
void set_log_color_enabled(bool enabled) {
	init_logger_state();
	logger_state->color_enabled = enabled;
}



// ========================================



size_t get_log_warning_count() {
	return logger_state->count_warning;
}
size_t get_log_severe_count() {
	return logger_state->count_severe;
}
size_t get_log_fatal_count() {
	return logger_state->count_fatal;
}

bool has_log_warnsevfat() {
	return (
		logger_state->count_warning > 0 ||
		logger_state->count_severe  > 0 ||
		logger_state->count_fatal   > 0
	);
}

size_t reset_log_counts() {
	init_logger_state();
	size_t total = 0;
	total += logger_state->count_warning;
	total += logger_state->count_severe;
	total += logger_state->count_fatal;
	logger_state->count_warning = 0;
	logger_state->count_severe  = 0;
	logger_state->count_fatal   = 0;
	return total;
}
