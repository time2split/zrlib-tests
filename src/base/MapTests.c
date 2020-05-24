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

#define XLIST \
	X(HashTable) \
	X(VectorMap) \
	X(VectorMapEq)

// ============================================================================

static size_t fhash(char *key)
{
	return (size_t)*key;
}

static ZRMap* HashTable_create(size_t keySize, size_t keyA, size_t objSize, size_t objA)
{
	size_t (*fhash_a[])(void*) =
	{	(fhash_t) fhash };
	return ZRHashTable_create(keySize, keyA, objSize, objA, fhash_a, ZRCARRAY_NBOBJ(fhash_a), NULL, ALLOCATOR);
}

int charcmp(void *a, void *b)
{
	return *(char*)a - *(char*)b;
}

static ZRMap* VectorMap_create(size_t keySize, size_t keyA, size_t objSize, size_t objA)
{
	return ZRVectorMap_create(keySize, keyA, objSize, objA, charcmp, NULL, ALLOCATOR, ZRVectorMap_modeOrder);
}

static ZRMap* VectorMapEq_create(size_t keySize, size_t keyA, size_t objSize, size_t objA)
{
	return ZRVectorMap_create(keySize, keyA, objSize, objA, charcmp, NULL, ALLOCATOR, ZRVectorMap_modeEq);
}

#define X(item) #item, ZRCONCAT(item, _create),
struct
{
	char *name;
	fmap_create fmap_create;
} TEST_CONFIG[] = { XLIST };
#undef X

// ============================================================================

MU_TEST(testGet)
{
	size_t const nb = 15;

	for (size_t c_i = 0; c_i < ZRCARRAY_NBOBJ(TEST_CONFIG); c_i++)
	{
		ZRMap *map = TEST_CONFIG[c_i].fmap_create(ZRTYPE_SIZE_ALIGNMENT(char), ZRTYPE_SIZE_ALIGNMENT(int));

		char key = 'a';
		int val = 0;

		for (int i = 0; i < 10; i++)
		{
			ZRMap_put(map, &key, &val);
			key++;
			val += 115;
		}
		ZRTEST_ASSERT_INT_EQ(0, *(int* )ZRMap_get(map, &((char ) { 'a' } )));
		ZRTEST_ASSERT_INT_EQ(115, *(int* )ZRMap_get(map, &((char ) { 'b' } )));
		ZRTEST_ASSERT_INT_EQ(230, *(int* )ZRMap_get(map, &((char ) { 'c' } )));

		ZRTEST_ASSERT_INT_EQ(false, ZRMap_delete(map, &((char ) { 'Z' } )) );

		ZRTEST_ASSERT_INT_EQ(true, ZRMap_delete(map, &((char ) { 'd' } )));
		ZRTEST_ASSERT_PTR_EQ(NULL, ZRMap_get(map, &((char ) { 'd' } )));

		ZRMap_destroy(map);
	}
}

// ============================================================================
// TESTS
// ============================================================================

MU_TEST_SUITE( AllTests)
{
	MU_RUN_TEST(testGet);
}

int MapTests(void)
{
	puts(__FUNCTION__);

	ALLOCATOR = malloc(sizeof(*ALLOCATOR));
	ZRCAllocator_init(ALLOCATOR);

	MU_RUN_SUITE(AllTests);
	MU_REPORT()
				;

	free(ALLOCATOR);
	return minunit_status;
}
