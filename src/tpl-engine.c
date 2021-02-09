#include "tpl-engine.h"

#include <string.h>

#include "../utils/files.h"
#include "../utils/strings.h"

#if __has_include("../utils/logs.h")
#include "../utils/logs.h"
#endif
#if __has_include("../api/web11api.h")
#include "../api/web11api.h"
#endif



//#define TPL_DETAIL_LOG



TPL_Doc* tpl_load_doc(char *file) {
	char *data;
	int len = load_text_file(file, &data);
	if (len < 0)
		return NULL;
	TPL_Doc *doc = calloc(1, sizeof(TPL_Doc));
	doc->nodes_size = tpl_parse(&(doc->nodes), data);
	return doc;
}



size_t tpl_new_node(TPL_Node **nodes, size_t *nodes_size) {
	// find space in existing array
	if ((*nodes)[(*nodes_size)-1].type == 0) {
		size_t index = (*nodes_size) - 1;
		while (true) {
			index--;
			if ((*nodes)[index].type != 0)
				return index + 1;
			if (index == 0)
				break;
		}
		return 0;
	}
	// expand array
	(*nodes_size) *= 2;
	*nodes = realloc(*nodes, sizeof(TPL_Node) * (*nodes_size));
	if (*nodes == NULL) {
		log_fatal("Failed to reallocate TPL Nodes array");
		exit(1);
	}
	for (size_t index=(*nodes_size)/2; index<(*nodes_size); index++) {
		tpl_init_node(nodes, index);
	}
	return tpl_new_node(nodes, nodes_size);
}

void tpl_init_node(TPL_Node **nodes, size_t index) {
	(*nodes)[index].type = 0;
	(*nodes)[index].data_size  = 0;
	(*nodes)[index].nodes_size = 0;
}



size_t tpl_parse(TPL_Node **nodes, char *data) {
	if (*nodes != NULL) {
		log_fatal("nodes array already allocated");
		exit(1);
	}
	// allocate nodes array
	size_t nodes_size = 2;
	*nodes = calloc(nodes_size, sizeof(TPL_Node));
	if (*nodes == NULL) {
		log_fatal("Failed to allocate TPL Nodes array");
		exit(1);
	}
	for (size_t index=0; index<nodes_size; index++) {
		tpl_init_node(nodes, index);
	}
	// parse data
	size_t data_len = strlen(data);
	size_t pos  = 0;
	size_t last = 0;
	//PARSE_LOOP:
#ifdef TPL_DETAIL_LOG
	log_detail("Parsing %lu bytes..", data_len);
#endif
	while (true) {
		if (pos >= data_len-1) break;
		if (data[pos] == '\0') break;

		// parse tag
		if (data[pos] == '{') {

			// {* comment tag *}
			if (data[pos+1] == '*') {
				pos += 2;
				last = pos;
				while (true) {
					if (pos >= data_len-1) break;
					if (data[pos] == '\0') break;
					if (data[pos] == '*' && data[pos+1] == '}') {
						// store comment
						// note: include empty comments
						//if (pos > last) {
							size_t index = tpl_new_node(nodes, &nodes_size);
							size_t len = (pos - last) + 1;
							(*nodes)[index].type = type_comment;
							(*nodes)[index].data_size = len;
							(*nodes)[index].text = calloc(len, sizeof(char));
							strlcpy( (*nodes)[index].text, &(data[last]), len );
#ifdef TPL_DETAIL_LOG
							log_detail("  <Comment: %lu bytes>", len);
#endif
						//}
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
					if (data[pos] == '}' && data[pos+1] == '}') {
						// store variable name
						if (pos > last) {
							size_t index = tpl_new_node(nodes, &nodes_size);
							size_t len = (pos - last) + 1;
							(*nodes)[index].type = type_var;
							(*nodes)[index].data_size = len;
							(*nodes)[index].var_name = calloc(len, sizeof(char));
							strlcpy( (*nodes)[index].var_name, &(data[last]), len );
#ifdef TPL_DETAIL_LOG
							log_detail("  <Var: %s>", (*nodes)[index].var_name);
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
			// include / if / elseif / endif / loop / endloop / break / continue
			if (data[pos+1] == '%') {
				pos += 2;
				last = pos;
				while (true) {
					if (pos >= data_len-1) break;
					if (data[pos] == '\0') break;
					if (data[pos] == '%' && data[pos+1] == '}') {
						// store the statement
						if (pos > last) {
							size_t index = tpl_new_node(nodes, &nodes_size);
							size_t len = (pos - last) + 1;
//TODO: find statement type
(*nodes)[index].type = type_loop;
							(*nodes)[index].data_size = len;
(*nodes)[index].expr = calloc(len, sizeof(char));
							strlcpy( (*nodes)[index].expr, &(data[last]), len );
#ifdef TPL_DETAIL_LOG
							log_detail("  <Statement: %s bytes>", (*nodes)[index].expr);
#endif
						}
						pos += 2;
						break;
					}
					pos++;
				}
				continue;
			}

		}

		// text until next tag
		{
			last = pos;
			while (true) {
				if (pos >= data_len-1) break;
				if (data[pos] == '\0') break;
				// found a tag
				if (data[pos] == '{') {
					if (data[pos+1] == '*' || data[pos+1] == '{' || data[pos+1] == '%') {
						// store text
						if (pos > last) {
							size_t index = tpl_new_node(nodes, &nodes_size);
							size_t len = (pos - last) + 1;
							(*nodes)[index].type = type_text;
							(*nodes)[index].data_size = len;
							(*nodes)[index].text = calloc(len, sizeof(char));
							strlcpy( (*nodes)[index].text, &(data[last]), len );
#ifdef TPL_DETAIL_LOG
							log_detail("  <Text: %lu bytes>", len);
#endif
						}
						break;
					}
				}
				pos++;
			}
		}

	} // end PARSE_LOOP
	// remaining text
	if (pos > last) {
		if (pos < data_len)
			pos++;
		size_t index = tpl_new_node(nodes, &nodes_size);
		size_t len = (pos - last) + 1;
		(*nodes)[index].type = type_text;
		(*nodes)[index].data_size = len;
		(*nodes)[index].text = calloc(len, sizeof(char));
		strlcpy( (*nodes)[index].text, &(data[last]), len );
#ifdef TPL_DETAIL_LOG
		log_detail("  <Text: %lu bytes>", len);
#endif
	}
	return nodes_size;
}

char* tpl_render(const TPL_Doc *doc) {
	size_t result_size = 200;
	char *result = calloc(result_size, sizeof(char));
	if (result == NULL) {
		log_fatal("Failed to allocate for rendered output");
		exit(1);
	}
	// render nodes
	tpl_render_nodes(&result, &result_size, doc->nodes, doc->nodes_size);
	return result;
}

void tpl_render_nodes(char **result, size_t *result_size, TPL_Node *nodes, size_t nodes_size) {
	// NODES_LOOP:
	for (size_t index=0; index<nodes_size; index++) {
		if (nodes[index].type == 0)
			continue;
		switch (nodes[index].type) {
		case type_comment:
			strlcat(*result, "/*", *result_size);
			strlcat(*result, nodes[index].text, *result_size);
			strlcat(*result, "*/", *result_size);
			break;
		case type_text:
			strlcat(*result, nodes[index].text, *result_size);
			break;
		case type_var:
			strlcat(*result, "<VAR:", *result_size);
			strlcat(*result, nodes[index].var_name, *result_size);
			strlcat(*result, ">", *result_size);
			break;
//TODO
		case type_include:
		case type_if:
		case type_elseif:
		case type_loop:
		case type_endloop:
		case type_break:
		case type_continue:
log_warning("Unfinished template node type! %i", nodes[index].type);
			break;
		default:
			log_severe("Unknown template node type: %i", nodes[index].type);
			break;
		}
	} // end NODES_LOOP
}
