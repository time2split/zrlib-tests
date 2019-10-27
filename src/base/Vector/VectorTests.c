#include <minunit/minunit.h>

#include <zrlib/base/Vector/Vector.h>
#include <zrlib/base/Allocator/CAllocator.h>
#include <zrlib/base/Vector/Vector2SideStrategy.h>
#include "../../main.h"

ZRAllocator *ALLOCATOR;

#define LOCAL_ZRTEST_RESULT(prefix) \
	ZRTEST_RESULT(prefix, MESSAGE_BUFF, result, expected)

#define LOCAL_ZRTEST_END() \
	ZRTEST_END(MESSAGE_BUFF, result, expected)

// ============================================================================

static void testSetup()
{
	mainTestSetup();
}

ZRVector* createFixedVector(size_t initialArraySize, size_t initialMemorySize)
{
	return ZRVector2SideStrategy_createFixedM(initialArraySize, initialMemorySize, sizeof(long), ALLOCATOR);
}

ZRVector* createDynamicVector(size_t initialArraySize, size_t initialMemorySize)
{
	return ZRVector2SideStrategy_createDynamicM(initialArraySize, initialMemorySize, sizeof(long), ALLOCATOR);
}

void deleteVector(ZRVector *vec)
{
	ZRVector_clean(vec);
	ZRFREE(ALLOCATOR, vec->strategy);
	ZRFREE(ALLOCATOR, vec);
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
	ZRTEST_BEGIN();
	ZRVector *result = createFixedVector(8, 0);
	ZRVector *expected = createFixedVector(8, 0);
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

	deleteVector(result);
	deleteVector(expected);
}

MU_TEST(testSize)
{
	ZRTEST_BEGIN();
	ZRVector *result = createFixedVector(8, 0);
	long data = 0;

	mu_check(ZRVector_nbObj(result) == 0);
	ZRVector_add(result, &data), data++;
	mu_check(ZRVector_nbObj(result) == 1);
	ZRVector_add(result, &data), data++;
	mu_check(ZRVector_nbObj(result) == 2);

	deleteVector(result);
}

MU_TEST(testDynamicGrowArrayInitialIs0)
{
	ZRTEST_BEGIN();
	size_t const nb = 100;
	size_t const result_initialMemorySize = 2;
	ZRVector *result = createDynamicVector(0, result_initialMemorySize);
	ZRVector *expected = createFixedVector(nb, 0);

	for (long i = 0; i < nb; i++)
	{
		ZRVector_add(result, &i);
		ZRVector_add(expected, &i);
	}
	LOCAL_ZRTEST_END();

	deleteVector(result);
	deleteVector(expected);
}

MU_TEST(testDynamicGrowMemoryInitialIs0)
{
	ZRTEST_BEGIN();
	size_t const nb = 100;
	size_t const result_initialArraySize = 10;
	ZRVector *result = createDynamicVector(result_initialArraySize, 0);
	ZRVector *expected = createFixedVector(nb, 0);

	for (long i = 0; i < nb; i++)
	{
		ZRVector_add(result, &i);
		ZRVector_add(expected, &i);
	}
	LOCAL_ZRTEST_END();

	deleteVector(result);
	deleteVector(expected);
}

MU_TEST(testDynamicGrow)
{
	ZRTEST_BEGIN();
	size_t const nb = 100;
	size_t const result_initialArraySize = 10;
	size_t const result_initialMemorySize = 20;
	ZRVector *result = createDynamicVector(result_initialArraySize, result_initialMemorySize);
	ZRVector *expected = createFixedVector(nb, 0);

	for (long i = 0; i < nb; i++)
	{
		ZRVector_add(result, &i);
		ZRVector_add(expected, &i);
	}
	LOCAL_ZRTEST_END();

	deleteVector(result);
	deleteVector(expected);
}

MU_TEST(testDynamicGrowAddOverflowSizeInitialArray)
{
	ZRTEST_BEGIN();
	size_t const nb = 100;
	size_t const result_initialArraySize = 10;
	ZRVector *result = createDynamicVector(result_initialArraySize, 0);
	ZRVector *expected = createFixedVector(nb, 0);

	for (long i = 0; i < nb; i++)
	{
		ZRVector_add(expected, &i);
	}
	ZRVector_add_nb(result, nb, expected->array);
	LOCAL_ZRTEST_END();

	deleteVector(result);
	deleteVector(expected);
}

MU_TEST(testDynamicGrowAddOverflowSizeInitialMemory)
{
	ZRTEST_BEGIN();
	size_t const nb = 100;
	size_t const result_initialMemorySize = 10;
	ZRVector *result = createDynamicVector(0, result_initialMemorySize);
	ZRVector *expected = createFixedVector(nb, 0);

	for (long i = 0; i < nb; i++)
	{
		ZRVector_add(expected, &i);
	}
	ZRVector_add_nb(result, nb, expected->array);
	LOCAL_ZRTEST_END();

	deleteVector(result);
	deleteVector(expected);
}

MU_TEST(testDynamicShrinkLeft)
{
	ZRTEST_BEGIN();
	size_t const nb = 15;
	size_t const less = 8;
	size_t const offset = 2;
	ZRVector *result = createDynamicVector(0, nb);
	ZRVector *expected = createFixedVector(nb - less, 0);

	for (long i = 0; i < nb; i++)
	{
		ZRVector_add(result, &i);

		if (i < offset || i >= offset + less)
			ZRVector_add(expected, &i);
	}
	ZRVector_delete_nb(result, offset, less);
	LOCAL_ZRTEST_END();

	deleteVector(result);
	deleteVector(expected);
}

MU_TEST(testDynamicShrinkRight)
{
	ZRTEST_BEGIN();
	size_t const nb = 15;
	size_t const less = 8;
	size_t const offset = 6;
	ZRVector *result = createDynamicVector(0, nb);
	ZRVector *expected = createFixedVector(nb - less, 0);

	for (long i = 0; i < nb; i++)
	{
		ZRVector_add(result, &i);

		if (i < offset || i >= offset + less)
			ZRVector_add(expected, &i);
	}
	ZRVector_delete_nb(result, offset, less);
	LOCAL_ZRTEST_END();

	deleteVector(result);
	deleteVector(expected);
}

// ============================================================================
// TESTS
// ============================================================================

MU_TEST_SUITE( AllTests)
{
	MU_RUN_TEST(testSize);
	MU_RUN_TEST(testInsert);
	MU_RUN_TEST(testDynamicGrowArrayInitialIs0);
	MU_RUN_TEST(testDynamicGrowMemoryInitialIs0);
	MU_RUN_TEST(testDynamicGrow);
	MU_RUN_TEST(testDynamicGrowAddOverflowSizeInitialArray);
	MU_RUN_TEST(testDynamicGrowAddOverflowSizeInitialMemory);
	MU_RUN_TEST(testDynamicShrinkLeft);
	MU_RUN_TEST(testDynamicShrinkRight);
}

int VectorTests(void)
{
	ALLOCATOR = malloc(sizeof(*ALLOCATOR));
	ZRCAllocator_init(ALLOCATOR);

	puts(__FUNCTION__);
	MU_SUITE_CONFIGURE(testSetup, NULL);
	MU_RUN_SUITE(AllTests);
	MU_REPORT()
	;
	free(ALLOCATOR);
	return minunit_status;
}
