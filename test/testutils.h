#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define RUNTEST(x) \
	printf("[ RUN ] %s -> %s\n", __FILE__, #x); \
	x();


#define TEST_DIE() exit(1)


#define _TEST_PRINT_ASSERT_FAILED() \
	printf("ASSERTION FAILED on %s:%d\n", __FILE__, __LINE__);



#define TEST_ASSERT_EQ_INT(expected,actual) \
	if (expected != actual) { \
		_TEST_PRINT_ASSERT_FAILED(); \
		printf("\tExpression : (%s == %s)\n", #expected, #actual); \
		printf("\tExpected   : %d\n", (int)expected); \
		printf("\tActual     : %d\n", (int)actual); \
		TEST_DIE(); \
	}

#define TEST_ASSERT_NEQ_INT(expected,actual) \
	if (expected == actual) { \
		_TEST_PRINT_ASSERT_FAILED(); \
		printf("\tExpression : (%s != %s)\n", #expected, #actual); \
		printf("\tExpected   : %d\n", (int)expected); \
		printf("\tActual     : %d\n", (int)actual); \
		TEST_DIE(); \
	}

#define TEST_ASSERT_EQ_STRN(expected, actual, length) \
	if (strncmp(expected, actual, length) != 0) {     \
		_TEST_PRINT_ASSERT_FAILED(); \
		printf("\tExpression : (strncmp(%s,%s,%s)\n", #expected, #actual, #length); \
		printf("\tExpected   : \"%.*s\"\n", (int)length, expected); \
		printf("\tActual     : \"%.*s\"\n", (int)length, actual); \
		TEST_DIE(); \
	}

#define TEST_ASSERT_EQ_STR(expected, actual) \
	if (strlen(expected) != strlen(actual)) { \
		_TEST_PRINT_ASSERT_FAILED(); \
		printf("\tExpression : (strcmp(%s,%s)\n", #expected, #actual);  \
		printf("\tExpected   : \"%s\" (len=%ld)\n", expected, strlen(expected)); \
		printf("\tActual     : \"%s\" (len=%ld)\n", actual, strlen(actual)); \
		TEST_DIE(); \
	} \
	TEST_ASSERT_EQ_STRN(expected, actual, strlen(expected)); \


#define TEST_ASSERT_EQ_BOOL(expected, actual) \
	if (expected != actual){     \
		_TEST_PRINT_ASSERT_FAILED(); \
		printf("\tExpression : (%s == %s)\n", #expected, #actual); \
		printf("\tExpected   : %s\n", (expected ? "true" : "false")); \
		printf("\tActual     : %s\n", (actual ? "true" : "false")); \
		TEST_DIE(); \
	}


#define TEST_ASSERT_MEMCMP(expected, actual_array, len) \
	for(size_t i=0; i < len; ++i) { \
		if (expected != actual_array[i]){     \
			_TEST_PRINT_ASSERT_FAILED(); \
			printf("\tExpression : (%s == %s[%zu])\n", #expected, #actual_array, i); \
			printf("\tExpected   : %d\n", (int)expected); \
			printf("\tActual     : %d\n", (int)actual_array[i]); \
			TEST_DIE(); \
		} \
	}

