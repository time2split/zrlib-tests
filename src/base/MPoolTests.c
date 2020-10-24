#include <stdlib.h>
#include <stdalign.h>
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
	return ZRMPoolDS_createBS(1, ZRTYPE_OBJINFOS(double), ALLOCATOR);
}

ZRMemoryPool* testGet_createReserveBits()
{
	return ZRMPoolReserve_createSimple(ZRTYPE_OBJINFOS(double), testGet_nb, ALLOCATOR, ZRMPoolReserveMode_bits);
}

ZRMemoryPool* testGet_createReserveChunk()
{
	return ZRMPoolReserve_createSimple(ZRTYPE_OBJINFOS(double), testGet_nb, ALLOCATOR, ZRMPoolReserveMode_chunk);
}

ZRMemoryPool* testGet_createReserveList()
{
	return ZRMPoolReserve_createSimple(ZRTYPE_OBJINFOS(double), testGet_nb, ALLOCATOR, ZRMPoolReserveMode_list);
}

MU_TEST(testGet)
{
	size_t const nb = testGet_nb;
	size_t const releaseNb = testGet_nb / 2;

	struct
	{
		ZRMemoryPool* (*create)(void);
		void (*destroy)(ZRMemoryPool*);
	} pools[] = { //
		{ testGet_createDynamic }, //
		{ testGet_createReserveBits }, //
		{ testGet_createReserveChunk }, //
		{ testGet_createReserveList }, //
		};

	for (size_t i = 0; i < ZRCARRAY_NBOBJ(pools); i++)
	{
		ZRMemoryPool *pool = pools[i].create();
		double *val = ZRMPool_reserve_nb(pool, nb);

		if(val == NULL)
			mu_fail("Error poll cannot allocate block");

		mu_assert_int_eq(nb, ZRMPool_areaNbBlocks(pool, val));

		for (int i = 0; i < nb; i++)
			val[i] = i;

		ZRMPool_release_nb(pool, val, releaseNb);
		mu_assert_int_eq(nb - releaseNb, ZRMPool_areaNbBlocks(pool, &val[releaseNb]));

		double *val2 = ZRMPool_reserve_nb(pool, releaseNb);

		if(val2 == NULL)
			mu_fail("Error poll cannot allocate block");

		ZRMPool_destroy(pool);
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
