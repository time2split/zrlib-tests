#include <stdlib.h>
#include <minunit/minunit.h>

#include <zrlib/base/MemoryPool/MemoryPool.h>
#include <zrlib/base/MemoryPool/MPoolDynamicStrategy.h>
#include <zrlib/base/Allocator/CAllocator.h>
#include "../main.h"

// ============================================================================

ZRAllocator *ALLOCATOR;

// ============================================================================

MU_TEST(testGet)
{
	size_t const nb = 15;
	ZRMemoryPool *pool = ZRMPoolDS_createBS(1, sizeof(double), ALLOCATOR);
	double *val = ZRMPool_reserve_nb(pool, nb);

	for (int i = 0; i < nb; i++)
		val[i] = i;

	ZRMPool_release_nb(pool, &val[1], 2);
	ZRMPoolDS_destroy(pool);
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
