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
#include "tpl.h"

#include "MemUtils.h"
#include "StringUtils.h"



// ========================================
// allocate arrays



size_t tpl_alloc_var(TPL_Context *context) {
	if (context->vars_size == 0) {
		context->vars_size = 1;
		context->vars = calloc(context->vars_size, sizeof(TPL_Var));
		if (context->vars == NULL) {
			log_fatal("Failed to allocate tpl vars array");
			exit(1);
		}
		return 0;
	}
	for (size_t index=0; index<context->vars_size; index++) {
		if ((context->vars)[index].used == false)
			return index;
	}
	size_t old_size = context->vars_size;
	size_t new_size = context->vars_size * 2;
	context->vars = realloc_zero(
		context->vars,
		sizeof(TPL_Var) * old_size,
		sizeof(TPL_Var) * new_size
	);
	if (context->vars == NULL) {
		log_fatal("Failed to reallocate tpl vars array");
		exit(1);
	}
	context->vars_size = new_size;
	return old_size;
}

size_t tpl_alloc_node(TPL_Node **nodes, size_t *nodes_size) {
	if (*nodes_size == 0) {
		*nodes_size = 1;
		*nodes = calloc(*nodes_size, sizeof(TPL_Node));
		if (*nodes == NULL) {
			log_fatal("Failed to allocate tpl nodes array");
			exit(1);
		}
		return 0;
	}
	// search from end to maintain order
	for (size_t index=(*nodes_size)-1; index>=0; index--) {
		if ((*nodes)[index].used != false) {
			if (index < (*nodes_size)-1)
				return index + 1;
			break;
		}
	}
	size_t old_size = *nodes_size;
	size_t new_size = (*nodes_size) * 2;
	*nodes = realloc_zero(
		*nodes,
		sizeof(TPL_Node) * old_size,
		sizeof(TPL_Node) * new_size
	);
	if (*nodes == NULL) {
		log_fatal("Failed to reallocate tpl nodes array");
		exit(1);
	}
	*nodes_size = new_size;
	return old_size;
}



void tpl_free_doc(TPL_Doc *doc) {
	if (doc == NULL) return;
	if (doc->file != NULL) {
		free(doc->file);
		doc->file = NULL;
	}
	tpl_free_nodes( &(doc->nodes), &(doc->nodes_size) );
	doc->nodes = NULL;
	free(doc);
}

void tpl_free_nodes(TPL_Node **nodes, size_t *nodes_size) {
	if (*nodes_size == 0) return;
	for (size_t index=0; index<*nodes_size; index++) {
		tpl_free_node( &((*nodes)[index]) );
	}
	*nodes_size = 0;
	free(*nodes);
}

void tpl_free_node(TPL_Node *node) {
	if (node == NULL) return;
	if (node->used == false) return;
	switch (node->type) {
		case NODE_TYPE_COMMENT:
		case NODE_TYPE_TEXT:
			if (node->text != NULL) {
				free(node->text);
				node->text_size = 0;
			}
			break;
		case NODE_TYPE_GET_VAR:
		default:
			break;
	}
	node->type = 0;
	node->line = 0;
	node->used = false;
}

void tpl_free_vars(TPL_Context *context) {
	if (context == NULL)         return;
	if (context->vars_size == 0) return;
	// FREE_VALS_LOOP:
	for (size_t index=0; index<context->vars_size; index++) {
		if ((context->vars)[index].value_size > 0) {
			TPL_Var *var = &((context->vars)[index]);
			if (var->value != NULL)
				free(var->value);
			var->value_size = 0;
			var->used       = false;
		}
	} // end FREE_VALS_LOOP
	free(context->vars);
	context->vars_size = 0;
}

void tpl_free_context(TPL_Context *context) {
	tpl_free_vars(context);
	free(context);
}



size_t tpl_doc_nodes_count(const TPL_Doc *doc) {
	return tpl_nodes_count(doc->nodes, doc->nodes_size);
}

size_t tpl_nodes_count(const TPL_Node *nodes, const size_t nodes_size) {
	size_t count = 0;
	// NODES_LOOP:
	for (size_t index=0; index<nodes_size; index++) {
		if (nodes[index].used == false)
			continue;
		count++;
//TODO: recursive nodes
	} // end NODES_LOOP
	return count;
}



// ========================================
// vars



size_t tpl_find_var(TPL_Context *context, const char *name) {
	// VARS_LOOP:
	for (size_t index=0; index<context->vars_size; index++) {
		TPL_Var *var = &((context->vars)[index]);
		if (var->used == false)
			continue;
		if (str_l_cmp(var->var_name, name, TPL_VAR_NAME_MAX) == 0)
			return index;
	} // end VARS_LOOP
	return -1;
}

char* tpl_get_var(TPL_Context *context, const char *name) {
	size_t index = tpl_find_var(context, name);
	if (index == -1)
		return NULL;
	TPL_Var *var = &((context->vars)[index]);
	return var->value;
}

size_t tpl_set_var(TPL_Context *context, const char *name, const char *value) {
	size_t index = tpl_find_var(context, name);
	TPL_Var *var;
	// new entry
	if (index == -1) {
		index = tpl_alloc_var(context);
		var = &((context->vars)[index]);
		var->used = true;
		str_l_cpy(var->var_name, name, TPL_VAR_NAME_MAX);
	// existing entry
	} else {
		var = &((context->vars)[index]);
		if (var->value != NULL)
			free(var->value);
	}
	var->value_size = str_len(value) + 1;
	var->value = str_l_dup(value, var->value_size);
	return index;
}
