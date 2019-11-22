#include <stdlib.h>
#include <minunit/minunit.h>

#include <zrlib/base/Map/HashTable.h>
#include <zrlib/base/macro.h>
#include <zrlib/base/Allocator/CAllocator.h>
#include "../main.h"

// ============================================================================

ZRAllocator *ALLOCATOR;

// ============================================================================

size_t fhash(char *key)
{
	return (size_t)*key;
}

MU_TEST(testGet)
{
	size_t const nb = 15;
	size_t const keySize = sizeof(char);
	size_t const valueSize = sizeof(int);
	size_t (*fhash_a[])(void*) =
	{	(fhash_t) fhash };

	ZRMap *map = ZRHashTable_create(keySize, valueSize, ZRCARRAY_NBOBJ(fhash_a), fhash_a, NULL, NULL, ALLOCATOR);

	char key = 'a';
	int val = 0;

	for (int i = 0; i < 10; i++)
	{
		ZRMap_put(map, &key, &val);
		key++;
		val += 115;
	}
	mu_check(*(int* )ZRMap_get(map, &((char ) { 'a' } )) == 0);
	mu_check(*(int* )ZRMap_get(map, &((char ) { 'b' } )) == 115);
	mu_check(*(int* )ZRMap_get(map, &((char ) { 'c' } )) == 230);

	mu_check(ZRMap_delete(map, &((char ) { 'Z' } )) == false);

	mu_check(ZRMap_delete(map, &((char ) { 'd' } )) == true);
	mu_check(ZRMap_get(map, &((char ) { 'd' } )) == NULL);

	ZRHashTable_destroy(map);
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
