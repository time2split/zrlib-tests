#include <string.h>
#include <minunit/minunit.h>

#include <zrlib/base/MemoryOp.h>
#include "../main.h"

// ============================================================================

#define TYPE char

static TYPE MEM_REF[] =
	{ 1, 2, 3, 4, 5 };

#define NBBYTES sizeof(MEM_REF)
#define NBITEMS (NBBYTES / sizeof(TYPE))

static TYPE MEM[NBITEMS];
static char * const END = (char*) (MEM + NBITEMS);

#define LOCAL_ZRTEST_RESULT(prefix) \
	ZRTEST_RESULT(prefix, MESSAGE_BUFF, MEM, result)

#define LOCAL_ZRTEST_END() \
	ZRTEST_END(MESSAGE_BUFF, MEM, result)

// ============================================================================

static void testSetup()
{
	mainTestSetup();
	memcpy(MEM, MEM_REF, NBBYTES);
}

static int FUN_CMP_NAME(void* result, void *expected)
{
	return memcmp(result, expected, NBBYTES);
}

static void FUN_PRINT_NAME(char * out, TYPE tab[NBITEMS])
{
	char buff[20];
	strcat(out, "[");

	for (int i = 0; i < NBITEMS; i++)
	{
		sprintf(buff, "%s%d", (i ? "," : ""), tab[i]);
		strcat(out, buff);
	}
	strcat(out, "]\n");
}

// ============================================================================
// SHIFT
// ============================================================================

static void testShift(const char *prefix, TYPE result[], size_t shift, bool toTheRight)
{
	testSetup();

	ZRMemoryOp_shift(MEM, END, shift, toTheRight);
	LOCAL_ZRTEST_RESULT(prefix);
}

MU_TEST(testShift_left)
{
	TYPE results[][NBITEMS] =
		{
			{ 1, 2, 3, 4, 5 },
			{ 2, 3, 4, 5, 5 },
			{ 3, 4, 5, 4, 5 },
			{ 4, 5, 3, 4, 5 },
			{ 5, 2, 3, 4, 5 },
			{ 1, 2, 3, 4, 5 } };
	char prefix[100];

	for (int i = 0; i < NBITEMS; i++)
	{
		sprintf(prefix, "%s %d: ", __FUNCTION__, i);
		testShift(prefix, results[i], i * sizeof(TYPE), false);
	}
}

MU_TEST(testShift_right)
{
	TYPE results[][NBITEMS] =
		{
			{ 1, 2, 3, 4, 5 },
			{ 1, 1, 2, 3, 4 },
			{ 1, 2, 1, 2, 3 },
			{ 1, 2, 3, 1, 2 },
			{ 1, 2, 3, 4, 1 },
			{ 1, 2, 3, 4, 5 } };
	char prefix[100];

	for (int i = 0; i < NBITEMS; i++)
	{
		sprintf(prefix, "%s %d: ", __FUNCTION__, i);
		testShift(prefix, results[i], i * sizeof(TYPE), true);
	}
}

// ============================================================================
// ROTATE
// ============================================================================

static void testRotate(const char *prefix, TYPE result[], size_t shift, bool toTheRight)
{
	testSetup();
	ZRMemoryOp_rotate(MEM, END, shift, toTheRight);
	LOCAL_ZRTEST_RESULT(prefix);
}

MU_TEST(testRotate_left)
{
	TYPE results[][NBITEMS] =
		{
			{ 1, 2, 3, 4, 5 },
			{ 2, 3, 4, 5, 1 },
			{ 3, 4, 5, 1, 2 },
			{ 4, 5, 1, 2, 3 },
			{ 5, 1, 2, 3, 4 },
			{ 1, 2, 3, 4, 5 } };
	char prefix[100];

	for (int i = 0; i < NBITEMS * 2; i++)
	{
		sprintf(prefix, "%s %d: ", __FUNCTION__, i);
		testRotate(prefix, results[i % NBITEMS], i * sizeof(TYPE), false);
	}
}

MU_TEST(testRotate_right)
{
	TYPE results[][NBITEMS] =
		{
			{ 1, 2, 3, 4, 5 },
			{ 5, 1, 2, 3, 4 },
			{ 4, 5, 1, 2, 3 },
			{ 3, 4, 5, 1, 2 },
			{ 2, 3, 4, 5, 1 },
			{ 1, 2, 3, 4, 5 } };
	char prefix[100];

	for (int i = 0; i < NBITEMS * 2; i++)
	{
		sprintf(prefix, "%s %d: ", __FUNCTION__, i);
		testRotate(prefix, results[i % NBITEMS], i * sizeof(TYPE), true);
	}
}

// ============================================================================
// SWAP
// ============================================================================

MU_TEST(testSwap)
{
	TYPE result[NBITEMS] =
		{ 3, 2, 1, 4, 5 };
	ZRTEST_BEGIN()
	;
	ZRMemoryOp_swap(MEM, (char*)MEM + 2, sizeof(TYPE));
	LOCAL_ZRTEST_END();
}

MU_TEST(testSwapB)
{
	TYPE result[NBITEMS] =
		{ 3, 4, 1, 2, 5 };
	ZRTEST_BEGIN()
	;
	ZRMemoryOp_swapB(MEM, (char*)MEM + 2, sizeof(TYPE) * 2, &(char[2]){ 0 });
	LOCAL_ZRTEST_END();
}

// ============================================================================
// TESTS
// ============================================================================

MU_TEST_SUITE( AllTests)
{
	MU_RUN_TEST(testSwap);
	MU_RUN_TEST(testSwapB);
	MU_RUN_TEST(testShift_left);
	MU_RUN_TEST(testShift_right);
	MU_RUN_TEST(testRotate_left);
	MU_RUN_TEST(testRotate_right);
}

int MemoryOpTests(void)
{
	puts(__FUNCTION__);
	MU_SUITE_CONFIGURE(testSetup, NULL);
	MU_RUN_SUITE(AllTests);
	MU_REPORT()
	;
	return minunit_status;
}
