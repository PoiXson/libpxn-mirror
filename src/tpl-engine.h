#pragma once

#include <stdlib.h>



typedef enum TPL_Node_Type {
	type_comment  = 0x1,
	type_text     = 0x2,
	type_var      = 0x3,
	type_include  = 0x4,
	type_if       = 0x5,
	type_elseif   = 0x6,
	type_endif    = 0x7,
	type_loop     = 0x8,
	type_endloop  = 0x9,
	type_break    = 0xa,
	type_continue = 0xb
} TPL_Node_Type;



typedef struct TPL_Node {
	// node type
	TPL_Node_Type type;
	// node data
	size_t data_size;
	union {
		char *text;     // type_text
		char *var_name; // type_var
		char *expr;     // type_if/elseif/loop
	};
	// child nodes
	size_t nodes_size;
	struct TPL_Node *nodes;
} TPL_Node;



typedef struct TPL_Doc {
	// doc nodes
	TPL_Node *nodes;
	size_t nodes_size;
} TPL_Doc;



TPL_Doc* tpl_load_doc(char *file);

size_t tpl_new_node(TPL_Node **nodes, size_t *nodes_size);
void tpl_init_node(TPL_Node **nodes, size_t index);

size_t tpl_parse(TPL_Node **nodes, char *data);
char* tpl_render(const TPL_Doc *doc);
void tpl_render_nodes(char **result, size_t *result_size, TPL_Node *nodes, size_t nodes_size);
