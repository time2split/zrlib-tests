/**
 * @author zuri
 * @date dimanche 24 novembre 2019, 01:54:11 (UTC+0100)
 */
#define NDEBUG 1
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <zrlib/base/Allocator/CAllocator.h>
#include <zrlib/base/MemoryPool/MPoolDynamicStrategy.h>

int PoolVsMalloc(int argc, char **argv)
{
	const size_t nb = 100;
	const size_t allocSize = 16;
	const size_t maxFreeBuckets = 10;
	const size_t nbObj = 10000;
	const size_t initialBSize = 100;

	ZRAllocator *allocator = malloc(sizeof(ZRAllocator));
	ZRCAllocator_init(allocator);
	ZRMemoryPool *pool = ZRMPoolDS_create(initialBSize, maxFreeBuckets, allocSize, allocator);
	char *p[nbObj];
	clock_t time_naive = 0;
	clock_t time_intrinsic = 0;
	{
		clock_t first = clock();
		for (size_t i = 0; i < nb; i++)
		{
			for(size_t j = 0; j < nbObj; j++)
			p[j] = malloc(allocSize);

			for(size_t j = 0; j < nbObj; j++)
			free(p[j]);
		}
		clock_t end = clock();
		time_naive = end - first;
	}
	{
		clock_t first = clock();
		for (size_t i = 0; i < nb; i++)
		{
			for(size_t j = 0; j < nbObj; j++)
			{
				p[j] = ZRMPool_reserve(pool);
			}

			for(size_t j = 0; j < nbObj; j++)
			ZRMPool_release(pool, p[j]);
//			ZRMPool_release_nb(pool, p[0], nbObj);
		}
		clock_t end = clock();
		time_intrinsic = end - first;
	}

	printf("naive:%ld ; %LE pool:%ld %LE\n", time_naive, (long double)time_naive / nb, time_intrinsic, (long double)time_intrinsic / nb);
	ZRMPoolDS_destroy(pool);
	free(allocator);
	return 0;
}
