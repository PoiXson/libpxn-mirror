
#include "src/Testing.h"

#include "src/NumberUtils.h"



TEST_INIT(test_NumberUtils)
void test_NumberUtils() {
	int len;
	// test len_int()
	len = len_int(123456789);
		assert_intcmp(9, len);
	len = len_int(-123456789);
		assert_intcmp(10, len);
	len = len_int(0);
		assert_intcmp(1, len);
	len = len_int(1);
		assert_intcmp(1, len);
	len = len_int(-1);
		assert_intcmp(2, len);
	len = len_int(999);
		assert_intcmp(3, len);
	len = len_int(-999);
		assert_intcmp(4, len);
	// test itostr()
	char *str;
	str = itostr(123456789);
		assert_strcmp("123456789", str); free(str);
	str = itostr(-123456789);
		assert_strcmp("-123456789", str); free(str);
	str = itostr(0);
		assert_strcmp("0", str); free(str);
	str = itostr(1);
		assert_strcmp("1", str); free(str);
	str = itostr(-1);
		assert_strcmp("-1", str); free(str);
	str = itostr(999);
		assert_strcmp("999", str); free(str);
	str = itostr(-999);
		assert_strcmp("-999", str); free(str);
}
