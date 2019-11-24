/**
 * @author zuri
 * @date dimanche 24 novembre 2019, 02:05:08 (UTC+0100)
 */
#define NDEBUG 1

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <zrlib/base/ReserveOp_bits.h>
#include <zrlib/base/ReserveOp_list.h>

int Reserve_listVsBits(int argc, char **argv)
{
	const size_t nb = 150;
	const size_t nbAvailable = 2;
	const size_t nbObj = 5000;
	clock_t time_bits = 0;
	clock_t time_list = 0;
	clock_t first;
	clock_t end;
	{
		struct DataS
		{
			char e;
			ZRReserveNextUnused nextUnused;
			long i;
		} data[nbObj];

		memset(data, 0, sizeof(*data) * nbObj);

		for (size_t i = 0; i < nb; i++)
		{
			for (size_t j = 0; j < nbObj; j += nbAvailable)
			{
				first = clock();
				size_t volatile pos = ZRRESERVEOPLIST_RESERVEFIRSTAVAILABLES(data, sizeof(*data), ZRCARRAY_NBOBJ(data), offsetof(struct DataS, nextUnused), nbAvailable);
				end = clock();
			}
			time_list += end - first;
			ZRRESERVEOPLIST_RELEASENB(data, sizeof(*data), ZRCARRAY_NBOBJ(data), 0, 0, nbObj);
		}
	}
	{
		ZRBits data[nbObj];
		memset(data, (int)ZRRESERVEOPBITS_BLOCKISEMPTY, sizeof(*data) * nbObj);

		for (size_t i = 0; i < nb; i++)
		{
			for (size_t j = 0; j < nbObj; j += nbAvailable)
			{
				first = clock();
				size_t volatile pos = ZRRESERVEOPBITS_RESERVEFIRSTAVAILABLES(data, ZRCARRAY_NBOBJ(data), nbAvailable);
				end = clock();
			}
			time_bits += end - first;
			ZRRESERVEOPBITS_RELEASENB(data, 0, nbObj);
		}
	}
	printf("list:%ld ; %LE bits:%ld %LE\n", time_list, (long double)time_list / nb, time_bits, (long double)time_bits / nb);
	return 0;
}
