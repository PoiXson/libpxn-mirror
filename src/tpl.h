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



#define TPL_VAR_NAME_MAX 32



// uncomment to enable
//#define DEBUG_TPL



typedef enum TPL_Node_Type {
	NODE_TYPE_COMMENT   = 0x0,
	NODE_TYPE_TEXT      = 0x1,
	NODE_TYPE_GET_VAR   = 0x2,
//	NODE_TYPE_SET_VAR   = 0x3
//	NODE_TYPE_INCLUDE   = 0x4
	NODE_TYPE_IF        = 0x5,
	NODE_TYPE_ELSE      = 0x6
//	NODE_TYPE_LOOP      = 0x
//	NODE_TYPE_ENDLOOP   = 0x
//	NODE_TYPE_CONTINUE  = 0x
//	NODE_TYPE_BREAK     = 0x
} TPL_Node_Type;

typedef enum TPL_Cond_Type {
	COND_TYPE_TRUE    = 0x0,
	COND_TYPE_FALSE   = 0x1,
	COND_TYPE_BOOLVAR = 0x2
} TPL_Cond_Type;

typedef struct TPL_Cond_BoolVar {
	char boolvar_name[TPL_VAR_NAME_MAX];
} TPL_Cond_BoolVar;

typedef struct TPL_Node {
	bool used;
	TPL_Node_Type type;
	size_t line;
	union {
		// text/comment
		struct {
			size_t text_size;
			char  *text;
		};
		// get var
		struct {
			char var_name[TPL_VAR_NAME_MAX];
		};
		// if block
		struct {
			TPL_Cond_Type cond_type;
			bool if_has_else;
			union {
				TPL_Cond_BoolVar *cond_boolvar;
			};
			size_t if_nodes_size;
			struct TPL_Node *if_nodes;
		};
		// else block
		struct {
			size_t else_nodes_size;
			struct TPL_Node *else_nodes;
		};
	};
} TPL_Node;

typedef struct TPL_Doc {
	char *file;
	size_t    nodes_size;
	TPL_Node *nodes;
} TPL_Doc;

typedef struct TPL_Var {
	bool used;
	char var_name[TPL_VAR_NAME_MAX];
	size_t value_size;
	char  *value;
} TPL_Var;

typedef struct TPL_Context {
	bool comments;
	size_t   vars_size;
	TPL_Var *vars;
} TPL_Context;



// tpl.c
size_t tpl_alloc_var (TPL_Context *context);
size_t tpl_alloc_node(TPL_Node **nodes, size_t *nodes_size);

void tpl_free_doc    (TPL_Doc *doc);
void tpl_free_nodes  (TPL_Node **nodes, size_t *nodes_size);
void tpl_free_node   (TPL_Node *node);
void tpl_free_vars   (TPL_Context *context);
void tpl_free_context(TPL_Context *context);

size_t tpl_doc_nodes_count(const TPL_Doc *doc);
size_t tpl_nodes_count(const TPL_Node *nodes, const size_t nodes_size);

size_t tpl_find_var(TPL_Context *context, const char *name);
char*  tpl_get_var (TPL_Context *context, const char *name);
size_t tpl_set_var (TPL_Context *context, const char *name, const char *value);



// tpl_parse.c
TPL_Doc* tpl_parse_file(char *file);
TPL_Doc* tpl_parse_data(char *data, size_t data_size);
size_t tpl_parse(TPL_Doc *doc, TPL_Node **nodes,
	char *data, size_t data_size,
	size_t *current_pos, size_t *current_line);



// tpl_render.c
char* tpl_render(TPL_Doc *doc, TPL_Context *context);
void tpl_render_nodes(TPL_Doc *doc, TPL_Context *context,
	char **out, size_t *out_size, TPL_Node *nodes, size_t nodes_size);

void tpl_dump_doc  (const TPL_Doc *doc);
void tpl_dump_nodes(int *indent, const TPL_Node *nodes, const size_t nodes_size);
void tpl_dump_node (int *indent, const TPL_Node *node);
void tpl_dump_vars (const TPL_Context *context);
