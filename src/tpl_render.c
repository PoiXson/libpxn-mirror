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
#include "Logs.h"



char* tpl_render(TPL_Doc *doc, TPL_Context *context) {
	#if defined(DEBUG) && defined(DEBUG_TPL)
	log_line("Rendering Doc..");
	#endif
	size_t out_size = 32;
	char *out = calloc(out_size, sizeof(char));
	if (out == NULL) {
		log_fatal("Failed to allocate for rendered output");
		exit(1);
	}
	tpl_render_nodes(
		doc, context,
		&out, &out_size,
		doc->nodes, doc->nodes_size
	);
	return out;
}

void tpl_render_nodes(TPL_Doc *doc, TPL_Context *context,
char **out, size_t *out_size, TPL_Node *nodes, size_t nodes_size) {
	#if defined(DEBUG) && defined(DEBUG_TPL)
		log_line("Rendering Nodes..");
	#endif
	// NODES_LOOP:
	for (size_t index=0; index<nodes_size; index++) {
		TPL_Node *node = &(nodes[index]);
		if (node->used == false)
			continue;
		switch (node->type) {

			// {* comment *}
			case NODE_TYPE_COMMENT:
				if (context->comments) {
					#if defined(DEBUG) && defined(DEBUG_TPL)
					{
						char *sum = str_a_sum(node->text);
						log_line("  <Comment: %lu chars> %s", node->text_size, sum);
						free(sum);
					}
					#endif
					str_la_cat(out, out_size, "/* ", 3);
					str_la_cat(out, out_size, node->text, node->text_size);
					str_la_cat(out, out_size, " */", 3);
				}
				continue;

			// text
			case NODE_TYPE_TEXT:
				str_la_cat(out, out_size, node->text, node->text_size);
				continue;

			// {{ var }}
			case NODE_TYPE_GET_VAR: {
				char *value = tpl_get_var(context, node->var_name);
				if (str_not_empty(value)) {
					str_la_cat(out, out_size, value, str_len(value));
				}
				continue;
			}

			// {? if ?}
			case NODE_TYPE_IF: {
				
				
				tpl_render_nodes(
					doc, context,
					out, out_size,
					node->if_nodes, node->if_nodes_size
				);
				
				
				// skip else block
				if (node->if_has_else) {
					TPL_Node *next_node = &(nodes[index+1]);
					if (next_node->type == NODE_TYPE_ELSE) {
						index++;
					}
				}
				continue;
			}

			case NODE_TYPE_ELSE:
				str_la_cat(out, out_size, " -=Unexpected `else` node=- ", 28);
				continue;

			default:
				log_severe("Unknown template node type: %i", node->type);
				continue;
		} // end type switch
	} // end NODES_LOOP
}



void tpl_dump_doc(const TPL_Doc *doc) {
	if (doc == NULL) {
		log_line("Doc:");
		return;
	}
	log_line("Doc: %s", (doc->file == NULL ? "null" : doc->file));
	int indent = 0;
	tpl_dump_nodes(&indent, doc->nodes, doc->nodes_size);
}

void tpl_dump_nodes(int *indent, const TPL_Node *nodes, const size_t nodes_size) {
	log_line("%*s[Nodes: %lu]", (*indent)*4, " ", tpl_nodes_count(nodes, nodes_size));
	if (nodes_size > 0) {
		for (size_t index=0; index<nodes_size; index++) {
			tpl_dump_node(indent, &(nodes[index]));
		}
	}
}

void tpl_dump_node(int *indent, const TPL_Node *node) {
	if (node == NULL) {
		log_line("Node: NULL");
		return;
	}
	if (node->used == false)
		return;
	switch (node->type) {
	case NODE_TYPE_COMMENT: {
		char *sum = str_a_sum(node->text);
		log_line("%*s%lu) COM: `%s`", (*indent)*4, " ", node->line, sum);
		free(sum);
		break;
	}
	case NODE_TYPE_TEXT: {
		char *sum = str_la_unescape(node->text, str_len(node->text));
		log_line("%*s%lu) TXT: `%s`", (*indent)*4, " ", node->line, sum);
		free(sum);
		break;
	}
	case NODE_TYPE_GET_VAR:
		log_line("%*s%lu) VAR: %s", (*indent)*4, " ", node->line, node->var_name);
		break;
	case NODE_TYPE_IF: {
		switch (node->cond_type) {
			case COND_TYPE_TRUE:
				log_line("%*s%lu) IFb: true",  (*indent)*4, " ", node->line);
				break;
			case COND_TYPE_FALSE:
				log_line("%*s%lu) IFb: false", (*indent)*4, " ", node->line);
				break;
			case COND_TYPE_BOOLVAR:
				log_line("%*s%lu) IFb: %s",    (*indent)*4, " ", node->line, node->cond_boolvar->boolvar_name);
				break;
			default:
				log_line("%*s%lu) IF: unknown condition: %i", (*indent)*4, " ", node->line, node->cond_type);
				break;
		}
		(*indent)++;
		tpl_dump_nodes(indent, node->if_nodes, node->if_nodes_size);
		(*indent)--;
		break;
	}
	case NODE_TYPE_ELSE: {
		log_line("%*s%lu) ELSE", (*indent)*4, " ", node->line);
		(*indent)++;
		tpl_dump_nodes(indent, node->else_nodes, node->else_nodes_size);
		(*indent)--;
		break;
	}
	default:
		log_line("%*s%lu) Unknown Node", (*indent)*4, " ", node->line);
		break;
	}
}

void tpl_dump_vars(const TPL_Context *context) {
	size_t count = 0;
	for (size_t index=0; index<context->vars_size; index++) {
		count++;
	}
	log_line("Dumping [%lu] Var%s:", count, (count == 1 ? "" : "s"));
	for (size_t index=0; index<context->vars_size; index++) {
		log_line(
			"  `%s` = `%s`",
			(context->vars)[index].var_name,
			(context->vars)[index].value
		);
	}
}
