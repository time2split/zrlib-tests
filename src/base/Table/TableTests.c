#include "../../main.h"

#include <zrlib/base/Table/VectorTable.h>

static ZRInitInfos_t INITINFOS;

#define CONFIG_XLIST() \
	X(VectorTable_column) \
	X(VectorTable_object) \
	X(VectorTable_both) \

#define ZRTABLETEST_BEGIN() ZRTEST_PRINTF("(%s) ", CONFIG.name)

typedef struct
{
	char *name;
	ZRTable* (*fcreate)(ZRObjAlignInfos *ainfos, size_t nb);
} Config;

static Config CONFIG;

static ZRTable* create_VectorTable_object(ZRObjAlignInfos *ainfos, size_t nb)
{
	ZRVectorTableIInfos(INITINFOS, ainfos, nb);
	ZRVectorTableIInfos_mode(INITINFOS, ZRVectorTableModeE_object);
	return ZRVectorTable_new(INITINFOS);
}

static ZRTable* create_VectorTable_column(ZRObjAlignInfos *ainfos, size_t nb)
{
	ZRVectorTableIInfos(INITINFOS, ainfos, nb);
	ZRVectorTableIInfos_mode(INITINFOS, ZRVectorTableModeE_column);
	return ZRVectorTable_new(INITINFOS);
}

static ZRTable* create_VectorTable_both(ZRObjAlignInfos *ainfos, size_t nb)
{
	ZRVectorTableIInfos(INITINFOS, ainfos, nb);
	ZRVectorTableIInfos_mode(INITINFOS, ZRVectorTableModeE_both);
	return ZRVectorTable_new(INITINFOS);
}

#define X(NAME) # NAME, create_ ## NAME,
static Config CONFIGS[] = { CONFIG_XLIST() };
#undef X

// ============================================================================

static ZRTable* testTable(void)
{
	ZRObjAlignInfos ainfos[] = { //
		ZRTYPE_OBJALIGNINFOS(char*),
		ZRTYPE_OBJALIGNINFOS(double),
		ZRTYPE_OBJALIGNINFOS(char),
		ZRTYPE_OBJALIGNINFOS(long),
		ZROBJALIGNINFOS_DEF0()
		};
	size_t const nb = ZRCARRAY_NBOBJ(ainfos) - 1;
	ZRStruct_bestOffsets(nb, ainfos);
	return CONFIG.fcreate(ainfos, nb);
}

static void testSetup()
{
	mainTestSetup();
}

MU_TEST(test_column)
{
	ZRTABLETEST_BEGIN();
	ZRTable *table = testTable();

	ZRTable_reserve_nb(table, 0, 1);
	ZRTable_set_column_nb(table, 0, 0, 1, "first");
	ZRTable_set_column_nb(table, 0, 1, 1, (double[] ) { 1.25 });
	ZRTable_set_column_nb(table, 0, 2, 1, (char[] ) { 'f' });
	ZRTable_set_column_nb(table, 0, 3, 1, (long[] ) { 11111 });

	ZRTEST_ASSERT_STR_EQ("first", ZRTable_get_column(table, 0, 0));
	ZRTEST_ASSERT_DOUBLE_EQ(1.25, *(double* )ZRTable_get_column(table, 0, 1));
	ZRTEST_ASSERT_CHAR_EQ('f', *(char* )ZRTable_get_column(table, 0, 2));
	ZRTEST_ASSERT_INT_EQ(11111, *(long* )ZRTable_get_column(table, 0, 3));
	ZRTable_destroy(table);
}

MU_TEST(testColumn)
{
	ZRTABLETEST_BEGIN();
	ZRTable *table = testTable();
	const size_t nb = 20;
	const size_t offset = 2;
	const size_t nbOp = 13;

	ZRTable_reserve_nb(table, 0, nb);
	ZRTEST_ASSERT_INT_EQ(nb, ZRTABLE_NBLINES(table));

	for (long i = 0; i < nb; i++)
		ZRTable_set_column_nb(table, i, 3, 1, &i);

	long *col = ZRTable_getColumn(table, offset, 3, nbOp);

	for (long i = 0; i < nbOp; i++)
		ZRTEST_ASSERT_INT_EQ(i + offset, col[i]);

	ZRTable_getColumn_end(table, col);
	ZRTable_destroy(table);
}

MU_TEST(testSet)
{
	ZRTABLETEST_BEGIN();
	ZRTable *table = testTable();
	struct dataS
	{
		char *s;
		double d;
		char c;
		long l;
	} src[] = { //
		{ "first", 1.1, 'A', 1000 },
		{ "second", 2.2, 'B', 2000 },
		{ "third", 3.3, 'C', 3000 },
		}, res[3];
#	define MEMBERS s,d,c,l
	char tmp[ZRTABLE_OBJINFOS(table).size * 10];
	ZRObjAlignInfos
	dataS_infos[] =
	{	ZRSTRUCT_OAI_E(struct dataS,MEMBERS)};
	size_t nb = ZRCARRAY_NBOBJ(src);

	ZRTable_reserve_nb(table, 0, nb);
	ZRTable_set_oai_nb(table, 0, dataS_infos, nb, src);

	for (int max = 1; max < 3; max++)
	{
		for (int min = 0; min < max; min++)
		{
			ZRTable_cpy_nb(table, min, max - min, tmp);
			ZRTable_cpy_oai_nb(table, min, dataS_infos, max - min, res);

			for (int i = min; i < max; i++)
			{
				ZRTEST_PRINTF("min=%d max=%d i=%d\n", min, max, i);
				ZRTEST_ASSERT_STR_EQ(src[i].s, *(char**)ZRARRAYOP_GET(tmp,ZRTABLE_OBJINFOS(table).size,i,table->ainfos[0].offset));
				ZRTEST_ASSERT_DOUBLE_EQ(src[i].d, *(double*)ZRARRAYOP_GET(tmp,ZRTABLE_OBJINFOS(table).size,i,table->ainfos[1].offset));
				ZRTEST_ASSERT_CHAR_EQ(src[i].c, *(char*)ZRARRAYOP_GET(tmp,ZRTABLE_OBJINFOS(table).size,i,table->ainfos[2].offset));
				ZRTEST_ASSERT_INT_EQ(src[i].l, *(int*)ZRARRAYOP_GET(tmp,ZRTABLE_OBJINFOS(table).size,i,table->ainfos[3].offset));
				ZRTEST_ASSERT_STR_EQ(src[i].s, res[i].s);
				ZRTEST_ASSERT_DOUBLE_EQ(src[i].d, res[i].d);
				ZRTEST_ASSERT_CHAR_EQ(src[i].c, res[i].c);
				ZRTEST_ASSERT_INT_EQ(src[i].l, res[i].l);
			}
		}
	}
	ZRTable_destroy(table);
#	undef MEMBERS
}

MU_TEST(testSet_column)
{
	ZRTABLETEST_BEGIN();
	size_t const nb = 20;
	size_t const offset = 2;
	size_t const nbOp = 15;
	size_t const column = 1;
	double d[nb];

	for (size_t i = 0; i < nb; i++)
		d[i] = 1.1 * i;

	ZRTable *table = testTable();
	ZRTable_reserve_nb(table, 0, nb);
	ZRTable_set_column_nb(table, offset, column, nbOp, &d[offset]);

	for (size_t i = 0; i < nbOp; i++)
	{
		ZRTEST_PRINTF("i=%ld\n", i);
		ZRTEST_ASSERT_DOUBLE_EQ((offset + i) * 1.1, *(double* )ZRTable_get_column(table, offset + i, column));
	}
	ZRTable_destroy(table);
}

MU_TEST(testDelete)
{
	ZRTABLETEST_BEGIN();
	size_t const nb = 20;
	size_t const offset = 2;
	size_t const nbOp = 10;
	size_t const column = 3;
	size_t const mul = 100;
	long l[nb];

	for (size_t i = 0; i < nb; i++)
		l[i] = mul * i;

	ZRTable *table = testTable();
	ZRTable_reserve_nb(table, 0, nb);
	ZRTEST_ASSERT_INT_EQ(nb, ZRTABLE_NBLINES(table));
	ZRTable_set_column_nb(table, 0, column, nb, l);
	ZRTable_delete_nb(table, offset, nbOp);
	ZRTEST_ASSERT_INT_EQ(nb - nbOp, ZRTABLE_NBLINES(table));

	for (size_t i = 0; i < offset; i++)
	{
		ZRTEST_PRINTF("i=%ld\n", i);
		ZRTEST_ASSERT_DOUBLE_EQ(mul * i, *(long* )ZRTable_get_column(table, i, column));
	}
	for (size_t i = offset; i < nb - nbOp; i++)
	{
		ZRTEST_PRINTF("i=%ld\n", i);
		ZRTEST_ASSERT_DOUBLE_EQ(mul * (i + nbOp), *(long* )ZRTable_get_column(table, i, column));
	}
	ZRTable_destroy(table);
}

// ============================================================================
// TESTS
// ============================================================================

MU_TEST_SUITE(AllTests)
{
	MU_RUN_TEST(test_column);
	MU_RUN_TEST(testColumn);
	MU_RUN_TEST(testSet);
	MU_RUN_TEST(testSet_column);
	MU_RUN_TEST(testDelete);
}

int TableTests(void)
{
	puts(__FUNCTION__);
	MU_SUITE_CONFIGURE(testSetup, NULL);

	for (size_t i = 0; i < ZRCARRAY_NBOBJ(CONFIGS); i++)
	{
		CONFIG = CONFIGS[i];
		MU_RUN_SUITE(AllTests);
	}
	MU_REPORT()
				;
	return minunit_status;
}
