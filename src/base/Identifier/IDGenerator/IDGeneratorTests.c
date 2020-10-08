#include <stdlib.h>
#include <minunit/minunit.h>

#include <zrlib/base/Identifier/IDGenerator/IDGenerator.h>
#include <zrlib/base/Allocator/CAllocator.h>
#include <zrlib/base/Algorithm/fcmp.h>
#include "../../../main.h"

// ============================================================================

ZRAllocator *ALLOCATOR;

// ============================================================================

ZRIDGenerator* createIDGenerator(void)
{
	ZRInitInfos_t infos;

	ZRIDGeneratorInfos(infos, ALLOCATOR);
	return ZRIDGenerator_new(infos);
}

#define XLIST() \
	X(IDGenerator)

#define X(a) #a, create ## a
static struct
{
	char *name;
	ZRIDGenerator* (*fcreate)(void);
} CONFIGS[] = { XLIST() }, *CONFIG;
#undef X

#define ZRIDGENERATORTEST_BEGIN() ZRTEST_PRINTF("config(%d) %s: ", (int)(CONFIG - CONFIGS), CONFIG->name)

// ============================================================================

MU_TEST(generate)
{
	ZRIDGENERATORTEST_BEGIN();
	ZRIDGenerator *generator = CONFIG->fcreate();
	ZRID id;
	size_t nb = 100;

	for (size_t i = 0; i < nb; i++)
	{
		ZRTEST_ASSERT_INT_EQ(false, ZRIDGenerator_present(generator, i));
		ZRTEST_ASSERT_INT_EQ(i, ZRIDGenerator_generate(generator));
		ZRTEST_ASSERT_INT_EQ(true, ZRIDGenerator_present(generator, i));
	}
	ZRTEST_ASSERT_INT_EQ(nb, generator->nbGenerated);

	ZRIDGenerator_destroy(generator);
}

MU_TEST(release)
{
	ZRIDGENERATORTEST_BEGIN();
	ZRIDGenerator *generator = CONFIG->fcreate();
	ZRID id;
	size_t nb = 100, delete = 23;

	for (size_t i = 0; i < nb; i++)
		ZRTEST_ASSERT_INT_EQ(i, ZRIDGenerator_generate(generator));

	ZRIDGenerator_release(generator, 4);
	ZRIDGenerator_release(generator, 2);
	ZRIDGenerator_release(generator, 7);
	ZRTEST_ASSERT_INT_EQ(2, ZRIDGenerator_generate(generator));
	ZRTEST_ASSERT_INT_EQ(4, ZRIDGenerator_generate(generator));
	ZRTEST_ASSERT_INT_EQ(7, ZRIDGenerator_generate(generator));

	for (size_t i = nb - delete; i < nb; i++)
	{
		ZRIDGenerator_release(generator, i);
		ZRTEST_ASSERT_INT_EQ(false, ZRIDGenerator_present(generator, i));
	}
	ZRTEST_ASSERT_INT_EQ(nb - delete, generator->nbGenerated);
	ZRTEST_ASSERT_INT_EQ(nb - delete, ZRIDGenerator_generate(generator));

	ZRIDGenerator_destroy(generator);
}

MU_TEST(releaseAll)
{
	ZRIDGENERATORTEST_BEGIN();
	ZRIDGenerator *generator = CONFIG->fcreate();
	ZRID id;
	size_t nb = 100;

	for (size_t i = 0; i < nb; i++)
		ZRIDGenerator_generate(generator);

	ZRIDGenerator_releaseAll(generator);
	ZRTEST_ASSERT_INT_EQ(0, generator->nbGenerated);

	for (size_t i = 0; i < nb; i++)
		ZRTEST_ASSERT_INT_EQ(false, ZRIDGenerator_present(generator, i));

	ZRIDGenerator_destroy(generator);
}

// ============================================================================
// TESTS
// ============================================================================

MU_TEST_SUITE( AllTests)
{
	MU_RUN_TEST(generate);
	MU_RUN_TEST(release);
	MU_RUN_TEST(releaseAll);
}

int IDGeneratorTests(void)
{
	puts(__FUNCTION__);

	ALLOCATOR = malloc(sizeof(*ALLOCATOR));
	ZRCAllocator_init(ALLOCATOR);

	for (size_t i = 0; i < ZRCARRAY_NBOBJ(CONFIGS); i++)
	{
		CONFIG = &CONFIGS[i];

		MU_RUN_SUITE(AllTests);
		MU_REPORT();
	}
	free(ALLOCATOR);
	return minunit_status;
}
