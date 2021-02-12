#pragma once

#include <stdlib.h>
#include <stdbool.h>



typedef enum TPL_Node_Type {
	NODE_TYPE_UNUSED   = 0x0,
	NODE_TYPE_COMMENT  = 0x1,
	NODE_TYPE_TEXT     = 0x2,
	NODE_TYPE_VAR      = 0x3,
	NODE_TYPE_SET_VAR  = 0x4
//TODO
//	NODE_TYPE_INCLUDE  = 0x,
//	NODE_TYPE_IF       = 0x,
//	NODE_TYPE_ELSE     = 0x,
//	NODE_TYPE_ELSEIF   = 0x,
//	NODE_TYPE_ENDIF    = 0x,
//	NODE_TYPE_LOOP     = 0x,
//	NODE_TYPE_ENDLOOP  = 0x,
//	NODE_TYPE_CONTINUE = 0x,
//	NODE_TYPE_BREAK    = 0x
} TPL_Node_Type;



typedef struct TPL_Var {
	size_t name_size;
	size_t value_size;
	char *name;
	char *value;
} TPL_Var;

typedef struct TPL_Node {
	// node type
	TPL_Node_Type type;

	size_t nodes_size;
	struct TPL_Node *nodes;

	TPL_Var var;
} TPL_Node;



typedef struct TPL_Doc {
	bool render_comments;
	size_t nodes_size;
	TPL_Node *nodes;
	size_t vars_size;
	TPL_Var *vars;
} TPL_Doc;



TPL_Doc* tpl_doc_load(const char *file);
TPL_Doc* tpl_doc_parse(char *data);

size_t tpl_parse(TPL_Doc *doc, TPL_Node **nodes, char *data);

size_t tpl_node_allocate(TPL_Node **nodes, size_t *nodes_size);
size_t tpl_var_allocate(TPL_Doc *doc);

void tpl_doc_init(TPL_Doc *doc);
void tpl_node_init(TPL_Node *node);
void tpl_var_init(TPL_Var *var);

char* tpl_get_var(TPL_Doc *doc, char *name);
void  tpl_set_var(TPL_Doc *doc, char *name, char* value);
void  tpl_dump_vars(TPL_Doc *doc);

size_t tpl_render(char **result, TPL_Doc *doc);
void   tpl_render_nodes(TPL_Doc *doc, char **result, size_t *result_size, TPL_Node *nodes, size_t nodes_size);
