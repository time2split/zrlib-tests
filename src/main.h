#include <stdio.h>
#include <minunit/minunit.h>
#include <zrlib/base/macro.h>

// ============================================================================

extern char MESSAGE_BUFF[];
extern char FUN_PREFIX[];

// ============================================================================

#define FUN_CMP_NAME \
	cmpResult
#define FUN_PRINT_NAME \
	printResult

#define ZRTEST_RESULT_6(prefix, msg_buffer, result, expected, fcmp, fprint) \
	testResult(prefix, msg_buffer, result, expected, fcmp, fprint)

#define ZRTEST_RESULT(prefix, msg_buffer, result, expected) \
	testResult(prefix, msg_buffer, result, expected, (int (*)(void*, void*))FUN_CMP_NAME, (void (*)(char*, void*))FUN_PRINT_NAME)

#define ZRTEST_BEGIN() \
	sprintf(FUN_PREFIX, "(in %s()) ", __FUNCTION__)

#define ZRTEST_BEGIN_MSG(MSG) \
	sprintf(FUN_PREFIX, "%s:\n%s\n", __FUNCTION__, MSG)

#define ZRTEST_PRINTF(MSG, ...) \
do{ \
	sprintf(FUN_PREFIX, "%s: " MSG, __FUNCTION__, __VA_ARGS__); \
}while(0)

#define ZRTEST_CATPRINTF(MSG, ...) ZRBLOCK( \
	sprintf(FUN_PREFIX, "%s: " MSG, FUN_PREFIX, __VA_ARGS__); \
)

#define ZRTEST_FAIL() ZRBLOCK(mu_fail(FUN_PREFIX);)

#define ZRTEST_CHECK(COND) \
do{ \
	if(!(COND)) { \
		strcat(FUN_PREFIX, " (" #COND ")\n"); \
		mu_fail(FUN_PREFIX); \
	} \
}while(0)

#define ZRTEST_FAIL_MSG(MSG) ZRBLOCK( \
	sprintf(FUN_PREFIX, "%s failed: %s!\n", FUN_PREFIX, MSG); \
	mu_fail(FUN_PREFIX); \
)



#define ZRTEST_ASSERT_TYPE_OP(E,R,TYPE,OP,PRINT) ZRTEST_ASSERT_TYPE_COND(E,R,TYPE, ZRCODE(_r OP _e), OP, PRINT)

#define ZRTEST_ASSERT_TYPE_COND(E,R,TYPE,COND,OP,PRINT) \
do{ \
	TYPE _e = (E); \
	TYPE _r = (R); \
	\
	if(!(COND)) { \
		sprintf(FUN_PREFIX, "%s" #E " " #OP " " #R "\n\tExpected " #OP " " PRINT " but have " PRINT "\n", FUN_PREFIX, _e, _r); \
		mu_fail(FUN_PREFIX); \
	} \
}while(0)

#define ZRTEST_ASSERT_DOUBLE_EQ(E,R) ZRTEST_ASSERT_TYPE_OP(E,R,double,==,"%lf")
#define ZRTEST_ASSERT_DOUBLE_NE(E,R) ZRTEST_ASSERT_TYPE_OP(E,R,double,!=,"%lf")
#define ZRTEST_ASSERT_DOUBLE_GE(E,R) ZRTEST_ASSERT_TYPE_OP(E,R,double,>=,"%lf")
#define ZRTEST_ASSERT_DOUBLE_LE(E,R) ZRTEST_ASSERT_TYPE_OP(E,R,double,<=,"%lf")

#define ZRTEST_ASSERT_INT_EQ(E,R) ZRTEST_ASSERT_TYPE_OP(E,R,int,==,"%d")
#define ZRTEST_ASSERT_INT_NE(E,R) ZRTEST_ASSERT_TYPE_OP(E,R,int,!=,"%d")
#define ZRTEST_ASSERT_INT_GE(E,R) ZRTEST_ASSERT_TYPE_OP(E,R,int,>=,"%d")
#define ZRTEST_ASSERT_INT_LE(E,R) ZRTEST_ASSERT_TYPE_OP(E,R,int,<=,"%d")

#define ZRTEST_ASSERT_INT_RANGE(A,B,R) do{ \
	ZRTEST_ASSERT_INT_GE(A,R); \
	ZRTEST_ASSERT_INT_LE(B,R); \
}while(0)

#define ZRTEST_ASSERT_CHAR_EQ(E,R) ZRTEST_ASSERT_TYPE_OP(E,R,char,==,"%c")
#define ZRTEST_ASSERT_CHAR_NE(E,R) ZRTEST_ASSERT_TYPE_OP(E,R,char,!=,"%c")

#define ZRTEST_ASSERT_PTR_EQ(E,R) ZRTEST_ASSERT_TYPE_OP(E,R,void*,==,"%p")
#define ZRTEST_ASSERT_PTR_NE(E,R) ZRTEST_ASSERT_TYPE_OP(E,R,void*,!=,"%p")

#define ZRTEST_ASSERT_STR_EQ(E,R) ZRTEST_ASSERT_TYPE_COND(E,R,char*,ZRCODE(strcmp(R,E) == 0), strcmp, "%s")
#define ZRTEST_ASSERT_STR_NE(E,R) ZRTEST_ASSERT_TYPE_COND(E,R,char*,ZRCODE(strcmp(R,E) != 0), strcmp, "%s")

#define ZRTEST_END(msg_buffer, result, expected) \
	ZRTEST_RESULT(FUN_PREFIX, msg_buffer, result, expected)

// ============================================================================

static void testResult(const char *prefix, char *MESSAGE_BUFF, void *result, void *expected, int (*cmpResult)(void*, void*), void (*print)(char*, void*))
{
	const int cmp = cmpResult(result, expected);

	if (cmp != 0)
	{
		char msg_expected[500] = "";
		char msg_have[500] = "";

		print(msg_expected, expected);
		print(msg_have, result);

		sprintf(MESSAGE_BUFF, "%s (cmp=%d) Expected\n%s but have\n%s\n", prefix, cmp, msg_expected, msg_have);
		fputs(MESSAGE_BUFF, stderr);
		fflush (stderr);
		mu_fail(MESSAGE_BUFF);
	}
}

static void mainTestSetup()
{
	*MESSAGE_BUFF = '\0';
}
