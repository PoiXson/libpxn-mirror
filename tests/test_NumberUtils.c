
#include "src/Testing.h"

#include "src/NumberUtils.h"



TEST_INIT(test_NumberUtils);
void test_NumberUtils() {
	int len;
	// test len_int()
	len = len_int(123456789);
		assert_intcmp(__FILE__, __LINE__, 9, len);
	len = len_int(-123456789);
		assert_intcmp(__FILE__, __LINE__, 10, len);
	len = len_int(0);
		assert_intcmp(__FILE__, __LINE__, 1, len);
	len = len_int(1);
		assert_intcmp(__FILE__, __LINE__, 1, len);
	len = len_int(-1);
		assert_intcmp(__FILE__, __LINE__, 2, len);
	len = len_int(999);
		assert_intcmp(__FILE__, __LINE__, 3, len);
	len = len_int(-999);
		assert_intcmp(__FILE__, __LINE__, 4, len);
	// test itostr()
	char *str;
	str = itostr(123456789);
		assert_strcmp(__FILE__, __LINE__, "123456789", str); free(str);
	str = itostr(-123456789);
		assert_strcmp(__FILE__, __LINE__, "-123456789", str); free(str);
	str = itostr(0);
		assert_strcmp(__FILE__, __LINE__, "0", str); free(str);
	str = itostr(1);
		assert_strcmp(__FILE__, __LINE__, "1", str); free(str);
	str = itostr(-1);
		assert_strcmp(__FILE__, __LINE__, "-1", str); free(str);
	str = itostr(999);
		assert_strcmp(__FILE__, __LINE__, "999", str); free(str);
	str = itostr(-999);
		assert_strcmp(__FILE__, __LINE__, "-999", str); free(str);
}
