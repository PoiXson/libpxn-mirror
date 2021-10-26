
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
