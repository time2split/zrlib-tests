/**
 * @author zuri
 * @date dimanche 24 novembre 2019, 01:51:07 (UTC+0100)
 */
#define NDEBUG 1
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <zrlib/base/Bits/Bits.h>

int Bits_naiveVsIntrinsic(int argc, char **argv)
{
	const size_t nb = 100000000;
	const size_t nbBits = ZRBITS_NBOF - 2;
	clock_t time_naive = 0;
	clock_t time_intrinsic = 0;
	{
		clock_t first = clock();
		for (size_t i = 0; i < nb; i++)
		{
			ZRBits res= ZRBits_getLMask_std(nbBits);
		}
		clock_t end = clock();
		time_naive = end - first;
	}
	{
		clock_t first = clock();
		for (size_t i = 0; i < nb; i++)
		{
			ZRBits res= ZRBits_getLMask_i(nbBits);
		}
		clock_t end = clock();
		time_intrinsic= end - first;
	}
	printf("naive:%ld ; %LE intrinsic:%ld %LE\n", time_naive, (long double)time_naive/nb, time_intrinsic, (long double)time_intrinsic/nb);
	return 0;
}
