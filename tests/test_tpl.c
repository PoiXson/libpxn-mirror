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



TEST_INIT(test_tpl_engine)
void test_tpl_engine() {
	char *data =
		"This is a te{*  first comment  *}st\n"
		"{%  set  letters = \"cd\"  %}"
		"{*  second comment  *}\n"
		"Ab{{   letters   }}efg\n";
	char *result =
		"This is a te/* first comment */st\n"
		"/* second comment */\n"
		"Abcdefg\n";
	// parse the template
	TPL_Doc *doc = tpl_doc_parse(data);
	// render the template
	char *out;
	doc->render_comments = true;
	tpl_render(&out, doc);
	assert_strcmp(result, out);
}
