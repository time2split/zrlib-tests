#include <stdlib.h>
#include <minunit/minunit.h>

#include <zrlib/base/Identifier/MapIdentifier.h>
#include <zrlib/base/Allocator/CAllocator.h>
#include <zrlib/base/Algorithm/fcmp.h>
#include "../../main.h"

// ============================================================================
#define TYPE long

ZRAllocator *ALLOCATOR;
ZRObjInfos OBJINFOS = ZRTYPE_OBJINFOS(TYPE);

size_t hash_long(void *l, void *data)
{
	return *(long*)l;
}

int hash_cmp(void *a, void *b, void *data)
{
	return zrfcmp_long(*(long**)a, *(long**)b);
}

// ============================================================================

ZRIdentifier* createMapIdentifier(void)
{
	alignas (max_align_t)
	char infos[1024 * 10];
	zrfuhash fuhash = hash_long;

	ZRMapIdentifierInfos(infos, OBJINFOS, &fuhash, 1);
	ZRMapIdentifierInfos_allocator(infos, ALLOCATOR);
	return ZRMapIdentifier_new(infos);
}

#define XLIST() \
	X(Map)

#define X(a) #a, create ## a ## Identifier
static struct
{
	char *name;
	ZRIdentifier* (*fcreate)(void);
} CONFIGS[] = { XLIST() }, *CONFIG;
#undef X

#define ZRIDTEST_BEGIN() ZRTEST_PRINTF("config(%d) %s: ", (int)(CONFIG - CONFIGS), CONFIG->name)

// ============================================================================

MU_TEST(internEq)
{
	ZRIDTEST_BEGIN();
	long *p, *pp;
	long l;

	ZRIdentifier *identifier = CONFIG->fcreate();
	l = 1;
	p = ZRIdentifier_intern(identifier, &l);
	pp = ZRIdentifier_intern(identifier, &l);

	ZRTEST_ASSERT_INT_EQ(1, ZRIdentifier_nbObj(identifier));
	ZRTEST_ASSERT_INT_EQ(1, *p);
	ZRTEST_ASSERT_INT_EQ(1, *pp);
	ZRTEST_ASSERT_PTR_EQ(p, pp);
	ZRTEST_ASSERT_INT_EQ(ZRIdentifier_getID(identifier, p), ZRIdentifier_getID(identifier, pp));

	ZRIdentifier_destroy(identifier);
}

MU_TEST(internDiff)
{
	ZRIDTEST_BEGIN();
	ZRID id1, id2;
	long *p1, *p2;
	long l;

	ZRIdentifier *identifier = CONFIG->fcreate();
	l = 1, p1 = ZRIdentifier_intern(identifier, &l);
	l = 2, p2 = ZRIdentifier_intern(identifier, &l);
	id1 = ZRIdentifier_getID(identifier, p1);
	id2 = ZRIdentifier_getID(identifier, p2);

	ZRTEST_ASSERT_INT_EQ(2, ZRIdentifier_nbObj(identifier));
	ZRTEST_ASSERT_INT_EQ(1, *p1);
	ZRTEST_ASSERT_INT_EQ(2, *p2);
	ZRTEST_ASSERT_INT_NE(*p1, *p2);
	ZRTEST_ASSERT_INT_NE(id1, id2);

	ZRIdentifier_destroy(identifier);
}

MU_TEST(internFromID)
{
	ZRIDTEST_BEGIN();
	long nb = 10000;
	long l;

	ZRIdentifier *identifier = CONFIG->fcreate();

	for (l = 0; l < nb; l++)
		ZRIdentifier_intern(identifier, &l);

	for (l = 0; l < nb; l++)
	{
		ZRID id = ZRIdentifier_getID(identifier, &l);
		long *lv = (long*)ZRIdentifier_fromID(identifier, id);
		ZRTEST_PRINTF("%ld\n", l);
		ZRTEST_ASSERT_PTR_NE(NULL, lv);
		ZRTEST_ASSERT_INT_EQ(l, *lv);
	}
	ZRIdentifier_destroy(identifier);
}

MU_TEST(release)
{
	ZRIDTEST_BEGIN();
	long nb = 100;
	long l;
	ZRIdentifier *identifier = CONFIG->fcreate();

	for (l = 0; l < nb; l++)
		ZRIdentifier_intern(identifier, &l);

	l = 7;
	ZRTEST_ASSERT_INT_EQ(true, ZRIdentifier_contains(identifier, &l));
	ZRIdentifier_release(identifier, &l);
	ZRTEST_ASSERT_INT_EQ(nb - 1, ZRIdentifier_nbObj(identifier));
	ZRTEST_ASSERT_INT_EQ(false, ZRIdentifier_contains(identifier, &l));

	ZRIdentifier_destroy(identifier);
}

MU_TEST(releaseID)
{
	ZRIDTEST_BEGIN();
	long nb = 100;
	long l;
	ZRIdentifier *identifier = CONFIG->fcreate();

	for (l = 0; l < nb; l++)
		ZRIdentifier_intern(identifier, &l);

	l = 7;
	ZRID id = ZRIdentifier_getID(identifier, &l);
	ZRTEST_ASSERT_INT_EQ(true, ZRIdentifier_contains(identifier, &l));
	ZRIdentifier_releaseID(identifier, id);
	ZRTEST_ASSERT_INT_EQ(nb - 1, ZRIdentifier_nbObj(identifier));
	ZRTEST_ASSERT_INT_EQ(false, ZRIdentifier_contains(identifier, &l));

	ZRIdentifier_destroy(identifier);
}

MU_TEST(releaseAll)
{
	ZRIDTEST_BEGIN();
	long nb = 100;
	long l;
	ZRIdentifier *identifier = CONFIG->fcreate();

	for (l = 0; l < nb; l++)
		ZRIdentifier_intern(identifier, &l);

	l = 7;
	ZRTEST_ASSERT_INT_EQ(true, ZRIdentifier_contains(identifier, &l));
	ZRIdentifier_releaseAll(identifier);
	ZRTEST_ASSERT_INT_EQ(0, ZRIdentifier_nbObj(identifier));
	ZRTEST_ASSERT_INT_EQ(false, ZRIdentifier_contains(identifier, &l));

	ZRIdentifier_destroy(identifier);
}

// ============================================================================
// TESTS
// ============================================================================

MU_TEST_SUITE( AllTests)
{
	MU_RUN_TEST(internEq);
	MU_RUN_TEST(internDiff);
	MU_RUN_TEST(internFromID);
	MU_RUN_TEST(release);
	MU_RUN_TEST(releaseID);
	MU_RUN_TEST(releaseAll);
}

int IdentifierTests(void)
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
