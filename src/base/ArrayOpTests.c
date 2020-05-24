#include <string.h>
#include <minunit/minunit.h>

#include <zrlib/base/ArrayOp.h>
#include "../main.h"

// ============================================================================

typedef struct
{
	int i;
	char c;
} TYPE;

static TYPE MEM_REF[] =
	{
		{ 1, 'a' },
		{ 2, 'b' },
		{ 3, 'c' },
		{ 4, 'd' },
		{ 5, 'e' } };

#define NBBYTES sizeof(MEM_REF)
#define NBITEMS NBBYTES / sizeof(TYPE)

static TYPE MEM[NBITEMS];

extern char MESSAGE_BUFF[];

#define LOCAL_ZRTEST_RESULT(prefix) \
	ZRTEST_RESULT(prefix, MESSAGE_BUFF, MEM, result)

#define LOCAL_ZRTEST_END() \
	ZRTEST_END(MESSAGE_BUFF, MEM, result)

// ============================================================================

static void testSetup()
{
	mainTestSetup();
	memcpy(MEM, MEM_REF, NBBYTES);
}

static int FUN_CMP_NAME(TYPE result[NBITEMS], TYPE expected[NBITEMS])
{
	int cmp;

	for (int i = 0; i < NBITEMS; i++)
	{
		cmp = result[i].i - expected[i].i;

		if (cmp)
			return cmp;

		cmp = result[i].c - expected[i].c;

		if (cmp)
			return cmp;
	}
	return 0;
}

static void FUN_PRINT_NAME(char *out, TYPE tab[NBITEMS])
{
	char buff[20];
	strcat(out, "[");

	for (int i = 0; i < NBITEMS; i++)
	{
		sprintf(buff, "%s", (i ? "|" : ""));
		strcat(out, buff);

		for (int j = 0; j < sizeof(TYPE); j++)
		{
			sprintf(buff, "%s%3d", (j ? "," : ""), ((char*)&tab[i])[j]);
			strcat(out, buff);
		}
	}
	strcat(out, "]\n");
}

// ============================================================================
// SHIFT
// ============================================================================

static void testShift(const char *prefix, TYPE result[], size_t shift, int toTheRight)
{
	testSetup();
	ZRArrayOp_shift(MEM, sizeof(TYPE), NBITEMS, shift, toTheRight);
	LOCAL_ZRTEST_RESULT(prefix);
}

MU_TEST(testShift_left)
{
	// static for 0 padding
	static TYPE results[][NBITEMS] =
		{
			{
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' },
				{ 4, 'd' },
				{ 5, 'e' } },
			{
				{ 2, 'b' },
				{ 3, 'c' },
				{ 4, 'd' },
				{ 5, 'e' },
				{ 5, 'e' } },
			{
				{ 3, 'c' },
				{ 4, 'd' },
				{ 5, 'e' },
				{ 4, 'd' },
				{ 5, 'e' } },
			{
				{ 4, 'd' },
				{ 5, 'e' },
				{ 3, 'c' },
				{ 4, 'd' },
				{ 5, 'e' } },
			{
				{ 5, 'e' },
				{ 2, 'b' },
				{ 3, 'c' },
				{ 4, 'd' },
				{ 5, 'e' } },
			{
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' },
				{ 4, 'd' },
				{ 5, 'e' } } };
	char prefix[100];

	for (int i = 0; i < NBITEMS; i++)
	{
		sprintf(prefix, "%s %d: ", __FUNCTION__, i);
		testShift(prefix, results[i], i, 0);
	}
}

MU_TEST(testShift_right)
{
	// static for 0 padding
	static TYPE results[][NBITEMS] =
		{
			{
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' },
				{ 4, 'd' },
				{ 5, 'e' } },
			{
				{ 1, 'a' },
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' },
				{ 4, 'd' } },
			{
				{ 1, 'a' },
				{ 2, 'b' },
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' } },
			{
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' },
				{ 1, 'a' },
				{ 2, 'b' } },
			{
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' },
				{ 4, 'd' },
				{ 1, 'a' } },
			{
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' },
				{ 4, 'd' },
				{ 5, 'e' } } };
	char prefix[100];

	for (int i = 0; i < NBITEMS; i++)
	{
		sprintf(prefix, "%s %d: ", __FUNCTION__, i);
		testShift(prefix, results[i], i, 1);
	}
}

// ============================================================================
// ROTATE
// ============================================================================

static void testRotate(const char *prefix, TYPE result[], size_t rotate, int toTheRight)
{
	testSetup();
	ZRArrayOp_rotate(MEM, sizeof(TYPE), NBITEMS, rotate, toTheRight);
	LOCAL_ZRTEST_RESULT(prefix);
}

MU_TEST(testRotate_left)
{
	// static for 0 padding
	static TYPE results[][NBITEMS] =
		{
			{
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' },
				{ 4, 'd' },
				{ 5, 'e' } },
			{
				{ 2, 'b' },
				{ 3, 'c' },
				{ 4, 'd' },
				{ 5, 'e' },
				{ 1, 'a' } },
			{
				{ 3, 'c' },
				{ 4, 'd' },
				{ 5, 'e' },
				{ 1, 'a' },
				{ 2, 'b' } },
			{
				{ 4, 'd' },
				{ 5, 'e' },
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' } },
			{
				{ 5, 'e' },
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' },
				{ 4, 'd' } },
			{
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' },
				{ 4, 'd' },
				{ 5, 'e' } } };
	char prefix[100];

	for (int i = 0; i < NBITEMS; i++)
	{
		sprintf(prefix, "%s %d: ", __FUNCTION__, i);
		testRotate(prefix, results[i], i, false);
	}
}

MU_TEST(testRotate_right)
{
	// static for 0 padding
	static TYPE results[][NBITEMS] =
		{
			{
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' },
				{ 4, 'd' },
				{ 5, 'e' } },
			{
				{ 1, 'a' },
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' },
				{ 4, 'd' } },
			{
				{ 1, 'a' },
				{ 2, 'b' },
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' } },
			{
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' },
				{ 1, 'a' },
				{ 2, 'b' } },
			{
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' },
				{ 4, 'd' },
				{ 1, 'a' } },
			{
				{ 1, 'a' },
				{ 2, 'b' },
				{ 3, 'c' },
				{ 4, 'd' },
				{ 5, 'e' } } };
	char prefix[100];

	for (int i = 0; i < NBITEMS; i++)
	{
		sprintf(prefix, "%s %d: ", __FUNCTION__, i);
		testShift(prefix, results[i], i, 1);
	}
}

// ============================================================================
// REVERSE
// ============================================================================

MU_TEST(testReverse)
{
	TYPE result[] =
		{
			{ 5, 'e' },
			{ 4, 'd' },
			{ 3, 'c' },
			{ 2, 'b' },
			{ 1, 'a' } };

	ZRTEST_BEGIN()
	;
	ZRArrayOp_reverse(MEM, sizeof(TYPE), NBITEMS);
	LOCAL_ZRTEST_END();
}

// ============================================================================
// REVERSE
// ============================================================================

MU_TEST(testSwitch)
{
	TYPE result[] =
		{
			{ 4, 'd' },
			{ 2, 'b' },
			{ 3, 'c' },
			{ 1, 'a' },
			{ 5, 'e' } };

	ZRTEST_BEGIN()
	;
	ZRArrayOp_swap(MEM, sizeof(TYPE), 0, 3);
	LOCAL_ZRTEST_END();
}

// ============================================================================
// FILL
// ============================================================================

MU_TEST(testFill)
{
	TYPE result[] =
		{
			{ 1, 'a' },
			{ 2, 'b' },
			{ 55, 'X' },
			{ 55, 'X' },
			{ 55, 'X' } };

	ZRTEST_BEGIN()
	;
	ZRArrayOp_fill(MEM + 2, sizeof(TYPE), 3, &(TYPE ) { 55, 'X' });
	LOCAL_ZRTEST_END();
}

// ============================================================================
// BSEARCH
// ============================================================================

#define testSearch(N, SIDE) \
	ZRCONCAT(ZRArrayOp_bsearch,SIDE)(array, sizeof(int), ZRCARRAY_NBOBJ(array), (int[]){ N }, cmpInt, NULL)

#define testInsert(N, SIDE) \
	ZRCONCAT(ZRArrayOp_binsert_pos,SIDE)(array, sizeof(int), ZRCARRAY_NBOBJ(array), (int[]){ N }, cmpInt, NULL)

int cmpInt(void *a, void *b, void *unused)
{
	return *(int*)a - *(int*)b;
}

MU_TEST(testBSearch)
{
	ZRTEST_BEGIN();
	int array[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	int *val;

	for (int N = 1; N <= ZRCARRAY_NBOBJ(array); N++)
	{
		val = testSearch(N,);
		ZRTEST_ASSERT_PTR_NEQ(NULL, val);
		ZRTEST_ASSERT_INT_EQ(N, *val);
	}
	ZRTEST_ASSERT_PTR_EQ(NULL, testSearch(0,));
	ZRTEST_ASSERT_PTR_EQ(NULL, testSearch(11,));
}

#define test(N, PITEM, SIDE) do{ \
	int *ret = testSearch(N, SIDE); \
	ZRTEST_ASSERT_PTR_NEQ(NULL, ret); \
	ZRTEST_ASSERT_PTR_EQ(PITEM, ret); \
}while(0)

MU_TEST(testBSearchMultiple)
{
	ZRTEST_BEGIN();
	int array[] = { 1, 2, 2, 5, 7, 7, 7, 9 };

	test(2, array + 1, _first);
	test(7, array + 4, _first);
	test(9, array + 7, _first);

	test(2, array + 2, _last);
	test(7, array + 6, _last);
	test(9, array + 7, _last);

	ZRTEST_ASSERT_PTR_EQ(NULL, testSearch(0,_first));
	ZRTEST_ASSERT_PTR_EQ(NULL, testSearch(3,_first));
	ZRTEST_ASSERT_PTR_EQ(NULL, testSearch(0,_last));
	ZRTEST_ASSERT_PTR_EQ(NULL, testSearch(3,_last));
}
#undef test

#define test(N, POS) \
	ZRTEST_ASSERT_INT_EQ(POS, testInsert(N,));

MU_TEST(testBInsertPos)
{
	ZRTEST_BEGIN();
	int array[] = { 1, 5, 6, 10 };
	test(0, 0);
	test(3, 1);
	test(7, 3);
	test(9, 3);
	test(11, 4);
}
#undef test

#define test(N, POS, SIDE) \
	ZRTEST_ASSERT_INT_EQ(POS, testInsert(N,SIDE));

MU_TEST(testBInsertPosMultiple)
{
	ZRTEST_BEGIN();
	int array[] = { 1, 5, 5, 6, 7, 7, 7, 9 };
	test(0, 0, _first);
	test(3, 1, _first);
	test(5, 1, _first);
	test(6, 3, _first);
	test(7, 4, _first);
	test(8, 7, _first);
	test(20, 8, _first);

	test(0, 0, _last);
	test(3, 1, _last);
	test(5, 3, _last);
	test(6, 4, _last);
	test(7, 7, _last);
	test(8, 7, _last);
	test(20, 8, _last);
}
#undef test

#undef testSearch
#undef testInsert

// ============================================================================
// TESTS
// ============================================================================

MU_TEST_SUITE( AllTests)
{
	MU_RUN_TEST(testSwitch);
	MU_RUN_TEST(testShift_left);
	MU_RUN_TEST(testShift_right);
	MU_RUN_TEST(testRotate_left);
	MU_RUN_TEST(testRotate_right);
	MU_RUN_TEST(testReverse);
	MU_RUN_TEST(testFill);
	MU_RUN_TEST(testBSearch);
	MU_RUN_TEST(testBSearchMultiple);
	MU_RUN_TEST(testBInsertPos);
	MU_RUN_TEST(testBInsertPosMultiple);
}

int MemoryArrayOpTests(void)
{
	puts(__FUNCTION__);
	MU_SUITE_CONFIGURE(testSetup, NULL);
	MU_RUN_SUITE(AllTests);
	MU_REPORT()
				;
	return minunit_status;
}
