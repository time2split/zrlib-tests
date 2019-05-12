#include <minunit/minunit.h>

#include <zrlib/base/Vector/Vector.h>
#include <zrlib/base/Allocator/CAllocator.h>
#include <zrlib/base/Vector/Vector2SideStrategy.h>
#include "../../main.h"

ZRAllocator *ALLOCATOR;
ZRVectorStrategy *STRATEGY;
ZRVector *RESULT;
ZRVector *EXPECTED;

#define LOCAL_ZRTEST_RESULT(prefix) \
	ZRTEST_RESULT(prefix, MESSAGE_BUFF, result, expected)

#define LOCAL_ZRTEST_END() \
	ZRTEST_END(MESSAGE_BUFF, result, expected)

// ============================================================================

static void testSetup()
{
	mainTestSetup();
	ALLOCATOR = malloc(sizeof(*ALLOCATOR));
	ZRCAllocator_init(ALLOCATOR);
	STRATEGY = ZRALLOC(ALLOCATOR, ZRVector_2SideStrategy_size());
	ZRVector_2SideStrategy_init(STRATEGY, ALLOCATOR);
}

static void testShutdown(void)
{
	ZRVector_clean(RESULT);
	free(RESULT);
	ZRVector_clean(EXPECTED);
	free(EXPECTED);
}

ZRVector* getExpectedVector(void)
{
	if (EXPECTED == NULL)
	{
		EXPECTED = malloc(ZRVECTOR_SIZEOF_STRUCT(STRATEGY));
		ZRVector_init(EXPECTED, sizeof(long), STRATEGY, NULL);
		return EXPECTED;
	}
	else
	{
		ZRVECTOR_CLEAN(EXPECTED);
		free(EXPECTED);
		return getExpectedVector();
	}
}
ZRVector* getResultVector(void)
{
	if (RESULT == NULL)
	{
		RESULT = malloc(ZRVECTOR_SIZEOF_STRUCT(STRATEGY));

		size_t const sdataSize = ZRVector_2SideData_size();
		char sdata[sdataSize];
		ZRVector_2SideData_init(sdata, STRATEGY);
		ZRVector_2SideData_initialSpace(sdata, STRATEGY, 1);
		ZRVector_init(RESULT, sizeof(long), STRATEGY, sdata);
		return RESULT;
	}
	else
	{
		ZRVECTOR_CLEAN(RESULT);
		free(RESULT);
		return getResultVector();
	}
}

// ============================================================================

static int FUN_CMP_NAME(ZRVector *result, ZRVector *expected)
{
	int cmp;
	cmp = ZRVector_nbObj(result) - ZRVector_nbObj(expected);

	if (cmp)
		return cmp;

	size_t nbObj = ZRVector_nbObj(result);
	size_t objSize = ZRVector_objSize(result);

	while (nbObj--)
	{
		cmp = memcmp(ZRVector_get(result, nbObj), ZRVector_get(expected, nbObj), objSize);

		if (cmp)
			return cmp;
	}
	return 0;
}

static void FUN_PRINT_NAME(char *out, ZRVector *vec)
{
	char buff[2048];
	size_t nbObj = ZRVector_nbObj(vec);
	size_t i = 0;
	strcat(out, "[");

	while (nbObj--)
	{
		long* l = ZRVector_get(vec, i);
		sprintf(buff, "%s%ld", (i ? "," : ""), *l);
		strcat(out, buff);
		i++;
	}
	strcat(out, "]\n");
}
// ============================================================================

MU_TEST(testInsert)
{
	ZRVector *result = getResultVector();
	ZRVector *expected = getExpectedVector();
	long i[8];
	i[0] = 100;
	ZRVector_add(result, i);
	i[0] = 1;
	ZRVector_add(result, i);

	i[0] = 21;
	i[1] = 22;
	ZRVector_insert_nb(result, 1, 2, i);

	i[0] = 1000;
	i[1] = 1001;
	ZRVector_addFirst_nb(result, 2, i);

	long lexpected[] = { 1000, 1001, 100, 21, 22, 1 };
	ZRVector_add_nb(expected, sizeof(lexpected) / sizeof(long), lexpected);
	LOCAL_ZRTEST_END();
}

// ============================================================================
// TESTS
// ============================================================================

MU_TEST_SUITE( AllTests)
{
	MU_RUN_TEST(testInsert);
}

int VectorTests(void)
{
	puts(__FUNCTION__);
	MU_SUITE_CONFIGURE(testSetup, NULL);
	MU_RUN_SUITE(AllTests);
	MU_REPORT()
	;
	testShutdown();
	return minunit_status;
}
