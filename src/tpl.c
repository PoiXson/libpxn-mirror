#include "tpl.h"

#include "StringUtils.h"
#include "NumberUtils.h"
#include "FileUtils.h"
#include "Logs.h"

#include <stdlib.h>
#include <string.h>


//#define TPL_DETAIL_LOG



TPL_Doc* tpl_doc_load(const char *file) {
	char *data;
	int len = load_text_file(file, &data);
	if (len < 0)
		return NULL;
	return tpl_doc_parse(data);
}

TPL_Doc* tpl_doc_parse(char *data) {
	TPL_Doc *doc = calloc(1, sizeof(TPL_Doc));
	tpl_doc_init(doc);
	doc->nodes_size = tpl_parse(doc, &(doc->nodes), data);
	return doc;
}



size_t tpl_parse(TPL_Doc *doc, TPL_Node **nodes, char *data) {
	if (*nodes != NULL) {
		log_fatal("nodes array already allocated");
		exit(1);
	}
	size_t data_len = strlen(data);
	#if defined(DEBUG) && defined(TPL_DETAIL_LOG)
	size_t line_count = 0;
	for (size_t index=0; index<data_len; index++) {
		if (data[index] == '\n')
			line_count++;
	}
	if (data[data_len-1] != '\n')
		line_count++;
	log_line("");
	log_line("Parsing [%lu] bytes, [%lu] lines..", data_len, line_count);
	#endif
	// parse data
	size_t pos  = 0;
	size_t last = 0;
	size_t nodes_size = 0;
	size_t nodes_count = 0;
	//PARSE_LOOP:
	while (true) {
		if (pos >= data_len-1) break;
		if (data[pos] == '\0') break;

		// parse tag {...}
		if (data[pos] == '{') {

			// {* comment tag *}
			if (data[pos+1] == '*') {
				pos += 2;
				last = pos;
				while (true) {
					if (pos >= data_len-1) break;
					if (data[pos] == '\0') break;
					// *}
					if (data[pos] == '*' && data[pos+1] == '}') {
						// store comment
						size_t index = tpl_node_allocate(nodes, &nodes_size);
						nodes_count++;
						size_t len = (pos - last) + 1;
						(*nodes)[index].type = NODE_TYPE_COMMENT;
						char *str_o = calloc(len, sizeof(char));
						strlcpy(str_o, &(data[last]), len);
						char *str_t = str_trim(str_o);
						len = strlen(str_t) + 1;
						(*nodes)[index].var.value_size = len;
						(*nodes)[index].var.value = calloc(len, sizeof(char));
						strlcpy((*nodes)[index].var.value, str_t, len);
						free(str_o);
						#if defined(DEBUG) && defined(TPL_DETAIL_LOG)
						log_line("  <Comment: %lu bytes> %.20s", len, (*nodes)[index].var.value);
						#endif
						pos += 2;
						break;
					}
					pos++;
				}
				continue;
			}

			// {{ variable|tag }}
			if (data[pos+1] == '{') {
				pos += 2;
				last = pos;
				while (true) {
					if (pos >= data_len-1) break;
					if (data[pos] == '\0') break;
					// }}
					if (data[pos] == '}' && data[pos+1] == '}') {
						// store variable name
						if (pos > last) {
							size_t index = tpl_node_allocate(nodes, &nodes_size);
							nodes_count++;
							size_t len = (pos - last) + 1;
							(*nodes)[index].type = NODE_TYPE_VAR;
							char *str_o = calloc(len, sizeof(char));
							strlcpy(str_o, &(data[last]), len);
							char *str_t = str_trim(str_o);
							len = strlen(str_t) + 1;
							(*nodes)[index].var.name_size = len;
							(*nodes)[index].var.name = calloc(len, sizeof(char));
							strlcpy( (*nodes)[index].var.name, str_t, len );
							free(str_o);
							#if defined(DEBUG) && defined(TPL_DETAIL_LOG)
							log_line("  <Var: %s>", (*nodes)[index].var.name);
							#endif
						}
						pos += 2;
						break;
					}
					pos++;
				}
				continue;
			}

			// {% statement tag %}
			// include / set / if / loop ...
			if (data[pos+1] == '%') {
				pos += 2;
				last = pos;
				while (true) {
					if (pos >= data_len-1) break;
					if (data[pos] == '\0') break;
					// %}
					if (data[pos] == '%' && data[pos+1] == '}') {

						// {% set variable = .. %}
						size_t content_size = (pos - last) + 1;
						char content[content_size];
						strlcpy(content, &(data[last]), content_size);
						char *content_t = str_trim(content);
						if (str_starts_with(content_t, "set ")) {
							content_t += 4;
							size_t pos = chrpos(content_t, '=');
							// var = ""
							if (pos == -1) {
								log_warning("Unknown variable value: %s", content);
//TODO: is this right?
								pos += 2;
								break;
							}
							content_t[pos] = '\0';
							char *var_name  = content_t;
							char *var_value = content_t + pos + 1;
							var_name  = str_trim(var_name);
							var_value = str_trim(var_value);
							size_t val_len = strlen(var_value);
							if ( (var_value[0] == '"' && var_value[val_len-1] == '"') ||
							(var_value[0] == '\'' && var_value[val_len-1] == '\'') ) {
								var_value[val_len-1] = '\0';
								var_value++;
							}
							// store set variable
							size_t index = tpl_node_allocate(nodes, &nodes_size);
							nodes_count++;
							(*nodes)[index].type = NODE_TYPE_SET_VAR;
							size_t len_name  = strlen(var_name)+1;
							size_t len_value = strlen(var_value)+1;
							(*nodes)[index].var.name_size  = len_name;
							(*nodes)[index].var.value_size = len_value;
							(*nodes)[index].var.name  = calloc(len_name,  sizeof(char));
							(*nodes)[index].var.value = calloc(len_value, sizeof(char));
							strlcpy( (*nodes)[index].var.name,  var_name,  len_name);
							strlcpy( (*nodes)[index].var.value, var_value, len_value);
							#if defined(DEBUG) && defined(TPL_DETAIL_LOG)
							log_line("  <Set: %s = %s>", (*nodes)[index].var.name, (*nodes)[index].var.value);
							#endif
						}

//						// store the statement
//						if (pos > last) {
//							size_t index = tpl_node_allocate(nodes, &nodes_size);
//							size_t len = (pos - last) + 1;
//TODO: find statement type
//(*nodes)[index].type = NODE_TYPE_LOOP;
//							(*nodes)[index].data_size = len;
//(*nodes)[index].expr = calloc(len, sizeof(char));
//							strlcpy( (*nodes)[index].expr, &(data[last]), len );
//						}
						pos += 2;
						break;
					}
					pos++;
				}
				continue;
			}

		} // end parse tag {...}

		// text until next tag
		{
			last = pos;
			// TEXT_LOOP:
			while (true) {
				if (pos >= data_len-1) break;
				if (data[pos] == '\0') break;
				// found a tag
				if (data[pos] == '{') {
					// {{ {* {%
					if (data[pos+1] == '{' || data[pos+1] == '*' || data[pos+1] == '%') {
						// store text
						if (pos > last) {
							size_t index = tpl_node_allocate(nodes, &nodes_size);
							nodes_count++;
							size_t len = (pos - last) + 1;
							(*nodes)[index].type = NODE_TYPE_TEXT;
							(*nodes)[index].var.value_size = len;
							(*nodes)[index].var.value = calloc(len, sizeof(char));
							strlcpy( (*nodes)[index].var.value, &(data[last]), len );
							#if defined(DEBUG) && defined(TPL_DETAIL_LOG)
							log_line("  <Text: %lu bytes>", len);
							#endif
						}
						break;
					}
				} // end found '{'
				pos++;
			} // end TEXT_LOOP
		} // end text

	} // end PARSE_LOOP
	// remaining text
	if (pos > last) {
		if (pos < data_len)
			pos++;
		size_t index = tpl_node_allocate(nodes, &nodes_size);
		nodes_count++;
		size_t len = (pos - last) + 1;
		(*nodes)[index].type = NODE_TYPE_TEXT;
		(*nodes)[index].var.value_size = len;
		(*nodes)[index].var.value = calloc(len, sizeof(char));
		strlcpy( (*nodes)[index].var.value, &(data[last]), len );
		#if defined(DEBUG) && defined(TPL_DETAIL_LOG)
		log_line("  <Text: %lu bytes>", len);
		#endif
	}
	#if defined(DEBUG) && defined(TPL_DETAIL_LOG)
	log_line("Parsed [%lu] nodes", nodes_count);
	log_line("");
	#endif
	return nodes_count;
}



size_t tpl_node_allocate(TPL_Node **nodes, size_t *nodes_size) {
	// allocate first block
	if (*nodes_size == 0) {
		*nodes_size = 1;
		*nodes = malloc((*nodes_size) * sizeof(TPL_Node));
		if (*nodes == NULL) {
			log_fatal("Failed to allocate nodes array");
			exit(1);
		}
		for (size_t index=0; index<*nodes_size; index++) {
			tpl_node_init( &((*nodes)[index]) );
		}
		return 0;
	}
	// find space in existing array
	for (size_t index=0; index<*nodes_size; index++) {
		if ( ((*nodes)[index]).type == NODE_TYPE_UNUSED ) {
			tpl_node_init( &((*nodes)[index]) );
			return index;
		}
	}
	// expand array
	*nodes = realloc(*nodes, sizeof(TPL_Node) * (*nodes_size) * 2);
	if (*nodes == NULL) {
		log_fatal("Failed to reallocate nodes array");
		exit(1);
	}
	for (size_t index=*nodes_size; index<(*nodes_size)*2; index++) {
		tpl_node_init( &((*nodes)[index]) );
	}
	(*nodes_size) *= 2;
	return tpl_node_allocate(nodes, nodes_size);
}

size_t tpl_var_allocate(TPL_Doc *doc) {
	// first var
	if (doc->vars_size == 0) {
		doc->vars_size = 1;
		doc->vars = malloc(doc->vars_size * sizeof(TPL_Var));
		if (doc->vars == NULL) {
			log_fatal("Failed to allocate tpl document vars array");
			exit(1);
		}
		for (size_t index=0; index<doc->vars_size; index++) {
			tpl_var_init( &(doc->vars[index]) );
		}
		return 0;
	}
	// find space in existing array
	for (size_t index=0; index<doc->vars_size; index++) {
		if (doc->vars[index].name_size == 0) {
			tpl_var_init( &(doc->vars[index]) );
			return index;
		}
	}
	// expand array
	doc->vars = realloc(doc->vars, sizeof(TPL_Var) * doc->vars_size * 2);
	if (doc->vars == NULL) {
		log_fatal("Failed to reallocate tpl document vars array");
		exit(1);
	}
	for (size_t index=doc->vars_size; index<doc->vars_size*2; index++) {
		tpl_var_init( &(doc->vars[index]) );
	}
	doc->vars_size *= 2;
	return tpl_var_allocate(doc);
}



void tpl_doc_init(TPL_Doc *doc) {
	doc->render_comments = false;
	doc->nodes_size = 0;
	doc->vars_size  = 0;
}

void tpl_node_init(TPL_Node *node) {
	node->type = NODE_TYPE_UNUSED;
	node->nodes_size = 0;
	node->var.name_size  = 0;
	node->var.value_size = 0;
}

void tpl_var_init(TPL_Var *var) {
	var->name_size  = 0;
	var->value_size = 0;
}



char* tpl_get_var(TPL_Doc *doc, char *name) {
	for (size_t index=0; index<doc->vars_size; index++) {
		if (doc->vars[index].name_size == 0)
			continue;
		if (strlcmp(doc->vars[index].name, name, doc->vars[index].name_size) == 0)
			return doc->vars[index].value;
	}
	return NULL;
}

void tpl_set_var(TPL_Doc *doc, char *name, char* value) {
	// find existing variable
	for (size_t index=0; index<doc->vars_size; index++) {
		if (doc->vars[index].name_size == 0)
			continue;
		if (strlcmp(doc->vars[index].name, name, doc->vars[index].name_size) == 0) {
			strrcpy( &(doc->vars[index].value), &(doc->vars[index].value_size), value );
			#if defined(DEBUG) && defined(TPL_DETAIL_LOG)
			log_line("  Changed variable: [%lu] %s = `%s`", index, name, value);
			#endif
			return;
		}
	}
	// add new variable
	{
		size_t index = tpl_var_allocate(doc);
		doc->vars[index].name_size  = strlen(name)  + 1;
		doc->vars[index].value_size = strlen(value) + 1;
		doc->vars[index].name  = calloc(doc->vars[index].name_size,  sizeof(char));
		doc->vars[index].value = calloc(doc->vars[index].value_size, sizeof(char));
		strlcpy(doc->vars[index].name,  name,  doc->vars[index].name_size);
		strlcpy(doc->vars[index].value, value, doc->vars[index].value_size);
		#if defined(DEBUG) && defined(TPL_DETAIL_LOG)
		log_line("  Set variable: [%lu] %s = `%s`", index, name, value);
		#endif
	}
}

void tpl_dump_vars(TPL_Doc *doc) {
	size_t count = 0;
	for (size_t index=0; index<doc->vars_size; index++) {
		count++;
	}
	log_line("Dumping [%lu] Var%s:", count, (count == 1 ? "" : "s"));
	for (size_t index=0; index<doc->vars_size; index++) {
		log_line("  %s = `%s`", doc->vars[index].name, doc->vars[index].value);
	}
}

void tpl_dump_doc(TPL_Doc *doc) {
	tpl_dump_nodes(0, doc->nodes, doc->nodes_size);
}

void tpl_dump_nodes(size_t indent, TPL_Node *nodes, size_t nodes_size) {
	log_line("Dumping [%lu] Node%s:", nodes_size, (nodes_size == 1 ? "" : "s"));
	// NODES_LOOP:
	for (size_t index=0; index<nodes_size; index++) {
		if (nodes[index].type == NODE_TYPE_UNUSED)
			continue;
		switch (nodes[index].type) {
		// {* comment *}
		case NODE_TYPE_COMMENT:
			log_line(" %lu)COMMENT: %s", index, nodes[index].var.value);
			break;
		// text
		case NODE_TYPE_TEXT:
			log_line(" %lu)TEXT: %s", index, nodes[index].var.value);
			break;
		// {{ variable }}
		case NODE_TYPE_VAR:
			log_line(" %lu)VAR: %s", index, nodes[index].var.name);
			break;
		// {% set var=value %}
		case NODE_TYPE_SET_VAR:
			log_line(" %lu)SET: %s = %s", index, nodes[index].var.name, nodes[index].var.value);
			break;
		// unknown
		default:
			log_severe("Unknown template node type: %i", nodes[index].type);
			break;
		}
	} // end NODES_LOOP
}



size_t tpl_render(char **result, TPL_Doc *doc) {
	// allocate first block
	size_t result_size = 100;
	*result = calloc(result_size, sizeof(char));
	if (*result == NULL) {
		log_fatal("Failed to allocate for rendered output");
		exit(1);
	}
	// render nodes
	#if defined(DEBUG) && defined(TPL_DETAIL_LOG)
	log_line("Rendering Doc..");
	#endif
	tpl_render_nodes(doc, result, &result_size, doc->nodes, doc->nodes_size);
	return MIN(result_size, strlen(*result));
}

void tpl_render_nodes(TPL_Doc *doc, char **result, size_t *result_size, TPL_Node *nodes, size_t nodes_size) {
	#if defined(DEBUG) && defined(TPL_DETAIL_LOG)
	log_line("Rendering Nodes..");
	#endif
	// NODES_LOOP:
	for (size_t index=0; index<nodes_size; index++) {
		if (nodes[index].type == NODE_TYPE_UNUSED)
			continue;
		switch (nodes[index].type) {
		// {* comment *}
		case NODE_TYPE_COMMENT:
			if (doc->render_comments) {
				strrcat(result, result_size, "/* ");
				strrcat(result, result_size, nodes[index].var.value);
				strrcat(result, result_size, " */");
			}
			break;
		// text
		case NODE_TYPE_TEXT:
			strrcat(result, result_size, nodes[index].var.value);
			break;
		// {{ variable }}
		case NODE_TYPE_VAR: {
			char *value = tpl_get_var(doc, nodes[index].var.name);
			if (value == NULL) {
				log_warning("Rendering Node [%lu] Unknown variable: %s", index, nodes[index].var.name);
				break;
			}
			strrcat(result, result_size, value);
			break;
		}
		// {% set var=value %}
		case NODE_TYPE_SET_VAR: {
			if (nodes[index].var.name_size == 0) {
				log_warning("Rendering Node [%lu] Unknown variable name", index);
				break;
			}
			if (nodes[index].var.value_size == 0) {
				log_warning("Rendering Node [%lu] Unknown variable value for: %s", index, nodes[index].var.name);
				break;
			}
			tpl_set_var(doc, nodes[index].var.name, nodes[index].var.value);
			break;
		}
		// unknown
		default:
			log_severe("Unknown template node type: %i", nodes[index].type);
			break;
		}
	} // end NODES_LOOP
}
