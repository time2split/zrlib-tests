#include <stdlib.h>
#include <minunit/minunit.h>

#include <zrlib/base/Identifier/MapIdentifier.h>
#include <zrlib/base/Allocator/CAllocator.h>
#include <zrlib/base/Algorithm/fcmp.h>
#include <zrlib/base/Algorithm/hash.h>
#include "../../main.h"

// ============================================================================

ZRAllocator *ALLOCATOR;

size_t hash_obj(void *obj, void *data)
{
	ZRObjectP *objectP = (ZRObjectP*)obj;

//	if (ZROBJINFOS_EQ(objectP->infos, ZRTYPE_OBJINFOS(long)))
//		return *(long*)objectP->object;

	return zrhash_jenkins_one_at_a_time(objectP->object, objectP->infos.size);
}

int cmp_obj(void *ap, void *bp, void *data)
{
	ZRObjectP *a = (ZRObjectP*)ap;
	ZRObjectP *b = (ZRObjectP*)bp;

	if (!ZROBJINFOS_EQ(a->infos, b->infos))
		return 1;

//	if (ZROBJINFOS_EQ(a->infos, ZRTYPE_OBJINFOS(long)))
//		return *(long*)a->object - *(long*)b->object;

	return memcmp(a->object, b->object, a->infos.size);
}

// ============================================================================

static ZRIdentifier* createMapIdentifier(void)
{
	alignas (max_align_t)
	char infos[1024 * 10];
	zrfuhash fuhash = hash_obj;

	ZRMapIdentifierIInfos(infos, ZROBJINFOS_DEF_UNKNOWN(), &fuhash, 1);
	ZRMapIdentifierIInfos_allocator(infos, ALLOCATOR);
	ZRMapIdentifierIInfos_fucmp(infos, cmp_obj);
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
	ZRObjectP objectP;
	ZRPTYPE_0(&objectP);

	long *p, *pp;
	long l;
	objectP = ZROBJECTP_DEF(ZRTYPE_OBJINFOS(long), &l);

	ZRIdentifier *identifier = CONFIG->fcreate();
	l = 1;
	p = ZRIdentifier_intern(identifier, &objectP);
	pp = ZRIdentifier_intern(identifier, &objectP);

	ZRTEST_ASSERT_INT_EQ(1, ZRIdentifier_nbObj(identifier));
	ZRTEST_ASSERT_INT_EQ(1, *p);
	ZRTEST_ASSERT_PTR_EQ(p, pp);
	ZRTEST_ASSERT_INT_EQ(ZRIdentifier_getID(identifier, &objectP), ZRIdentifier_getID(identifier, &objectP));

	ZRIdentifier_destroy(identifier);
}

MU_TEST(internDiff)
{
	ZRIDTEST_BEGIN();

	ZRID id1, id2;
	char *p1, *p2;

	ZRIdentifier *identifier = CONFIG->fcreate();

	p1 = ZRIdentifier_intern(identifier, &ZRSTRING_OBJECTP("one"));
	p2 = ZRIdentifier_intern(identifier, &ZRSTRING_OBJECTP("two"));
	id1 = ZRIdentifier_getID(identifier, &ZRSTRING_OBJECTP("one"));
	id2 = ZRIdentifier_getID(identifier, &ZRSTRING_OBJECTP("two"));

	ZRTEST_ASSERT_INT_EQ(2, ZRIdentifier_nbObj(identifier));
	ZRTEST_ASSERT_STR_EQ("one", p1);
	ZRTEST_ASSERT_STR_EQ("two", p2);
	ZRTEST_ASSERT_STR_NE(p1, p2);
	ZRTEST_ASSERT_INT_NE(id1, id2);

	ZRIdentifier_destroy(identifier);
}

#define l(i) i * .25

MU_TEST(internFromID)
{
	ZRIDTEST_BEGIN();
	int nb = 10000;
	double l;

	ZRIdentifier *identifier = CONFIG->fcreate();

	l = 0;

	for (int i = 0; i < nb; i++, l = l(i))
		ZRIdentifier_intern(identifier, &ZRPTYPE_OBJECTP(&l));

	ZRTEST_ASSERT_INT_EQ(nb, ZRIdentifier_nbObj(identifier));
	l = 0;

	for (int i = 0; i < nb; i++, l = l(i))
	{
		ZRID id = ZRIdentifier_getID(identifier, &ZRPTYPE_OBJECTP(&l));
		double *dp = ZRIdentifier_fromID(identifier, id);
		ZRTEST_ASSERT_PTR_NE(NULL, dp);
		ZRTEST_PRINTF("%lf\n", *dp);
		ZRTEST_ASSERT_DOUBLE_EQ(l, *dp);
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
		ZRIdentifier_intern(identifier, &ZRPTYPE_OBJECTP(&l));

	l = 7;
	ZRTEST_ASSERT_INT_EQ(true, ZRIdentifier_contains(identifier, &ZRPTYPE_OBJECTP(&l)));
	ZRIdentifier_release(identifier, &ZRPTYPE_OBJECTP(&l));
	ZRTEST_ASSERT_INT_EQ(nb - 1, ZRIdentifier_nbObj(identifier));
	ZRTEST_ASSERT_INT_EQ(false, ZRIdentifier_contains(identifier, &ZRPTYPE_OBJECTP(&l)));

	ZRIdentifier_destroy(identifier);
}

MU_TEST(releaseID)
{
	ZRIDTEST_BEGIN();
	long nb = 100;
	long l;
	ZRIdentifier *identifier = CONFIG->fcreate();

	for (l = 0; l < nb; l++)
		ZRIdentifier_intern(identifier, &ZRPTYPE_OBJECTP(&l));

	l = 7;
	ZRID id = ZRIdentifier_getID(identifier, &ZRPTYPE_OBJECTP(&l));
	ZRTEST_ASSERT_INT_EQ(true, ZRIdentifier_contains(identifier, &ZRPTYPE_OBJECTP(&l)));
	ZRIdentifier_releaseID(identifier, id);
	ZRTEST_ASSERT_INT_EQ(nb - 1, ZRIdentifier_nbObj(identifier));
	ZRTEST_ASSERT_INT_EQ(false, ZRIdentifier_contains(identifier, &ZRPTYPE_OBJECTP(&l)));

	ZRIdentifier_destroy(identifier);
}

MU_TEST(releaseAll)
{
	ZRIDTEST_BEGIN();
	long nb = 100;
	long l;
	ZRIdentifier *identifier = CONFIG->fcreate();

	for (l = 0; l < nb; l++)
		ZRIdentifier_intern(identifier, &ZRPTYPE_OBJECTP(&l));

	l = 7;
	ZRTEST_ASSERT_INT_EQ(true, ZRIdentifier_contains(identifier, &ZRPTYPE_OBJECTP(&l)));
	ZRIdentifier_releaseAll(identifier);
	ZRTEST_ASSERT_INT_EQ(0, ZRIdentifier_nbObj(identifier));
	ZRTEST_ASSERT_INT_EQ(false, ZRIdentifier_contains(identifier, &ZRPTYPE_OBJECTP(&l)));

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

int IdentifierUTests(void)
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
