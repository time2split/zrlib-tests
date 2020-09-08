#include <stdlib.h>
#include <minunit/minunit.h>

#include <zrlib/base/Map/HashTable.h>
#include <zrlib/base/Map/VectorMap.h>
#include <zrlib/base/macro.h>
#include <zrlib/base/Allocator/CAllocator.h>
#include "../main.h"



// ============================================================================

ZRAllocator *ALLOCATOR;

typedef ZRMap* (*fmap_create)(size_t keySize, size_t keyA, size_t objSize, size_t objA);

// ============================================================================

static size_t fhash(void *key, void *map)
{
	return *(size_t*)key;
}

static ZRMap* HashTable_create(size_t keySize, size_t keyA, size_t objSize, size_t objA)
{
	zrfuhash fhash_a[] =
		{ fhash };

	return ZRHashTable_create(ZROBJINFOS_DEF(keyA, keySize), ZROBJINFOS_DEF(objA, objSize), fhash_a, ZRCARRAY_NBOBJ(fhash_a), NULL, ALLOCATOR);
}

int charcmp(void *a, void *b)
{
	return *(size_t*)a - *(size_t*)b;
}

static ZRMap* VectorMap_create(size_t keySize, size_t keyA, size_t objSize, size_t objA)
{
	return ZRVectorMap_create(keySize, keyA, objSize, objA, charcmp, NULL, ALLOCATOR, ZRVectorMap_modeOrder);
}

static ZRMap* VectorMapEq_create(size_t keySize, size_t keyA, size_t objSize, size_t objA)
{
	return ZRVectorMap_create(keySize, keyA, objSize, objA, charcmp, NULL, ALLOCATOR, ZRVectorMap_modeEq);
}

// ============================================================================

#define XLIST \
	X(HashTable) \
	X(VectorMap) \
	X(VectorMapEq)

#define X(item) #item, ZRCONCAT(item, _create),
static struct
{
	char *name;
	fmap_create fmap_create;
} TEST_CONFIG[] = { XLIST }, *CONFIG;
#undef X

#define ZRMAPTEST_BEGIN() ZRTEST_PRINTF("config: %d, ", (int)(CONFIG - TEST_CONFIG))


MU_TEST(testGet)
{
	ZRMAPTEST_BEGIN();
	ZRMap *map = CONFIG->fmap_create(ZRTYPE_SIZE_ALIGNMENT(size_t), ZRTYPE_SIZE_ALIGNMENT(int));
	size_t const nb = 15;

	size_t key = 'a';
	int val = 0;

	for (int i = 0; i < nb; i++)
	{
		ZRMap_put(map, &key, &val);
		key++;
		val += 115;
	}
	ZRTEST_ASSERT_INT_EQ(0, *(int* )ZRMap_get(map, &((size_t ) { 'a' } )));
	ZRTEST_ASSERT_INT_EQ(115, *(int* )ZRMap_get(map, &((size_t ) { 'b' } )));
	ZRTEST_ASSERT_INT_EQ(230, *(int* )ZRMap_get(map, &((size_t ) { 'c' } )));

	ZRTEST_ASSERT_INT_EQ(false, ZRMap_delete(map, &((size_t ) { 'Z' } )));

	ZRTEST_ASSERT_INT_EQ(true, ZRMap_delete(map, &((size_t ) { 'd' } )));
	ZRTEST_ASSERT_PTR_EQ(NULL, ZRMap_get(map, &((size_t ) { 'd'} )));

	ZRMap_destroy(map);
}

MU_TEST(testStress)
{
	ZRMAPTEST_BEGIN();
	ZRMap *map = CONFIG->fmap_create(ZRTYPE_SIZE_ALIGNMENT(size_t), ZRTYPE_SIZE_ALIGNMENT(int));
	size_t const nb = 10000;
	size_t const nb_rest = 15;
	int val = 0;

	for (size_t i = 0; i < nb; i++)
	{
		ZRMap_put(map, &i, &val);
		val++;
	}
	ZRTEST_ASSERT_INT_EQ(nb, ZRMAP_NBOBJ(map));

	for (size_t i = 0; i < nb - nb_rest; i++)
	{
		ZRMap_delete(map, &i);
	}
	ZRTEST_ASSERT_INT_EQ(nb_rest, ZRMAP_NBOBJ(map));
	ZRMap_destroy(map);
}

// ============================================================================
// TESTS
// ============================================================================

MU_TEST_SUITE( AllTests)
{
	MU_RUN_TEST(testGet);
	MU_RUN_TEST(testStress);
}

int MapTests(void)
{
	puts(__FUNCTION__);

	ALLOCATOR = malloc(sizeof(*ALLOCATOR));
	ZRCAllocator_init(ALLOCATOR);

	for (size_t c_i = 0; c_i < ZRCARRAY_NBOBJ(TEST_CONFIG); c_i++)
	{
		CONFIG = &TEST_CONFIG[c_i];
		MU_RUN_SUITE(AllTests);
	}
	MU_REPORT()
				;

	free(ALLOCATOR);
	return minunit_status;
}
