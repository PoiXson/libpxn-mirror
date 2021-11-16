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

#include "StringUtils.h"
#include "FileUtils.h"
#include "Logs.h"



TPL_Doc* tpl_parse_file(char *file) {
	char *data;
	size_t data_size = load_text_file(file, &data);
	if (data_size < 0) return NULL;
	TPL_Doc *doc = calloc(1, sizeof(TPL_Doc));
	doc->file = str_dup(file);
	#if defined(DEBUG) && defined(DEBUG_TPL)
	{
		log_detail("Parsing file: %s", doc->file);
		size_t line_count = 0;
		for (size_t pos=0; pos<data_size; pos++) {
			if (data[pos] == '\n')
				line_count++;
		}
		if (data[data_size-1] != '\n')
			line_count++;
		log_detail(
			"Parsing [%lu] bytes, [%lu] line%s..",
			data_size,
			line_count,
			(line_count == 1 ? "" : "s")
		);
	}
	#endif
	size_t current_line = 1;
	size_t current_pos  = 0;
	doc->nodes_size =
		tpl_parse(
			doc, &(doc->nodes),
			data, data_size,
			&current_pos, &current_line
		);
	#if defined(DEBUG) && defined(DEBUG_TPL)
	{
		size_t  nodes_count = tpl_doc_nodes_count(doc);
		log_detail("Parsed [%lu] nodes",  nodes_count);
	}
	#endif
	return doc;
}

TPL_Doc* tpl_parse_data(char *data, size_t data_size) {
	TPL_Doc *doc = calloc(1, sizeof(TPL_Doc));
	#if defined(DEBUG) && defined(DEBUG_TPL)
	{
		size_t line_count = 0;
		for (size_t pos=0; pos<data_size; pos++) {
			if (data[pos] == '\n')
				line_count++;
		}
		if (data[data_size-1] != '\n')
			line_count++;
		log_line(
			"Parsing [%lu] bytes, [%lu] line%s..",
			data_size,
			line_count,
			(line_count == 1 ? "" : "s")
		);
	}
	#endif
	size_t current_line = 1;
	size_t current_pos  = 0;
	doc->nodes_size =
		tpl_parse(
			doc, &(doc->nodes),
			data, data_size,
			&current_pos, &current_line
		);
	#if defined(DEBUG) && defined(DEBUG_TPL)
	{
		size_t nodes_count = tpl_doc_nodes_count(doc);
		log_detail("Parsed [%lu] nodes",  nodes_count);
	}
	#endif
	return doc;
}

size_t tpl_parse(TPL_Doc *doc, TPL_Node **nodes,
char *data, size_t data_size,
size_t *current_pos, size_t *current_line) {
	if (*nodes != NULL) {
		log_fatal("nodes array already allocated");
		exit(1);
	}
	// parse data
	size_t nodes_size = 0;
	size_t from = 0;
	size_t pos = *current_pos;

	// LOOP_DATA:
	while (true) {
		pos = *current_pos;
		if (pos >= data_size)
			break;

		// {? else ?} block - special position
		if (pos >= 2
		&&  data[pos-1] == '{'
		&&  data[pos  ] == '?') {
			pos++;
			from = pos;
			while (pos < data_size) {
				if (data[pos] == '\0'
				||  data[pos] == '\r'
				||  data[pos] == '\n')
					break;
				// find ?}
				if (data[pos] == '?' && data[pos+1] == '}')
					break;
				pos++;
			}
			size_t len = (pos - from) + 1;
			char *stmt_str = calloc(len, sizeof(char));
			str_l_cpy(stmt_str, &(data[from]), len);
			char *stmt_str_trim = str_trim(stmt_str);

			// {? else ?}
			if (str_cmp("else", stmt_str_trim) == 0) {
				free(stmt_str);
				*current_pos = pos;
				// store tag
				size_t index = tpl_alloc_node(nodes, &nodes_size);
				TPL_Node *node = &((*nodes)[index]);
				node->used = true;
				node->type = NODE_TYPE_ELSE;
				node->line = *current_line;
				node->else_nodes = NULL;
				node->else_nodes_size = 0;
				// ensure previous node is if
				{
					TPL_Node *pre_node = &((*nodes)[index-1]);
					if (pre_node->type == NODE_TYPE_IF) {
						pre_node->if_has_else = true;
					} else {
						log_severe(
							"Previous node type expected to be IF, found [%i] at index: %lu",
							pre_node->type, index
						);
					}
				}
				#if defined(DEBUG) && defined(DEBUG_TPL)
				log_line("  <Else>");
				#endif
				*current_pos = pos + 2;
				node->else_nodes_size =
					tpl_parse(
						doc, &(node->else_nodes),
						data, data_size,
						current_pos, current_line
					);
				continue;
			}

			// {? end ?}
			if (str_cmp("end",  stmt_str_trim) == 0) {
				free(stmt_str);
				#if defined(DEBUG) && defined(DEBUG_TPL)
				log_line("  <EndIf>");
				#endif
				*current_pos = pos + 2;
				continue;
			}

			free(stmt_str);
		}

		// parse tag {...}
		if (data[pos] == '{') {

			// {* comment tag *}
			if (data[pos+1] == '*') {
				pos += 2;
				from = pos;
				while (pos < data_size) {
					if (data[pos] == '\0'
					||  data[pos] == '\r'
					||  data[pos] == '\n')
						break;
					// find *}
					if (data[pos] == '*' && data[pos+1] == '}')
						break;
					pos++;
				}
				// store comment
				size_t len = (pos - from) + 1;
				char *text = calloc(len, sizeof(char));
				str_l_cpy(text, &(data[from]), len);
				char *trimmed = str_trim(text);
				len = str_len(trimmed);
				size_t index = tpl_alloc_node(nodes, &nodes_size);
				TPL_Node *node = &((*nodes)[index]);
				node->used = true;
				node->type = NODE_TYPE_COMMENT;
				node->line = *current_line;
				node->text_size = len;
				node->text = str_l_dup(trimmed, len+1);
				free(text);
				#if defined(DEBUG) && defined(DEBUG_TPL)
				{
					char *sum = str_a_sum(node->text);
					log_line("  <Comment: %lu chars> %s", len, sum);
					free(sum);
				}
				#endif
				pos += 2;
				*current_pos = pos;
				continue;
			}

			// {{ var }}
			if (data[pos+1] == '{') {
				pos += 2;
				from = pos;
				while (pos < data_size) {
					if (data[pos] == '\0'
					||  data[pos] == '\r'
					||  data[pos] == '\n')
						break;
					// find }}
					if (data[pos] == '}' && data[pos+1] == '}')
						break;
					pos++;
				}
				// store var tag
				size_t len = (pos - from) + 1;
				char *text = calloc(len, sizeof(char));
				str_l_cpy(text, &(data[from]), len);
				char *trimmed = str_trim(text);
				len = str_len(trimmed);
				str_l_make_safe(trimmed, len+1);
				size_t index = tpl_alloc_node(nodes, &nodes_size);
				TPL_Node *node = &((*nodes)[index]);
				node->used = true;
				node->type = NODE_TYPE_GET_VAR;
				node->line = *current_line;
				str_l_cpy(node->var_name, trimmed, TPL_VAR_NAME_MAX);
				free(text);
				#if defined(DEBUG) && defined(DEBUG_TPL)
				{
					char *sum = str_a_sum(node->var_name);
					log_line("  <Var: %lu chars> %s", len, sum);
					free(sum);
				}
				#endif
				pos += 2;
				*current_pos = pos;
				continue;
			} // end {{ tag }}

			// {? if/else/end ?}
			if (data[pos+1] == '?') {
				pos += 2;
				from = pos;
				while (pos < data_size) {
					if (data[pos] == '\0'
					||  data[pos] == '\r'
					||  data[pos] == '\n')
						break;
					// find ?}
					if (data[pos] == '?' && data[pos+1] == '}')
						break;
					pos++;
				}
				size_t len = (pos - from) + 1;
				char *cond_str = calloc(len, sizeof(char));
				str_l_cpy(cond_str, &(data[from]), len);
				char *cond_str_trim = str_trim(cond_str);

				// {? else ?}
				if (str_cmp("else", cond_str_trim) == 0) {
					free(cond_str);
					// reset position to tag
					*current_pos = from - 1;
					return nodes_size;
				}

				// {? end ?}
				if (str_cmp("end",  cond_str_trim) == 0) {
					free(cond_str);
					// reset position to tag
					*current_pos = from - 1;
					return nodes_size;
				}

				// store {? if ?} tag
				size_t index = tpl_alloc_node(nodes, &nodes_size);
				TPL_Node *node = &((*nodes)[index]);
				node->used = true;
				node->type = NODE_TYPE_IF;
				node->line = *current_line;
				node->if_nodes = NULL;
				node->if_nodes_size = 0;
//TODO
node->cond_type = COND_TYPE_BOOLVAR;
node->cond_boolvar = calloc(1, sizeof(TPL_Cond_BoolVar));
str_l_cpy(node->cond_boolvar->boolvar_name, cond_str_trim, 10);
				free(cond_str);
				#if defined(DEBUG) && defined(DEBUG_TPL)
				log_line("  <If>");
				#endif
				*current_pos = pos + 2;
				node->if_nodes_size =
					tpl_parse(
						doc, &(node->if_nodes),
						data, data_size,
						current_pos, current_line
					);
				continue;
			} // end {? if ?} tag

		} // end parse tag { ... }

		// text until next tag
		{
			from = pos;
			size_t line_number = *current_line;
			// TEXT_LOOP:
			while (pos < data_size) {
				if (data[pos] == '\0') break;
				if (data[pos] == '\r'
				||  data[pos] == '\n')
					(*current_line)++;
				// found a tag
				if (data[pos] == '{') {
					// {{ {* {%
					if (data[pos+1] == '{'
					||  data[pos+1] == '*'
					||  data[pos+1] == '%'
					||  data[pos+1] == '?')
						break;
				}
				pos++;
			}
			// store text
			if (pos > from) {
				size_t len = (pos - from);
				size_t index = tpl_alloc_node(nodes, &nodes_size);
				TPL_Node *node = &((*nodes)[index]);
				node->used = true;
				node->type = NODE_TYPE_TEXT;
				node->line = line_number;
				node->text_size = len;
				node->text = str_l_dup( &(data[from]), len+1 );
				node->text = calloc(len+1, sizeof(char));
				str_l_cpy( (*nodes)[index].text, &(data[from]), len+1 );
				#if defined(DEBUG) && defined(DEBUG_TPL)
				{
					char *sum = str_a_sum(node->text);
					log_line("  <Text: %lu chars> %s", len, sum);
					free(sum);
				}
				#endif
			}
			*current_pos = pos;
		} // end text

	} // end LOOP_DATA
	return nodes_size;
}
