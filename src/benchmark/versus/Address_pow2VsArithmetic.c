/**
 * @author zuri
 * @date dimanche 24 novembre 2019, 01:51:07 (UTC+0100)
 */
#define NDEBUG 1
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <zrlib/base/AddressOp.h>

#define test(TIME,NB,ALIGN,P,FUN)  \
{ \
	size_t _align = ALIGN; \
	TIME = 0; \
	for (size_t i = 0; i < NB; i++) \
	{ \
		clock_t first = clock(); \
		res = FUN(P, _align); \
		clock_t end = clock(); \
		TIME += end - first; \
		_align *= 2; \
		\
		if (_align == 0) \
			_align = ALIGN; \
			\
		res++; \
	} \
}

int Address_pow2VsArithmetic(int argc, char **argv)
{
	size_t const nb = 1000000;
	volatile size_t res;
	clock_t time_a;
	clock_t time_pow;

	size_t const align = 2;
	void *p = argv + 5;

	puts("alignUp test");
	test(time_a,nb,align,p,ZRAddressOp_alignUp_a);
	test(time_pow,nb,align,p,ZRAddressOp_alignUp_pow2);
	printf("arithmetic:%ld ; %LE pow:%ld %LE\n", time_a, (long double)time_a / nb, time_pow, (long double)time_pow / nb);

	puts("");

	puts("alignDown test");
	test(time_a,nb,align,p,ZRAddressOp_alignDown_a);
	test(time_pow,nb,align,p,ZRAddressOp_alignDown_pow2);
	printf("arithmetic:%ld ; %LE pow:%ld %LE\n", time_a, (long double)time_a / nb, time_pow, (long double)time_pow / nb);
	return 0;
}
