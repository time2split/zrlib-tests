#include <zrlib/base/struct.h>

#include <zrlib/lib/init.h>
#include <zrlib/base/Allocator/Allocator.h>
#include "../main.h"

// ============================================================================

static void testSetup()
{
	mainTestSetup();
}

// ============================================================================

MU_TEST(arithm)
{
	ZRTEST_BEGIN();
	ZRObjAlignInfos infos[] = { //
		ZROBJALIGNINFOS_DEF_AS(8, 5),
		ZROBJALIGNINFOS_DEF_AS(3, 1),
		ZROBJALIGNINFOS_DEF0(),
		};
	size_t const nb = ZRCARRAY_NBOBJ(infos) - 1;
	ZRStruct_makeOffsets_flags(nb, infos, ZRSTRUCT_FLAG_ARITHMETIC);

	ZRTEST_ASSERT_INT_EQ(0, infos[0].offset);
	ZRTEST_ASSERT_INT_EQ(8, infos[0].alignment);
	ZRTEST_ASSERT_INT_EQ(5, infos[0].size);

	ZRTEST_ASSERT_INT_EQ(6, infos[1].offset);
	ZRTEST_ASSERT_INT_EQ(3, infos[1].alignment);
	ZRTEST_ASSERT_INT_EQ(1, infos[1].size);

	ZRTEST_ASSERT_INT_EQ(0, infos[2].offset);
	ZRTEST_ASSERT_INT_EQ(24, infos[2].alignment);
	ZRTEST_ASSERT_INT_EQ(24, infos[2].size);
}

MU_TEST(arithmResize)
{
	ZRTEST_BEGIN();
	ZRObjAlignInfos infos[] = { //
		ZROBJALIGNINFOS_DEF_AS(8, 5),
		ZROBJALIGNINFOS_DEF_AS(3, 1),
		ZROBJALIGNINFOS_DEF0(),
		};
	size_t const nb = ZRCARRAY_NBOBJ(infos) - 1;
	ZRStruct_makeOffsets_flags(nb, infos, ZRSTRUCT_FLAG_ARITHMETIC | ZRSTRUCT_FLAG_RESIZE);

	ZRTEST_ASSERT_INT_EQ(0, infos[0].offset);
	ZRTEST_ASSERT_INT_EQ(8, infos[0].alignment);
	ZRTEST_ASSERT_INT_EQ(8, infos[0].size);

	ZRTEST_ASSERT_INT_EQ(9, infos[1].offset);
	ZRTEST_ASSERT_INT_EQ(3, infos[1].alignment);
	ZRTEST_ASSERT_INT_EQ(3, infos[1].size);

	ZRTEST_ASSERT_INT_EQ(0, infos[2].offset);
	ZRTEST_ASSERT_INT_EQ(24, infos[2].alignment);
	ZRTEST_ASSERT_INT_EQ(24, infos[2].size);
}

MU_TEST(ObjInfos_union)
{
	ZRTEST_BEGIN();
	unsigned flags = ZRSTRUCT_FLAG_ARITHMETIC;
	ZRObjInfos ret, a = ZROBJINFOS_DEF(2, 7),
		b = ZROBJINFOS_DEF(8, 17),
		c = ZROBJINFOS_DEF(24, 70),
		d = ZROBJINFOS_DEF(150, 1)
			;

	ret = ZRObjInfos_union_flags_l(flags, a, b);
	ZRTEST_ASSERT_INT_EQ(8, ret.alignment);
	ZRTEST_ASSERT_INT_EQ(17, ret.size);

	ret = ZRObjInfos_union_flags_l(flags, a, b, c);
	ZRTEST_ASSERT_INT_EQ(24, ret.alignment);
	ZRTEST_ASSERT_INT_EQ(70, ret.size);

	ret = ZRObjInfos_union_flags_l(flags, a, b, d);
	ZRTEST_ASSERT_INT_EQ(600, ret.alignment);
	ZRTEST_ASSERT_INT_EQ(17, ret.size);

	/* Array version */

	ret = ZROBJINFOS_UNION_FLAGS((ZRObjInfos[] ) { a, b }, 2, flags);
	ZRTEST_ASSERT_INT_EQ(8, ret.alignment);
	ZRTEST_ASSERT_INT_EQ(17, ret.size);

	ret = ZROBJINFOS_UNION_FLAGS((ZRObjInfos[] ) { a, b, c }, 3, flags);
	ZRTEST_ASSERT_INT_EQ(24, ret.alignment);
	ZRTEST_ASSERT_INT_EQ(70, ret.size);

	ret = ZROBJINFOS_UNION_FLAGS((ZRObjInfos[] ) { a, b, d }, 3, flags);
	ZRTEST_ASSERT_INT_EQ(600, ret.alignment);
	ZRTEST_ASSERT_INT_EQ(17, ret.size);
}

// ============================================================================
// TESTS
// ============================================================================

MU_TEST_SUITE( AllTests)
{
	MU_RUN_TEST(arithm);
	MU_RUN_TEST(arithmResize);
	MU_RUN_TEST(ObjInfos_union);
}

int structTests(void)
{
	puts(__FUNCTION__);
	MU_SUITE_CONFIGURE(testSetup, NULL);
	MU_RUN_SUITE(AllTests);
	MU_REPORT()
				;
	return minunit_status;
}
