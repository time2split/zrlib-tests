#include <minunit/minunit.h>
#include <stdint.h>

#include <zrlib/base/AddressOp.h>
#include <zrlib/base/Vector/Vector.h>
#include <zrlib/base/Allocator/CAllocator.h>
#include <zrlib/base/Vector/Vector2SideStrategy.h>
#include "../../main.h"

static ZRAllocator *ALLOCATOR;
static ZRInitInfos_t INITINFOS;

#define CONFIG_XLIST() \
	X(fixedVector, true) \
	X(dynamicVector, false)

#define ZRVECTORTEST_BEGIN() ZRTEST_PRINTF("(%s) ", CONFIG.name)

#define LOCAL_ZRTEST_RESULT(prefix) \
	ZRTEST_RESULT(prefix, MESSAGE_BUFF, result, expected)

#define LOCAL_ZRTEST_END() \
	ZRTEST_END(MESSAGE_BUFF, result, expected)

typedef struct
{
	char *name;
	ZRVector* (*fcreate)(ZRObjInfos objInfos, size_t initialArraySize, size_t initialMemorySize);
	unsigned fixed :1;
} Config;

ZRVector* create_fixedVector(ZRObjInfos objInfos, size_t initialArraySize, size_t initialMemorySize)
{
	ZRVector2SideStrategyInfos(INITINFOS, objInfos);
	ZRVector2SideStrategyInfos_allocator(INITINFOS, ALLOCATOR);
	ZRVector2SideStrategyInfos_initialArraySize(INITINFOS, initialArraySize);
	ZRVector2SideStrategyInfos_initialMemorySize(INITINFOS, initialMemorySize);
	ZRVector2SideStrategyInfos_fixedArray(INITINFOS);
	return ZRVector2SideStrategy_new(INITINFOS);
}

ZRVector* create_dynamicVector(ZRObjInfos objInfos, size_t initialArraySize, size_t initialMemorySize)
{
	ZRVector2SideStrategyInfos(INITINFOS, objInfos);
	ZRVector2SideStrategyInfos_allocator(INITINFOS, ALLOCATOR);
	ZRVector2SideStrategyInfos_initialArraySize(INITINFOS, initialArraySize);
	ZRVector2SideStrategyInfos_initialMemorySize(INITINFOS, initialMemorySize);
	return ZRVector2SideStrategy_new(INITINFOS);
}

#define X(NAME, FIXED) # NAME, create_ ## NAME, FIXED,
static Config CONFIGS[] = { CONFIG_XLIST() };
#undef X
static Config CONFIG;

#define ASSERT_FIXED() ZRBLOCK( \
	if(false == CONFIG.fixed) return; \
)

#define ASSERT_DYNAMIC() ZRBLOCK( \
	if(true == CONFIG.fixed) return; \
)

// ============================================================================

static void testSetup()
{
	mainTestSetup();
}

static void printArray_long(char *out, long *array, size_t nbObj)
{
	char buff[128];
	size_t i = 0;
	strcat(out, "[");

	while (nbObj--)
	{
		long l = array[i];
		snprintf(buff, ZRCARRAY_NBOBJ(buff), "%s%ld", (i ? "," : ""), l);
		strcat(out, buff);
		i++;
	}
	strcat(out, "]\n");
}

static bool checkArray_long(long *lexpected, size_t nbObj, ZRVector *result)
{
	for (size_t i = 0; i < nbObj; i++)
	{
		long res = *(long*)ZRVECTOR_GET(result, i);

		if (res != lexpected[i])
		{
			char buffer_e[4096] = "";
			char buffer_r[4096] = "";
			printArray_long(buffer_e, lexpected, nbObj);
			printArray_long(buffer_r, ZRVECTOR_ARRAYP(result), nbObj);
			ZRTEST_CATPRINTF("failed: expected\n%s but have\n%s\n", buffer_e, buffer_r);
			return false;
		}
	}
	return true;
}

// ============================================================================

MU_TEST(testInsert)
{
	ZRVECTORTEST_BEGIN();
	long lexpected[] = { 1000, 1001, 100, 21, 22, 1 };
	ZRVector *result = CONFIG.fcreate(ZRTYPE_OBJINFOS(long), 8, 0);

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

	if (false == checkArray_long(lexpected, ZRCARRAY_NBOBJ(lexpected), result))
		ZRTEST_FAIL();

	ZRVector_destroy(result);
}

MU_TEST(testSize)
{
	ZRVECTORTEST_BEGIN();
	ZRVector *result = CONFIG.fcreate(ZRTYPE_OBJINFOS(long), 2, 0);
	long data = 0;

	ZRTEST_ASSERT_INT_EQ(0, ZRVector_nbObj(result));
	ZRVector_add(result, &data), data++;
	ZRTEST_ASSERT_INT_EQ(1, ZRVector_nbObj(result));
	ZRVector_add(result, &data), data++;
	ZRTEST_ASSERT_INT_EQ(2, ZRVector_nbObj(result));

	ZRVector_destroy(result);
}

MU_TEST(testDynamicGrowArrayInitialIs0)
{
	ASSERT_DYNAMIC();
	ZRVECTORTEST_BEGIN();
	size_t const nb = 100;
	size_t const result_initialMemorySize = 2;
	ZRVector *result = CONFIG.fcreate(ZRTYPE_OBJINFOS(long), 0, result_initialMemorySize);
	long lexpected[nb];

	for (long i = 0; i < nb; i++)
	{
		ZRVector_add(result, &i);
		lexpected[i] = i;
	}

	if (false == checkArray_long(lexpected, ZRCARRAY_NBOBJ(lexpected), result))
		ZRTEST_FAIL();

	ZRVector_destroy(result);
}

MU_TEST(testDynamicGrowMemoryInitialIs0)
{
	ASSERT_DYNAMIC();
	ZRVECTORTEST_BEGIN();
	size_t const nb = 100;
	size_t const result_initialArraySize = 10;
	ZRVector *result = CONFIG.fcreate(ZRTYPE_OBJINFOS(long), result_initialArraySize, 0);
	long lexpected[nb];

	for (long i = 0; i < nb; i++)
	{
		ZRVector_add(result, &i);
		lexpected[i] = i;
	}

	if (false == checkArray_long(lexpected, ZRCARRAY_NBOBJ(lexpected), result))
		ZRTEST_FAIL();

	ZRVector_destroy(result);
}

MU_TEST(testDynamicGrow)
{
	ASSERT_DYNAMIC();
	ZRVECTORTEST_BEGIN();
	size_t const nb = 100;
	size_t const result_initialArraySize = 10;
	size_t const result_initialMemorySize = 20;
	ZRVector *result = CONFIG.fcreate(ZRTYPE_OBJINFOS(long), result_initialArraySize, result_initialMemorySize);
	long lexpected[nb];

	for (long i = 0; i < nb; i++)
	{
		ZRVector_add(result, &i);
		lexpected[i] = i;
	}

	if (false == checkArray_long(lexpected, ZRCARRAY_NBOBJ(lexpected), result))
		ZRTEST_FAIL();

	ZRVector_destroy(result);
}

MU_TEST(testDynamicGrowAddOverflowSizeInitialArray)
{
	ASSERT_DYNAMIC();
	ZRVECTORTEST_BEGIN();
	size_t const nb = 100;
	size_t const result_initialArraySize = 10;
	ZRVector *result = CONFIG.fcreate(ZRTYPE_OBJINFOS(long), result_initialArraySize, 0);
	long lexpected[nb];

	for (long i = 0; i < nb; i++)
		lexpected[i] = i;

	ZRVector_add_nb(result, nb, lexpected);

	if (false == checkArray_long(lexpected, ZRCARRAY_NBOBJ(lexpected), result))
		ZRTEST_FAIL();

	ZRVector_destroy(result);
}

MU_TEST(testDynamicGrowAddOverflowSizeInitialMemory)
{
	ASSERT_DYNAMIC();
	ZRVECTORTEST_BEGIN();
	size_t const nb = 100;
	size_t const result_initialMemorySize = 10;
	ZRVector *result = CONFIG.fcreate(ZRTYPE_OBJINFOS(long), 0, result_initialMemorySize);
	long lexpected[nb];

	for (long i = 0; i < nb; i++)
		lexpected[i] = i;

	ZRVector_add_nb(result, nb, lexpected);

	if (false == checkArray_long(lexpected, ZRCARRAY_NBOBJ(lexpected), result))
		ZRTEST_FAIL();

	ZRVector_destroy(result);
}

MU_TEST(testDynamicShrinkLeft)
{
	ASSERT_DYNAMIC();
	ZRVECTORTEST_BEGIN();
	size_t const nb = 15;
	size_t const less = 8;
	size_t const offset = 2;
	ZRVector *result = CONFIG.fcreate(ZRTYPE_OBJINFOS(long), 0, nb);
	long lexpected[nb - less];

	for (long i = 0, ei = 0; i < nb; i++)
	{
		ZRVector_add(result, &i);

		if (i < offset || i >= offset + less)
			lexpected[ei++] = i;
	}
	ZRVector_delete_nb(result, offset, less);

	if (false == checkArray_long(lexpected, ZRCARRAY_NBOBJ(lexpected), result))
		ZRTEST_FAIL();

	ZRVector_destroy(result);
}

MU_TEST(testDynamicShrinkRight)
{
	ASSERT_DYNAMIC();
	ZRVECTORTEST_BEGIN();
	size_t const nb = 15;
	size_t const less = 8;
	size_t const offset = 6;
	ZRVector *result = CONFIG.fcreate(ZRTYPE_OBJINFOS(long), 0, nb);
	long lexpected[nb - less];

	for (long i = 0, ei = 0; i < nb; i++)
	{
		ZRVector_add(result, &i);

		if (i < offset || i >= offset + less)
			lexpected[ei++] = i;
	}
	ZRVector_delete_nb(result, offset, less);

	if (false == checkArray_long(lexpected, ZRCARRAY_NBOBJ(lexpected), result))
		ZRTEST_FAIL();

	ZRVector_destroy(result);
}

MU_TEST(testTrim)
{
	ZRVECTORTEST_BEGIN();
	size_t const nb = 15;
	size_t const capacity = nb * 3;
	ZRVector *result = CONFIG.fcreate(ZRTYPE_OBJINFOS(long), 0, capacity);

	for (long i = 0; i < nb; i++)
		ZRVector_add(result, &i);

	ZRTEST_CHECK(capacity - nb <= ZRVECTOR_CAPACITY(result));
	ZRTEST_ASSERT_INT_EQ(nb, ZRVECTOR_NBOBJ(result));

	ZRVector_memoryTrim(result);

	if (false == CONFIG.fixed)
		ZRTEST_ASSERT_INT_EQ(nb, ZRVECTOR_CAPACITY(result));

	ZRTEST_ASSERT_INT_EQ(nb, ZRVECTOR_NBOBJ(result));

	if (false == CONFIG.fixed)
	{
		ZRVector_add(result, (int[] ) { 12 });
		ZRTEST_CHECK(nb <= ZRVECTOR_CAPACITY(result));
		ZRTEST_ASSERT_INT_EQ(nb + 1, ZRVECTOR_NBOBJ(result));
	}
	ZRVector_destroy(result);
}

MU_TEST(testChangeSize)
{
	typedef char T1[4];
	typedef uint64_t T2;

	size_t const T1_align = 2;
	ZRVECTORTEST_BEGIN();
	size_t const nb = 100;
	size_t const capacity = (nb + 1) * sizeof(T2);
	ZRVector *result = CONFIG.fcreate(ZROBJINFOS_DEF(T1_align, sizeof(T1)), capacity, capacity);

	for (T1 i = { 0 }; i[0] < nb; i[0]++)
		ZRVector_add(result, &i);

	ZRTEST_ASSERT_INT_EQ(nb, ZRVECTOR_NBOBJ(result));
	ZRTEST_CHECK(ZRAddress_aligned_a(ZRVECTOR_ARRAYP(result), T1_align));
	return;
	ZRVector_changeObjSize(result, ZRTYPE_OBJINFOS(T2));
	ZRTEST_ASSERT_INT_EQ(0, ZRVECTOR_NBOBJ(result));

	for (T2 i = 0; i < nb; i++)
		ZRVector_add(result, &i);

	ZRTEST_ASSERT_INT_EQ(nb, ZRVECTOR_NBOBJ(result));
	ZRTEST_ASSERT_PTR_EQ(ZRVECTOR_GET(result, 0), ZRVECTOR_ARRAYP(result));
	ZRTEST_ASSERT_INT_EQ(0, ZRAddress_aligned_a(ZRVECTOR_ARRAYP(result), alignof(T2)));
	ZRTEST_ASSERT_INT_EQ(nb / 2, (*(T2* )ZRVECTOR_GET(result, nb / 2)));
	ZRVector_destroy(result);
}

MU_TEST(testStress)
{
	ASSERT_DYNAMIC();
	ZRVECTORTEST_BEGIN();
#define T1 int

	ZRTEST_BEGIN();
	size_t const nb = 10000;
	size_t const nb_rest = 13;
	size_t const capacity = nb * 3;
	ZRVector *result = CONFIG.fcreate(ZRTYPE_OBJINFOS(T1), 0, 1);

	for (T1 i = 0; i < nb; i++)
		ZRVector_add(result, &i);

	ZRTEST_ASSERT_INT_EQ(nb, ZRVECTOR_NBOBJ(result));

	for (T1 i = 0; i < nb - nb_rest; i++)
		ZRVector_dec(result);

	ZRTEST_ASSERT_INT_EQ(nb_rest, ZRVECTOR_NBOBJ(result));
	ZRVector_destroy(result);
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
	MU_RUN_TEST(testTrim);
	MU_RUN_TEST(testChangeSize);
	MU_RUN_TEST(testStress);
}

int VectorTests(void)
{
	ALLOCATOR = malloc(sizeof(*ALLOCATOR));
	ZRCAllocator_init(ALLOCATOR);

	puts(__FUNCTION__);
	MU_SUITE_CONFIGURE(testSetup, NULL);

	for (size_t i = 0; i < ZRCARRAY_NBOBJ(CONFIGS); i++)
	{
		CONFIG = CONFIGS[i];
		MU_RUN_SUITE(AllTests);
	}
	MU_REPORT()
				;
	free(ALLOCATOR);
	return minunit_status;
}
