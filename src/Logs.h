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

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h> // va_list
#include <limits.h> // INT_MIN/MAX



#define DEFAULT_CONSOLE_COLOR false



#define COLOR_BLACK       "\e[0;30m"
#define COLOR_BLUE        "\e[0;34m"
#define COLOR_GREEN       "\e[0;32m"
#define COLOR_CYAN        "\e[0;36m"
#define COLOR_RED         "\e[0;31m"
#define COLOR_PURPLE      "\e[0;35m"
#define COLOR_BROWN       "\e[0;33m"
#define COLOR_LIGHTGRAY   "\e[0;37m"
#define COLOR_DARKGRAY    "\e[1;30m"
#define COLOR_LIGHTBLUE   "\e[1;34m"
#define COLOR_LIGHTGREEN  "\e[1;32m"
#define COLOR_LIGHTCYAN   "\e[1;36m"
#define COLOR_LIGHTRED    "\e[1;31m"
#define COLOR_LIGHTPURPLE "\e[1;35m"
#define COLOR_YELLOW      "\e[1;33m"
#define COLOR_WHITE       "\e[1;37m"
#define COLOR_RESET       "\e[0m"



typedef enum LogLevel {
	LVL_OFF     = INT_MAX,
	LVL_ALL     = INT_MIN,
	LVL_TITLE   = 9000,
	LVL_DETAIL  = 100,
	LVL_INFO    = 500,
	LVL_NOTICE  = 700,
	LVL_WARNING = 800,
	LVL_SEVERE  = 900,
	LVL_FATAL   = 1000
} LogLevel;



#define LOG_PRINTER_ARGS const LogLevel, const char*, va_list args

typedef struct LoggerState {
	LogLevel current_level;
	bool color_enabled;

	size_t count_warning;
	size_t count_severe;
	size_t count_fatal;

	// printer/writer handler
	void (*printer)(LOG_PRINTER_ARGS);
	void (*writer)(const char *line);
} LoggerState;
extern LoggerState *logger_state;



void init_logger_state();

void log_nl();
void log_line   (const char *msg, ...);
void log_title  (const char *msg, ...);
void log_detail (const char *msg, ...);
void log_info   (const char *msg, ...);
void log_notice (const char *msg, ...);
void log_warning(const char *msg, ...);
void log_severe (const char *msg, ...);
void log_fatal  (const char *msg, ...);

void log_print(LOG_PRINTER_ARGS);
void log_write(const char *line);

bool  is_level_loggable(const LogLevel level);
void  log_level_set(const LogLevel level);

char* log_level_to_name_short(const LogLevel level);
char* log_level_to_name_full (const LogLevel level);
char* log_level_to_color(const LogLevel level);

size_t get_log_warning_count();
size_t get_log_severe_count();
size_t get_log_fatal_count();
size_t reset_log_counts();

bool has_log_color_enabled();
void set_log_color_enabled(bool enabled);
