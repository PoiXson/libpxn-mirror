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



void test_tpl() {
	#if defined(DEBUG) && defined(DEBUG_TPL)
	log_nl();
	#endif

	// basic tags
	{
		TPL_Context *context = calloc(1, sizeof(TPL_Context));
			assert_not_null(__LINE__, context);
		// test vars
		assert_sizet_cmp(__LINE__, -1,    tpl_find_var(context, "test"));
		assert_sizet_cmp(__LINE__,  0,    tpl_set_var (context, "test", "something"));
		assert_str_cmp  (__LINE__, "something", tpl_get_var (context, "test"));
		char *data = "Abc{* comment *} {{test}}\ndef {a}";
		TPL_Doc *doc = tpl_parse_data(data, str_len(data));
			assert_not_null (__LINE__, doc);
			assert_sizet_cmp(__LINE__, 8, doc->nodes_size);
		// text node
		assert(__LINE__, (doc->nodes[0].type == NODE_TYPE_TEXT));
			assert_sizet_cmp(__LINE__, 1,     doc->nodes[0].line     );
			assert_sizet_cmp(__LINE__, 3,     doc->nodes[0].text_size);
			assert_str_cmp  (__LINE__, "Abc", doc->nodes[0].text);
		// comment node
		assert(__LINE__, (doc->nodes[1].type == NODE_TYPE_COMMENT));
			assert_sizet_cmp(__LINE__, 1, doc->nodes[1].line     );
			assert_sizet_cmp(__LINE__, 7, doc->nodes[1].text_size);
			assert_str_cmp  (__LINE__, "comment", doc->nodes[1].text);
		// text node
		assert(__LINE__, (doc->nodes[2].type == NODE_TYPE_TEXT));
			assert_sizet_cmp(__LINE__, 1,   doc->nodes[2].line     );
			assert_sizet_cmp(__LINE__, 1,   doc->nodes[2].text_size);
			assert_str_cmp  (__LINE__, " ", doc->nodes[2].text);
		// var tag
		assert(__LINE__, (doc->nodes[3].type == NODE_TYPE_GET_VAR));
			assert_sizet_cmp(__LINE__, 1,      doc->nodes[3].line    );
			assert_str_cmp  (__LINE__, "test", doc->nodes[3].var_name);
		// text node
		assert(__LINE__, (doc->nodes[4].type == NODE_TYPE_TEXT));
			assert_sizet_cmp(__LINE__, 1, doc->nodes[4].line     );
			assert_sizet_cmp(__LINE__, 8, doc->nodes[4].text_size);
			assert_str_cmp  (__LINE__, "\ndef {a}", doc->nodes[4].text);
		#if defined(DEBUG) && defined(DEBUG_TPL)
		tpl_dump_vars(context);
		#endif
		// render
		char *result = tpl_render(doc, context);
		assert_str_cmp(__LINE__, "Abc something\ndef {a}", result);
		free(result);
		tpl_free_doc(doc);
	}

	#if defined(DEBUG) && defined(DEBUG_TPL)
	log_nl();
	#endif
}
