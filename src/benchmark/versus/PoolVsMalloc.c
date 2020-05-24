/**
 * @author zuri
 * @date dimanche 24 novembre 2019, 01:54:11 (UTC+0100)
 */
#define NDEBUG 1
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <zrlib/base/macro.h>
#include <zrlib/base/Allocator/CAllocator.h>
#include <zrlib/base/MemoryPool/MPoolDynamicStrategy.h>
#include <zrlib/base/MemoryPool/MPoolReserve.h>

int PoolVsMalloc(int argc, char **argv)
{
	const size_t nb = 100;
	const size_t allocSize = 16;
	const size_t maxFreeBuckets = 10;
	const size_t nbObj = 1000;
	const size_t initialBSize = 10000;
	const size_t step = 90;

	ZRAllocator *allocator = malloc(sizeof(ZRAllocator));
	ZRCAllocator_init(allocator);
	struct
	{
		char *name;
		ZRMemoryPool *pool;
	} pools[] = { //
		"bits", ZRMPoolReserve_createSimple(allocSize, alignof(max_align_t), initialBSize, allocator, ZRMPoolReserveMode_bits), //
		"list", ZRMPoolReserve_createSimple(allocSize, alignof(max_align_t), initialBSize, allocator, ZRMPoolReserveMode_list), //
		"chunk", ZRMPoolReserve_createSimple(allocSize, alignof(max_align_t), initialBSize, allocator, ZRMPoolReserveMode_chunk), //
		"Dyn:chunk", ZRMPoolDS_create(initialBSize, maxFreeBuckets, allocSize, alignof(max_align_t), allocator), //
		};

	char *p[nbObj];

	{
		clock_t time = 0;
		clock_t time_alloc = 0;
		clock_t time_free = 0;
		clock_t first = clock();
		for (size_t i = 0; i < nb; i++)
		{
			clock_t time_a;

			time_a = clock();
			for (size_t j = 0; j < nbObj; j++)
				p[j] = malloc(allocSize);

			time_alloc += clock() - time_a;

			time_a = clock();
			for (size_t j = 0; j < nbObj; j++)
				free(p[j]);

			time_free += clock() - time_a;
		}
		clock_t end = clock();
		time = end - first;
		printf("naive:%10lu ; alloc:%10lu free:%10lu\n", time, time_alloc, time_free);
	}

	for (int pool_i = 0; pool_i < ZRCARRAY_NBOBJ(pools); pool_i++)
	{
		clock_t time = 0;
		clock_t time_alloc = 0;
		clock_t time_free = 0;

		ZRMemoryPool *pool = pools[pool_i].pool;
		char *name = pools[pool_i].name;

		clock_t first = clock();
		for (size_t i = 0; i < nb; i++)
		{
			clock_t time_a;

			time_a = clock();
			for (size_t j = 0; j < nbObj; j += step)
				p[j] = ZRMPool_reserve_nb(pool, step);

			time_alloc += clock() - time_a;

			for (size_t j = 0; j < nbObj; j += step)
			{
				if (p[j] == NULL)
				{
					fprintf(stderr, "%s: Can't allocate i=%lu j=%lu\n", name, i, j);
					exit(1);
				}
			}

			time_a = clock();
//			for (size_t j = 0; j < nbObj; j++)
//				ZRMPool_release_nb(pool, p[j], 1);

			ZRMPool_clean(pool);
			time_free += clock() - time_a;
		}
		clock_t end = clock();
		time = end - first;

		printf("%5s:%10lu ; alloc:%10lu free:%10lu\n", name, time, time_alloc, time_free);
		ZRMPool_destroy(pool);
	}
	free(allocator);
	return 0;
}
