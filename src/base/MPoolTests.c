#include <stdlib.h>
#include <minunit/minunit.h>

#include <zrlib/base/MemoryPool/MemoryPool.h>
#include <zrlib/base/MemoryPool/MPoolDynamicStrategy.h>
#include <zrlib/base/MemoryPool/MPoolReserve.h>
#include <zrlib/base/Allocator/CAllocator.h>
#include <zrlib/base/macro.h>
#include "../main.h"

// ============================================================================

ZRAllocator *ALLOCATOR;

// ============================================================================

size_t const testGet_nb = 15;

ZRMemoryPool* testGet_createDynamic(void)
{
	return ZRMPoolDS_createBS(1, sizeof(double), ALLOCATOR);
}

ZRMemoryPool* testGet_createReserve()
{
	return ZRMPoolReserve_create(sizeof(double), testGet_nb, ALLOCATOR);
}

MU_TEST(testGet)
{
	size_t const nb = testGet_nb;

	struct
	{
		ZRMemoryPool* (*create)(void);
		void (*destroy)(ZRMemoryPool*);
	} pools[] = { //
		{ testGet_createDynamic, ZRMPoolDS_destroy }, //
		{ testGet_createReserve, ZRMPoolReserve_destroy }, //
		};

	for (size_t i = 0; i < ZRCARRAY_NBOBJ(pools); i++)
	{
		ZRMemoryPool *pool = pools[i].create();
		double *val = ZRMPool_reserve_nb(pool, nb);

		for (int i = 0; i < nb; i++)
			val[i] = i;

		ZRMPool_release_nb(pool, &val[1], 2);
		pools[i].destroy(pool);
	}
}

// ============================================================================
// TESTS
// ============================================================================

MU_TEST_SUITE( AllTests)
{
	MU_RUN_TEST(testGet);
}

int MPoolTests(void)
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
