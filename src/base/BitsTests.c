#include <string.h>
#include <stdbool.h>

#include <zrlib/base/Bits.h>

#include "../main.h"

// ============================================================================

static ZRBits MEM_REF[] = { ZRBITS_MASK_1L | ZRBITS_MASK_1R, 0x000f, 0x8000, 0x003f };

#define NBBYTES sizeof(MEM_REF)
#define NBITEMS (NBBYTES / sizeof(ZRBits))

static ZRBits MEM[NBITEMS];

// ============================================================================

#define GUARD_BITS  (ZRBITS_MASK_1R | ZRBITS_MASK_1L)

static void testSetup()
{
	mainTestSetup();
	memcpy(MEM, MEM_REF, NBBYTES);
}

static int FUN_CMP_NAME(ZRBits *result, ZRBits *expected)
{
	int ret;

	while (*result != GUARD_BITS)
	{
		ret = memcmp(result, expected, sizeof(ZRBits));

		if (ret != 0)
			return ret;

		result++;
		expected++;
	}
	return 0;
}

static void FUN_PRINT_NAME(char * out, ZRBits *bits)
{
	char buff[150];
	strcat(out, "[");

	while (*bits != GUARD_BITS)
	{
		sprintf(buff, "%lX:", *bits);
		strcat(out, buff);
		bits++;
	}
	strcat(out, "]\n");
}

// ============================================================================
// SETBITS
// ============================================================================

MU_TEST(testSetBits)
{
	ZRBits source;
	//Zero local
	{
		ZRBits expected[] = { (ZRBits)0x5 << ZRBITS_NBOF - 4, GUARD_BITS };
		ZRBits local[] = { 0, GUARD_BITS };

		ZRBits const localRef = *local;

		ZRTEST_BEGIN();
		ZRBits_setBitsFromTheRight(local, 0, 4, (ZRBits)5);
		ZRTEST_END(MESSAGE_BUFF, local, expected);

		*local = localRef;
		ZRTEST_BEGIN();
		ZRBits_setBitsFromTheRight(local, 1, 3, (ZRBits)5);
		ZRTEST_END(MESSAGE_BUFF, local, expected);
	}
	//Full one local
	{
		ZRBits expected[] = { ZRBITS_MASK_FULL & ~((ZRBits)0x5 << ZRBITS_NBOF - 4), GUARD_BITS };
		ZRBits local[] = { ZRBITS_MASK_FULL, GUARD_BITS };

		ZRBits const localRef = *local;

		ZRTEST_BEGIN();
		ZRBits_setBitsFromTheRight(local, 0, 4, (ZRBits)0xa);
		ZRTEST_END(MESSAGE_BUFF, local, expected);

		*local = localRef;
		ZRTEST_BEGIN();
		ZRBits_setBitsFromTheRight(local, 1, 3, (ZRBits)0xa);
		ZRTEST_END(MESSAGE_BUFF, local, expected);
	}
	//Overlap
	{
		ZRBits expected[] = { 5, (ZRBits)7 << ZRBITS_NBOF - 3, GUARD_BITS };
		ZRBits local[] = { 0, 0, GUARD_BITS };

		ZRTEST_BEGIN();
		ZRBits_setBitsFromTheRight(local, ZRBITS_NBOF - 3, 6, (ZRBits)0x2f);
		ZRTEST_END(MESSAGE_BUFF, local, expected);
	}
}

// ============================================================================
// GETMASK
// ============================================================================

MU_TEST(testGetlMask)
{
	ZRBits (*getLMask[])(unsigned) =
	{ //
			ZRBits_getLMask_std,
#ifdef ZRBITS_INTRINSIC
		ZRBits_getLMask_i,
#endif
	};
	for (int fun_i = 0; fun_i < ZRCARRAY_NBOBJ(getLMask); fun_i++)
	{
		ZRBits expected[2] = { 0, GUARD_BITS };
		ZRBits local[2] = { 0, GUARD_BITS };
		char msg[100];

		for (int i = 0; i <= ZRBITS_NBOF; i++)
		{
			*local = getLMask[fun_i](i);
			sprintf(msg, "fun=%d", fun_i);
			ZRTEST_BEGIN_MSG(msg);
			ZRTEST_END(MESSAGE_BUFF, local, expected);
			*expected >>= 1;
			*expected |= ZRBITS_MASK_1L;
		}
	}
}

MU_TEST(testGetrMask)
{
	ZRBits expected[2] = { 0, GUARD_BITS };
	ZRBits local[2] = { 0, GUARD_BITS };

	for (int i = 0; i <= ZRBITS_NBOF; i++)
	{
		*local = ZRBits_getRMask(i);
		ZRTEST_BEGIN();
		ZRTEST_END(MESSAGE_BUFF, local, expected);
		*expected <<= 1;
		*expected |= ZRBITS_MASK_1R;
	}
}

// ============================================================================
// PACK
// ============================================================================

MU_TEST(testPack)
{
	ZRBits expected[2] = { ZRBITS_MASK_1L | (ZRBITS_MASK_1L >> 2) | (ZRBITS_MASK_1L >> 3), GUARD_BITS };
	ZRBits local[2] = { 0, GUARD_BITS };
	{
		unsigned char packet[] = { 1, 0, 1, 1 };
		ZRBits_cpack(local, 1, packet, sizeof(packet) / sizeof(*packet));
		ZRTEST_BEGIN_MSG("1x4");
		ZRTEST_END(MESSAGE_BUFF, local, expected);
	}
	{
		unsigned char packet[] = { 2, 3 };
		ZRBits_cpack(local, 2, packet, sizeof(packet) / sizeof(*packet));
		ZRTEST_BEGIN_MSG("2x2");
		ZRTEST_END(MESSAGE_BUFF, local, expected);
	}
	{
		unsigned char packet[] = { 11 };
		ZRBits_cpack(local, 4, packet, sizeof(packet) / sizeof(*packet));
		ZRTEST_BEGIN_MSG("4x1");
		ZRTEST_END(MESSAGE_BUFF, local, expected);
	}
	{
		ZRBits const part = (ZRBits)5 << ZRBITS_NBOF - 3;
		ZRBits expected[2] = { part | part >> 3 | part >> 6, GUARD_BITS };
		unsigned packet[] = { 5, 5, 5 };
		ZRBITS_PACK_ARRAY(local, 3, packet);
		ZRTEST_BEGIN_MSG("array 3x3");
		ZRTEST_END(MESSAGE_BUFF, local, expected);
	}
}

// ============================================================================
// 1POS
// ============================================================================

MU_TEST(test1RPos)
{
	size_t result;
	size_t const nbZRBits = 2;
	ZRBits local_a[][2] = { //
		{ ZRBITS_MASK_1L >> 1, 0 }, //
		{ ZRBITS_MASK_1L >> 1, ZRBITS_MASK_1R }, //
		};
	size_t pos_a[] = { //
		0, //
		1, //
		};
	size_t expected_a[] = { //
		ZRBITS_NBOF * 2 - 2, //
		ZRBITS_NBOF * 2 - 2, //
		};

	for (unsigned i = 0; i < ZRCARRAY_NBOBJ(expected_a); i++)
	{
		ZRBits *local = local_a[i];
		size_t expected = expected_a[i];

		ZRTEST_PRINTF("[%u] pos=%u expected=%u", i, (unsigned )pos_a[i], (unsigned )expected);
		result = ZRBits_1RPos(local, nbZRBits, pos_a[i]);

		if (result != expected)
		{
			sprintf(MESSAGE_BUFF, "%s, have %u", FUN_PREFIX, (unsigned)result);
			fputs(MESSAGE_BUFF, stderr);
			fflush(stderr);
			mu_fail(MESSAGE_BUFF);
		}
	}
}

MU_TEST(test1LPos)
{
	size_t result;
	size_t const nbZRBits = 2;
	ZRBits local_a[][2] = { //
		{ 0, ZRBITS_MASK_1R << 1 }, //
		{ ZRBITS_MASK_1L, ZRBITS_MASK_1R << 1 }, //
		};
	size_t pos_a[] = { //
		0, //
		1, //
		};
	size_t expected_a[] = { //
		ZRBITS_NBOF * 2 - 2, //
		ZRBITS_NBOF * 2 - 2, //
		};

	for (unsigned i = 0; i < ZRCARRAY_NBOBJ(expected_a); i++)
	{
		ZRBits *local = local_a[i];
		size_t expected = expected_a[i];

		ZRTEST_PRINTF("[%u] pos=%u expected=%u", i, (unsigned )pos_a[i], (unsigned )expected);
		result = ZRBits_1LPos(local, nbZRBits, pos_a[i]);

		if (result != expected)
		{
			sprintf(MESSAGE_BUFF, "%s, have %u", FUN_PREFIX, (unsigned)result);
			fputs(MESSAGE_BUFF, stderr);
			fflush(stderr);
			mu_fail(MESSAGE_BUFF);
		}
	}
}

// ============================================================================
// COPY
// ============================================================================

MU_TEST(testCopyOneInsideOne)
{
	ZRBits local[] = { ZRBITS_MASK_1L | (ZRBITS_MASK_1L >> 2) | (ZRBITS_MASK_1L >> 3) | ZRBITS_MASK_1R | ZRBITS_MASK_1R << 1, GUARD_BITS };
	ZRBits result[] = { 0, GUARD_BITS };
	ZRBits expected[] = { 0, GUARD_BITS };
	{
		result[0] = 5;
		expected[0] = result[0] | (*local & ZRBits_getLMask(3));
		ZRTEST_BEGIN_MSG("pos=0, nbBits=3, outPos=0");
		ZRBits_copy(local, 0, 3, result, 0);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
	{
		result[0] = 5;
		expected[0] = result[0] | ((*local & ZRBits_getRMask(3)) << (ZRBITS_NBOF - 3));
		ZRTEST_BEGIN_MSG("pos=*1 - 3, nbBits=3, outPos=0");
		ZRBits_copy(local, ZRBITS_NBOF - 3, 3, result, 0);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
	{
		result[0] = 5 | ZRBITS_MASK_1L;
		expected[0] = result[0] | (*local & ZRBits_getLMask(3)) >> 1;
		ZRTEST_BEGIN_MSG("pos=0, nbBits=3, outPos=1");
		ZRBits_copy(local, 0, 3, result, 1);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
	{
		result[0] = 5 | ZRBITS_MASK_1L;
		expected[0] = result[0] | ((*local & ZRBits_getRMask(3)) << (ZRBITS_NBOF - 4));
		ZRTEST_BEGIN_MSG("pos=*1 - 3, nbBits=3, outPos=1");
		ZRBits_copy(local, ZRBITS_NBOF - 3, 3, result, 1);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
}

MU_TEST(testCopyOneOverTwo)
{
	ZRBits local[] = { ZRBITS_MASK_1R | ZRBITS_MASK_1R << 2, ZRBITS_MASK_1L | ZRBITS_MASK_1L >> 1 | ZRBITS_MASK_1R << 3, GUARD_BITS };
	ZRBits result[] = { 0, 0, GUARD_BITS };
	ZRBits expected[] = { 0, 0, GUARD_BITS };
	{
		result[0] = ZRBITS_MASK_1L >> 5;
		expected[0] = result[0] | ZRBITS_MASK_1L | (ZRBITS_MASK_1L >> 2) | (ZRBITS_MASK_1L >> 3) | (ZRBITS_MASK_1L >> 4);
		ZRTEST_BEGIN_MSG("pos=*1-3, nbBits=5, outPos=0");
		ZRBits_copy(local, ZRBITS_NBOF - 3, 5, result, 0);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
	{
		result[0] = 1 | ZRBITS_MASK_1L >> 6;
		expected[0] = (ZRBITS_MASK_1L | (ZRBITS_MASK_1L >> 2) | (ZRBITS_MASK_1L >> 3) | (ZRBITS_MASK_1L >> 4)) >> 1;
		expected[0] |= result[0];
		ZRTEST_BEGIN_MSG("pos=*1-3, nbBits=5, outPos=1");
		ZRBits_copy(local, ZRBITS_NBOF - 3, 5, result, 1);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
	{
		result[0] = 1 | ZRBITS_MASK_1R << 5;
		expected[0] = result[0] | 0x17;
		ZRTEST_BEGIN_MSG("pos=*1-3, nbBits=5, outPos=-5");
		ZRBits_copy(local, ZRBITS_NBOF - 3, 5, result, ZRBITS_NBOF - 5);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
	{
		result[0] = 0;
		expected[0] = (ZRBits)0x17 << (ZRBITS_NBOF - 5) | ZRBITS_MASK_1R;
		ZRTEST_BEGIN_MSG("pos=*1-3, nbBits=*1, outPos=0");
		ZRBits_copy(local, ZRBITS_NBOF - 3, ZRBITS_NBOF, result, 0);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
}

MU_TEST(testCopyMore)
{
	ZRBits local[] = { 0, 0, 0, 0, GUARD_BITS };
	ZRBits result[] = { 0, 0, 0, 0, GUARD_BITS };
	ZRBits expected[] = { 0, 0, 0, 0, GUARD_BITS };

	local[0] = ZRBITS_MASK_1L | ZRBITS_MASK_1L >> 2 | ZRBITS_MASK_1R << 1 | ZRBITS_MASK_1R;
	local[1] = *local;
	local[2] = *local;
	local[3] = *local;

	{
		memcpy(expected, local, sizeof(ZRBits) * 4);
		ZRTEST_BEGIN_MSG("pos=0, nbBits=*4, outPos=0");
		ZRBits_copy(local, 0, ZRBITS_NBOF * 4, result, 0);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
	{
		memset(result, 0, sizeof(ZRBits) * 4);
		memset(expected, 0, sizeof(ZRBits) * 4);
		expected[0] = *local << (ZRBITS_NBOF - 2) | *local >> 2;
		expected[1] = *local << (ZRBITS_NBOF - 2) | ((*local >> 2) & ZRBits_getLMask(4));
		ZRTEST_BEGIN_MSG("pos=*1 - 2, nbBits=*1 + 4, outPos=0");
		ZRBits_copy(local, ZRBITS_NBOF - 2, ZRBITS_NBOF + 4, result, 0);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
	{
		memset(result, 0, sizeof(ZRBits) * 4);
		memset(expected, 0, sizeof(ZRBits) * 4);
		expected[0] = *local << (ZRBITS_NBOF - 2) | *local >> 2;
		expected[0] >>= 2;
		expected[0] |= ZRBITS_MASK_1L;
		expected[1] = *local << (ZRBITS_NBOF - 2) | ((*local >> 2) & ZRBits_getLMask(4));
		expected[1] >>= 2;
		expected[1] |= expected[0] << (ZRBITS_NBOF - 2);
		ZRTEST_BEGIN_MSG("pos=*1 - 2, nbBits=*1 + 4, outPos=2");
		result[0] = ZRBITS_MASK_1L;
		ZRBits_copy(local, ZRBITS_NBOF - 2, ZRBITS_NBOF + 4, result, 2);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
	{
		memset(result, 0, sizeof(ZRBits) * 4);
		memset(expected, 0, sizeof(ZRBits) * 4);
		expected[0] = *local << (ZRBITS_NBOF - 2) | *local >> 2;
		expected[1] = *local << (ZRBITS_NBOF - 2) | *local >> 2;
		expected[2] = *local << (ZRBITS_NBOF - 2) | ((*local >> 2) & ZRBits_getLMask(4));
		ZRTEST_BEGIN_MSG("pos=*1 - 2, nbBits=*2 + 4, outPos=0");
		ZRBits_copy(local, ZRBITS_NBOF - 2, ZRBITS_NBOF * 2 + 4, result, 0);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}

	{
		memset(result, 0, sizeof(ZRBits) * 4);
		memset(expected, 0, sizeof(ZRBits) * 4);
		expected[0] = *local >> 2;
		expected[1] = *local << (ZRBITS_NBOF - 2) | ((*local >> 2));
		expected[2] = *local << (ZRBITS_NBOF - 2) | ((*local >> 2) & ZRBits_getLMask(2));
		ZRTEST_BEGIN_MSG("pos=0, nbBits=*2, outPos=2");
		ZRBits_copy(local, 0, ZRBITS_NBOF * 2, result, 2);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
	{
		memset(result, 0, sizeof(ZRBits) * 4);
		memset(expected, 0, sizeof(ZRBits) * 4);
		ZRTEST_BEGIN_MSG("pos=0, nbBits=*2 + 2, outPos=2");
		expected[0] = *local >> 2;
		expected[1] = *local << (ZRBITS_NBOF - 2) | ((*local >> 2));
		expected[2] = *local << (ZRBITS_NBOF - 2) | ((*local >> 2) & ZRBits_getLMask(4));
		ZRBits_copy(local, 0, ZRBITS_NBOF * 2 + 2, result, 2);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}

	{
		memset(result, 0, sizeof(ZRBits) * 4);
		memset(expected, 0, sizeof(ZRBits) * 4);
		expected[0] = *local & ZRBits_getRMask(ZRBITS_NBOF - 2);
		expected[1] = *local;
		expected[2] = *local & ZRBits_getLMask(2);
		ZRTEST_BEGIN_MSG("pos=2, nbBits=*2, outPos=2");
		ZRBits_copy(local, 2, ZRBITS_NBOF * 2, result, 2);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
	{
		memset(result, 0, sizeof(ZRBits) * 4);
		memset(expected, 0, sizeof(ZRBits) * 4);
		expected[0] = *local & ZRBits_getRMask(ZRBITS_NBOF - 2);
		expected[1] = *local;
		expected[2] = *local & ZRBits_getLMask(4);
		ZRTEST_BEGIN_MSG("pos=2, nbBits=*2 + 2, outPos=2");
		ZRBits_copy(local, 2, ZRBITS_NBOF * 2 + 2, result, 2);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
}

// ============================================================================
// GETBIT
// ============================================================================
MU_TEST(testGetBit)
{
	mu_check(ZRBits_getBit(MEM + 0, 0) == true);
	mu_check(ZRBits_getBit(MEM + 0, 1) == false);
	mu_check(ZRBits_getBit(MEM+0, ZRBITS_NBOF - 1) == true);
	mu_check(ZRBits_getBit(MEM+0, ZRBITS_NBOF - 2) == false);
	mu_check(ZRBits_getBit(MEM + 0, 0) == true);
	mu_check(ZRBits_getBit(MEM + 0, 1) == false);
	mu_check(ZRBits_getBit(MEM+0, ZRBITS_NBOF - 1) == true);
	mu_check(ZRBits_getBit(MEM+0, ZRBITS_NBOF - 2) == false);

	mu_check(ZRBits_getBit(MEM+1, ZRBITS_NBOF - 4) == true);
	mu_check(ZRBits_getBit(MEM+0, 2*ZRBITS_NBOF - 4) == true);
	mu_check(ZRBits_getBit(MEM+1, ZRBITS_NBOF - 5) == false);
	mu_check(ZRBits_getBit(MEM+0, 2*ZRBITS_NBOF - 5) == false);
}

MU_TEST(testGetOneBits)
{
	ZRBits local[] = { //
		ZRBITS_MASK_1L | (ZRBITS_MASK_1L >> 2) | (ZRBITS_MASK_1L >> 3) | ZRBITS_MASK_1R, //
		ZRBITS_MASK_1L | (ZRBITS_MASK_1L >> 2), //
		ZRBITS_MASK_1L, //
		GUARD_BITS //
		};
	ZRBits result[] = { 0, GUARD_BITS };
	ZRBits expected[] = { 0, GUARD_BITS };

	{
		result[0] = 0;
		expected[0] = ZRBITS_MASK_1L | (ZRBITS_MASK_1L >> 2);
		ZRTEST_BEGIN();
		ZRBits_getBits(local, 0, 3, result);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
	{
		result[0] = 0;
		expected[0] = (ZRBITS_MASK_1L >> 2);
		ZRTEST_BEGIN();
		ZRBits_getBits(local, ZRBITS_NBOF - 3, 3, result);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
	{
		result[0] = 0;
		expected[0] = ZRBITS_MASK_1L | (ZRBITS_MASK_1L >> 1);
		ZRTEST_BEGIN();
		ZRBits_getBits(local, ZRBITS_NBOF - 1, 2, result);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
	{
		result[0] = 0;
		expected[0] = (ZRBITS_MASK_1L | (ZRBITS_MASK_1L >> 1) | (ZRBITS_MASK_1L >> 3)) >> 1;
		ZRTEST_BEGIN();
		ZRBits_getBits(local, ZRBITS_NBOF - 2, 5, result);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
}

MU_TEST(testGetBits)
{
	ZRBits rpart = 0x5;
	ZRBits lpart = rpart << (ZRBITS_NBOF - 3);
	ZRBits local[] = { //
		rpart | lpart, //
		ZRBITS_MASK_1L | ZRBITS_MASK_1R, //
		ZRBITS_MASK_1L | rpart, //
		GUARD_BITS //
		};
	ZRBits result[] = { 0, 0, 0, GUARD_BITS };
	ZRBits expected[] = { 0, 0, 0, GUARD_BITS };
	{
		expected[0] = ((rpart | lpart) << 1) | ZRBITS_MASK_1R;
		expected[1] = 0x3;
		expected[2] = rpart << 1;
		ZRTEST_BEGIN();
		ZRBits_getBits(local, 1, (ZRBITS_NBOF * 3) - 1, result);
		ZRTEST_END(MESSAGE_BUFF, result, expected);
	}
}

// ============================================================================
// SHIFT
// ============================================================================

MU_TEST(testInArrayShift)
{
	{
		ZRBits expected[] = { 0x2345, 0, GUARD_BITS };
		ZRBits local[] = { 0x1234, 0x2345, GUARD_BITS };

		ZRTEST_BEGIN();
		ZRBits_inArrayShift(local, 2, sizeof(*local) * CHAR_BIT, false);
		ZRTEST_END(MESSAGE_BUFF, local, expected);
	}
	{
		ZRBits expected[] = { 0, 0x1234, GUARD_BITS };
		ZRBits local[] = { 0x1234, 0x2345, GUARD_BITS };

		ZRTEST_BEGIN();
		ZRBits_inArrayShift(local, 2, sizeof(*local) * CHAR_BIT, true);
		ZRTEST_END(MESSAGE_BUFF, local, expected);
	}
	{
		ZRBits expected[] = { ZRBITS_MASK_1L >> 1, ZRBITS_MASK_1R << 2, GUARD_BITS };
		ZRBits local[] = { ZRBITS_MASK_1L >> 2, ZRBITS_MASK_1R << 1, GUARD_BITS };

		ZRTEST_BEGIN();
		ZRBits_inArrayLShift(local, 2, 1);
		ZRTEST_END(MESSAGE_BUFF, local, expected);
	}
	{
		ZRBits expected[] = { ZRBITS_MASK_1L >> 3, ZRBITS_MASK_1R, GUARD_BITS };
		ZRBits local[] = { ZRBITS_MASK_1L >> 2, ZRBITS_MASK_1R << 1, GUARD_BITS };

		ZRTEST_BEGIN();
		ZRBits_inArrayRShift(local, 2, 1);
		ZRTEST_END(MESSAGE_BUFF, local, expected);
	}
}

// ============================================================================
// SEARCH
// ============================================================================

MU_TEST(testSearch)
{
	ZRBits *search;
	size_t pos;
	{
		const size_t nb = 10;
		const size_t nbBits = 4;
		const size_t offsetBloc = 1;
		const size_t offsetBits = 3;
		ZRBits haystack[nb];

		ZRARRAYOP_FILL(haystack, sizeof(ZRBits), nb, &(ZRBits ) { 0 });
		ZRBits_setBitsFromTheRight(haystack + offsetBloc, offsetBits, nbBits, ZRBits_getRMask(nbBits));

		ZRTEST_PRINTF("nb=%zu nbBits=%zu", nb, nbBits);
		ZRBits_searchFixedPattern(haystack, 0, nb, nbBits, &search, &pos);
		mu_check(search == haystack + offsetBloc);
		mu_check(pos == offsetBits);
	}
	{
		const size_t nb = 10;
		const size_t nbBits = 1;
		const size_t offsetBloc = 8;
		const size_t offsetBits = 3;
		ZRBits haystack[nb];

		ZRARRAYOP_FILL(haystack, sizeof(ZRBits), nb, &(ZRBits ) { 0 });
		ZRBits_setBitsFromTheRight(haystack + offsetBloc, offsetBits, nbBits, ZRBits_getRMask(nbBits));

		ZRTEST_PRINTF("nb=%zu nbBits=%zu", nb, nbBits);
		ZRBits_searchFixedPattern(haystack, 0, nb, nbBits, &search, &pos);
		mu_check(search == haystack + offsetBloc);
		mu_check(pos == offsetBits);
	}
	{
		const size_t nb = 10;
		const size_t nbBits = ZRBITS_NBOF;
		const size_t offsetBloc = 5;
		const size_t offsetBits = 0;
		ZRBits haystack[nb];

		ZRARRAYOP_FILL(haystack, sizeof(ZRBits), nb, &(ZRBits ) { 0 });
		ZRBits_setBitsFromTheRight(haystack + offsetBloc, offsetBits, nbBits, ZRBits_getRMask(nbBits));

		ZRTEST_PRINTF("nb=%zu nbBits=%zu", nb, nbBits);
		ZRBits_searchFixedPattern(haystack, 0, nb, nbBits, &search, &pos);
		mu_check(search == haystack + offsetBloc);
		mu_check(pos == offsetBits);
	}
	{
		const size_t nb = 10;
		const size_t nbBits = ZRBITS_NBOF;
		const size_t offsetBloc = 5;
		const size_t offsetBits = 13;
		ZRBits haystack[nb];

		ZRARRAYOP_FILL(haystack, sizeof(ZRBits), nb, &(ZRBits ) { 0 });
		ZRBits_setBitsFromTheRight(haystack + offsetBloc, offsetBits, nbBits, ZRBits_getRMask(nbBits));

		ZRTEST_PRINTF("nb=%zu nbBits=%zu", nb, nbBits);
		ZRBits_searchFixedPattern(haystack, 0, nb, nbBits, &search, &pos);
		mu_check(search == haystack + offsetBloc);
		mu_check(pos == offsetBits);
	}
	{
		const size_t nb = 10;
		const size_t nbBits = ZRBITS_NBOF + 1;
		const size_t offsetBloc = 2;
		const size_t offsetBits = 13;
		ZRBits haystack[nb];

		ZRARRAYOP_FILL(haystack, sizeof(ZRBits), nb, &(ZRBits ) { 0 });
		ZRBits_fill(haystack + offsetBloc, offsetBits, nbBits);

		ZRTEST_PRINTF("nb=%zu nbBits=%zu", nb, nbBits);
		ZRBits_searchFixedPattern(haystack, 0, nb, nbBits, &search, &pos);
		mu_check(search == haystack + offsetBloc);
		mu_check(pos == offsetBits);
	}
}

// ============================================================================
// TESTS
// ============================================================================

MU_TEST_SUITE(AllTests)
{
	MU_RUN_TEST(testGetlMask);
	MU_RUN_TEST(testGetrMask);
	MU_RUN_TEST(test1LPos);
	MU_RUN_TEST(test1RPos);
	MU_RUN_TEST(testSetBits);
	MU_RUN_TEST(testPack);
	MU_RUN_TEST(testGetBit);
	MU_RUN_TEST(testCopyOneInsideOne);
	MU_RUN_TEST(testCopyOneOverTwo);
	MU_RUN_TEST(testCopyMore);
	MU_RUN_TEST(testGetOneBits);
	MU_RUN_TEST(testGetBits);
	MU_RUN_TEST(testInArrayShift);
	MU_RUN_TEST(testSearch);
}

int BitsTests(void)
{
	puts(__FUNCTION__);
	MU_SUITE_CONFIGURE(testSetup, NULL);
	MU_RUN_SUITE(AllTests);
	MU_REPORT()
	;
	return minunit_status;
}
