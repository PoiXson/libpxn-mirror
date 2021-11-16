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
#include "src/Testing.h"

#include "src/tpl.h"
#include "src/StringUtils.h"
#include "src/NumberUtils.h"



// {* comment *}
void assert_tpl_node_comment(const int src_line, const TPL_Node *node,
const size_t expected_line, const char *expected_str) {
	size_t line = src_line * pow_sizet(10, len_int(__LINE__)+2);
	assert_int_cmp  (line+__LINE__, NODE_TYPE_COMMENT,     node->type     );
	assert_sizet_cmp(line+__LINE__, expected_line,         node->line     );
	assert_sizet_cmp(line+__LINE__, str_len(expected_str), node->text_size);
	assert_str_cmp  (line+__LINE__, expected_str,          node->text     );
}
// text
void assert_tpl_node_text(const int src_line, const TPL_Node *node,
const size_t expected_line, const char *expected_str) {
	size_t line = src_line * pow_sizet(10, len_int(__LINE__)+2);
	assert_int_cmp  (line+__LINE__, NODE_TYPE_TEXT,        node->type     );
	assert_sizet_cmp(line+__LINE__, expected_line,         node->line     );
	assert_sizet_cmp(line+__LINE__, str_len(expected_str), node->text_size);
	assert_str_cmp  (line+__LINE__, expected_str,          node->text     );
}
// {{ var }}
void assert_tpl_node_get_var(const int src_line, const TPL_Node *node,
const size_t expected_line, const char *expected_var_name) {
	size_t line = src_line * pow_sizet(10, len_int(__LINE__)+2);
	assert_int_cmp  (line+__LINE__, NODE_TYPE_GET_VAR, node->type    );
	assert_sizet_cmp(line+__LINE__, expected_line,     node->line    );
	assert_str_cmp  (line+__LINE__, expected_var_name, node->var_name);
}
// {? if ?}
void assert_tpl_node_ifbv(const int src_line, const TPL_Node *node,
const size_t expected_line, const char *expected_cond, const size_t expect_nodes_size) {
	size_t line = src_line * pow_sizet(10, len_int(__LINE__)+2);
	assert_int_cmp  (line+__LINE__, NODE_TYPE_IF,      node->type         );
	assert_int_cmp  (line+__LINE__, COND_TYPE_BOOLVAR, node->cond_type    );
	assert_sizet_cmp(line+__LINE__, expected_line,     node->line         );
	assert_str_cmp  (line+__LINE__, expected_cond,     node->cond_boolvar->boolvar_name);
	assert_sizet_cmp(line+__LINE__, expect_nodes_size, node->if_nodes_size);
}
// {? else ?}
void assert_tpl_node_else(const int src_line, const TPL_Node *node,
const size_t expected_line, const size_t expect_nodes_size) {
	size_t line = src_line * pow_sizet(10, len_int(__LINE__)+2);
	assert_int_cmp  (line+__LINE__, NODE_TYPE_ELSE,    node->type           );
	assert_sizet_cmp(line+__LINE__, expected_line,     node->line           );
	assert_sizet_cmp(line+__LINE__, expect_nodes_size, node->else_nodes_size);
}



void test_tpl() {
	#if defined(DEBUG) && defined(DEBUG_TPL)
	log_nl();
	#endif

	// basic tags
	{
		TPL_Context *context = calloc(1, sizeof(TPL_Context));
			assert_not_null(__LINE__, context);
		// test vars
		assert_sizet_cmp(__LINE__, -1, tpl_find_var(context, "test_var"));
		assert_sizet_cmp(__LINE__,  0, tpl_set_var (context, "test_var", "something"));
		assert_sizet_cmp(__LINE__,  0, tpl_find_var(context, "test_var"));
		assert_str_cmp  (__LINE__, "something", tpl_get_var (context, "test_var"));
		char *data =
			"Abc{* comment *} {{test_var}}\n"
			"def {nothing}\n"
			"{? bool_test ?}\n"
			"	Is True\n"
			"{? else ?}\n"
			"	Is False\n"
			"{? end ?}";
		TPL_Doc *doc = tpl_parse_data(data, str_len(data));
			assert_not_null (__LINE__, doc);
			assert_sizet_cmp(__LINE__, 8, doc->nodes_size);
			assert_sizet_cmp(__LINE__, 7, tpl_nodes_count(doc->nodes, doc->nodes_size));
		// check nodes
		assert_tpl_node_text   (__LINE__, &(doc->nodes[0]), 1, "Abc"     );
		assert_tpl_node_comment(__LINE__, &(doc->nodes[1]), 1, "comment" );
		assert_tpl_node_text   (__LINE__, &(doc->nodes[2]), 1, " "       );
		assert_tpl_node_get_var(__LINE__, &(doc->nodes[3]), 1, "test_var");
		assert_tpl_node_text   (__LINE__, &(doc->nodes[4]), 1, "\ndef {nothing}\n");
		assert_tpl_node_ifbv(__LINE__, &(doc->nodes[5]), 3, "bool_test", 1);
		assert_tpl_node_text(__LINE__, &(doc->nodes[5].if_nodes[0]), 3, "\n\tIs True\n");
		assert_tpl_node_else(__LINE__, &(doc->nodes[6]), 5, 1);
		assert_tpl_node_text(__LINE__, &(doc->nodes[6].else_nodes[0]), 5, "\n\tIs False\n");
		// render
		char *result = tpl_render(doc, context);
		assert_str_cmp(__LINE__, "Abc something\ndef {nothing}\n\n\tIs True\n", result);
		free(result);
		tpl_free_doc(doc);
	}

	#if defined(DEBUG) && defined(DEBUG_TPL)
	log_nl();
	#endif
}
